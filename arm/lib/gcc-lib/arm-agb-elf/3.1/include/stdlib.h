#ifndef __stdlib_h
#define __stdlib_h

#define __need_size_t
#include <stddef.h>

#if defined(__USE_FIXED_PROTOTYPES__) || defined(__cplusplus) || defined (__STRICT_ANSI__)
#ifndef abort
extern void abort (void);
#endif
extern int abs (int);
extern int atexit (void (*) (void));
extern double atof (const char *);
extern int atoi (const char *);
extern long int atol (const char *);
extern void * bsearch (const void *, const void *, __SIZE_TYPE__, __SIZE_TYPE__, int (*) (const void *, const void *));
extern void * calloc (__SIZE_TYPE__, __SIZE_TYPE__);
extern void exit (int);
extern void free (void *);
extern char * getenv (const char *);
extern long int labs (long int);
extern void * malloc (__SIZE_TYPE__);
extern int putenv (char *);
extern void qsort (void *, __SIZE_TYPE__, __SIZE_TYPE__, int (*) (const void *, const void *));
extern int rand (void);
extern void * realloc (void *, __SIZE_TYPE__);
extern void srand (unsigned int);
extern double strtod (const char *, char **);
extern long int strtol (const char *, char **, int);
extern long unsigned int strtoul (const char *, char **, int);
extern int system (const char *);
#endif /* defined(__USE_FIXED_PROTOTYPES__) || ... */
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#endif /* __stdlib_h */
