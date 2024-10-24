#include <stdlib.h>

void *myalloc(size_t);
void *myalloc_big(size_t);
void myfree(void *);
void myfree_big(void *);
void *my_realloc(void *, size_t);
void *realloc_big(void *, size_t);
void print_mem(void);
