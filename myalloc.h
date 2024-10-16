#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define MAX_SMALL 100
#define SIZE_BLK_SMALL (128-sizeof(size_t))

struct bloc {
    struct bloc *head;
    char body[SIZE_BLK_SMALL];
};

typedef struct bloc Bloc;

static Bloc small_tab[MAX_SMALL];
static Bloc *ptr_free = &small_tab[0];


void *myalloc(size_t);
void myfree(void *);    
void *realloc(void *, size_t);

void print_mem(void);
