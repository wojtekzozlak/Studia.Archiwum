#define _GNU_SOURCE
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <link.h>
#include <sys/mman.h>
#include <errno.h>
#include <dlfcn.h>
#include <stdbool.h>
#include "call_cnt.h"

/* errflow defines */

#define CHECKEQP(x,y) { int __errval = 0; if( (__errval = x) != y ) return __errval; }
#define CHECKNEQ(x,y,failval) { if( x == y ) return failval; }


/* errors */

#define CALLCNT_EIO  -1 /* I/O operation failed */
#define CALLCNT_EARG -2 /* Bad argument         */
#define CALLCNT_EMEM -3 /* Memory allocation/deallocation problem */
#define CALLCNT_EINT -4 /* Interception in progress */


/* -- structures -- */

typedef enum r_record_type_t { INTERNAL, EXTERNAL } r_record_type;

typedef struct plt_snippet_t {
  /* mov __addr__ %eax    */
  /* lock addl 0x1 (%eax) */
  /* push __addr__        */
  /* ret                  */
  char bytes1[1];
  Elf32_Addr counter;
  char bytes2[5];
  Elf32_Addr function;
  char bytes3[1];
} __attribute__((packed)) plt_snippet;

typedef struct r_record_t {
  Elf32_Addr got;           /* GOT record address       */
  ssize_t counter;          /* calls counter            */
  plt_snippet snippet;      /* snippet code             */
  r_record_type type;       /* record type              */
  struct r_record_t *next;  /* next interception record */
} r_record;

struct call_cnt {
  bool intercepting; /* is intercepting in progress        */
  Elf32_Addr addr;   /* address of library in memory       */
  ssize_t internal;  /* number of internal calls           */
  ssize_t external;  /* number of external calls           */
  r_record *first;   /* first interception record          */
  r_record *last;    /* last interception record           */
  char *location;    /* library name                       */
};


/* -- internal functions -- */

r_record_type check_type(Elf32_Addr fun, struct dl_phdr_info *info)
/* Returns record type. */
{
  Elf32_Phdr *phdr = (Elf32_Phdr *) info->dlpi_phdr;
  r_record_type ret = EXTERNAL;
  int i;

  for(i = 0; i < info->dlpi_phnum; i++)
    if(info->dlpi_addr + phdr[i].p_vaddr <= fun &&
       fun < info->dlpi_addr + phdr[i].p_vaddr + phdr[i].p_memsz){
      ret = INTERNAL;
      break;
    }

  return ret;
}

int add_record(Elf32_Rel *record, struct call_cnt *ld,
               struct dl_phdr_info *info)
/* Creates an interception record.                *
 *                                                *
 * Returns 0 on success, CALLCNT_EMEM on failure. */
{
  r_record *rec;
  Elf32_Addr got, fun;
  r_record_type type;

  /* x86 does not check executable flag, so we don't have to set it *
   * (though probably it would be nicer to use mmap/mprotect...)    */
  rec = malloc(sizeof(r_record));
  if(NULL == ld) return CALLCNT_EMEM;

  got = ld->addr + record->r_offset;
  fun = *((Elf32_Addr *)(ld->addr + record->r_offset));
  type = check_type(fun, info);

  *rec = (r_record){
      got, 0, /* GOT address; counter */
      {
        {0xb8}, (Elf32_Addr)(&rec->counter), /* mov __addr__ %eax    */
        {0xf0, 0x83, 0x00, 0x01,             /* lock addl 0x1 (%eax) */
         0x68}, fun,                         /* push __addr__        */
        {0xc3}                               /* ret                  */
      },
      type, NULL /* type; next record */
    };

  /* record registration */
  if(ld->last == NULL) ld->last = rec;
  else {
    ld->last->next = rec;
    ld->last = rec;
  }
  ld->first = ld->first == NULL ? rec : ld->first;
  *((Elf32_Addr *)rec->got) = (Elf32_Addr)&rec->snippet;

  return 0;
}

