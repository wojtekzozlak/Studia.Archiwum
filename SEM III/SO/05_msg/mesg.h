#define	MAXMESGDATA 4000

typedef struct {
  long	mesg_type;	
  char	mesg_data[MAXMESGDATA];
} Mesg;

#define	MKEY	1234L
