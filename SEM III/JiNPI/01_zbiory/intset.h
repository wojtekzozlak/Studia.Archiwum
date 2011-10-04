#ifndef INTSET
#define INTSET

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned long intset_new();
/* Creates new set of integers and returns its ID. */

void intset_delete(unsigned long id);
/* Removes set with [id] id. If there is no such set - function does nothing. */

void intset_insert(unsigned long id, int elem);
/* Inserts element [elem] from set with [id] id. If there is no such set,
   or it already contain such element - function does nothing. */

void intset_remove(unsigned long id, int elem);
/* Removes element [elem] from set with [id] id. If there is no such set,
   or it does not contain such element - function does nothing. */

int intset_find(unsigned long id, int elem);
/* If set with [id] id exist and contain element [elem] returns 1.
   Otherwise returns 0. */

size_t intset_size(unsigned long id);
/* If set with [id] id exist, returns its size (number of elements inside).
   Otherwise returns 0. */

void intset_union(unsigned long id1, unsigned long id2, unsigned long id3);
/* Constructs a union of two sets - [id1] and [id2]. Then overrides the 
   [id1] set with it.

    IDs does not have to be different from each other, but they must refer to
    existing sets. */

void intset_difference(unsigned long id1, unsigned long id2, unsigned long id3);
/* Constructs a difference of two sets - [id1] and [id2] (elements which are in
   the [id2], but not in the [id3]). Then overrides the [id1] set with it.

    IDs does not have to be different from each other, but they must refer to
    existing sets. */

#ifdef __cplusplus
}
#endif

#endif