int parse_library(struct dl_phdr_info *info, size_t size, void *data)
/* A callback function for `dl_iterate_phdr`.                          *
 *                                                                     *
 * Checks if a given library info matches given name. If so - fills up *
 * interception sctructures for that library.                          *
 *                                                                     *
 * Returns 0 if there's no match, 1 on success and <0 on error.        */
{
  struct call_cnt *ld = (struct call_cnt *) data;
  Elf32_Phdr *phdr;
  Elf32_Dyn *dyn;
  Elf32_Rel *rel;
  Elf32_Word rel_records;
  char *rel_base = NULL;
  unsigned int i, j;

  if(strstr(info->dlpi_name, ld->location) != NULL)
  /* Our library! */
  {
    ld->addr = info->dlpi_addr;
    phdr = (Elf32_Phdr *) info->dlpi_phdr;

    for(i = 0; i < info->dlpi_phnum; i++){
      if(PT_DYNAMIC == phdr[i].p_type){
        dyn = (Elf32_Dyn *) (info->dlpi_addr + phdr[i].p_vaddr);
        for(j = 0; dyn[j].d_tag != 0; j++){
          /* seek for relocation related records */
          if(DT_PLTRELSZ == dyn[j].d_tag) rel_records = dyn[j].d_un.d_val;
          else if(DT_PLTREL == dyn[j].d_tag)
            size = DT_REL == dyn[j].d_un.d_val ?
                sizeof(Elf32_Rel) : sizeof(Elf32_Rela);
          else if(DT_JMPREL == dyn[j].d_tag)
            rel_base = (char *) dyn[j].d_un.d_ptr;
        }
        break;
      }
    }

    if(NULL == rel_base) return 1; /* no relocation records */

    for(i = 0; i < rel_records / size; i++){ /* patching */
        rel = (Elf32_Rel *) (rel_base + size * i);
        CHECKEQP(add_record(rel, ld, info), 0);
    }

    return 1;
  }
  return 0;
}


/* -- public functions -- */

int intercept(struct call_cnt **desc, char const *lib_name)
/* Starts an interception of function calls in `lib_name` dynamic loaded *
 * library, performed through a PLT table. Interception descriptor is    *
 * written into a `desc` argument                                        *
 *                                                                       *
 * Returns 0 on success, <0 otherwise                                    */
{
  int retval;
  struct call_cnt *ld = malloc(sizeof(struct call_cnt));
  if(NULL == ld) return CALLCNT_EMEM;

  *ld = (struct call_cnt){
    true, 0, 0, 0, NULL, NULL,
    (char *) lib_name
  };

  retval = dl_iterate_phdr(parse_library, ld);
  CHECKNEQ(retval, 0, CALLCNT_EARG);
  CHECKEQP(retval, 1);
  *desc = ld;

  return 0;
}

int print_stats_to_stream(FILE *stream, struct call_cnt *desc)
/* Prints stats of a given interception into a given stream. */
{
  Dl_info info;
  r_record *rec = desc->first;

  while(NULL != rec){
    if(0 != dladdr((void *)rec->snippet.function, &info)){
      fprintf(stream, "%s: ", info.dli_sname);
    } else fprintf(stream, "0x%x: ", rec->snippet.function);
    fprintf(stream, "%d\n", rec->counter);
    rec = rec->next;
  }

  return 0;
}

int release_stats(struct call_cnt *desc)
/* Releases memory allocated by `intercept` function and destroys interception *
 * descriptor.                                                                 *
 *                                                                             *
 * Returns 0 on succeess and CALLCNT_EINT when interception still is in        *
 * progress.                                                                   */
{
  r_record *buff, *rec = desc->first;

  if(desc->intercepting) return CALLCNT_EINT;
  while(NULL != rec){
    buff = rec;
    rec = rec->next;
    free(buff);
  }
  free(desc);
  return 0;
}

int stop_intercepting(struct call_cnt *desc)
/* Stops call interception and revert GOT related changes in intercepted *
 * library.                                                              *
 *                                                                       *
 * Returns 0.                                                            */
{
  r_record *rec = desc->first;
  desc->internal = desc->external = 0;

  if(!desc->intercepting) return 0;

  while(NULL != rec){
    if(INTERNAL == rec->type) desc->internal += rec->counter;
    else desc->external += rec->counter;
    *((Elf32_Addr *)rec->got) = (Elf32_Addr) rec->snippet.function;
    rec = rec->next;
  }

  desc->intercepting = false;
  return 0;
}

ssize_t get_num_intern_calls(struct call_cnt *desc)
/* Returns a number of internal PLT calls in intercepted library or *
 * CALLCNT_EINT if interception is still in progress.               */
{
  return (desc->intercepting ? CALLCNT_EINT : desc->internal);
}

ssize_t get_num_extern_calls(struct call_cnt *desc)
/* Returns a number of external PLT calls in intercepted library or *
 * CALLCNT_EINT if interception is still in progress.               */
{
  return (desc->intercepting ? CALLCNT_EINT : desc->external);
}
