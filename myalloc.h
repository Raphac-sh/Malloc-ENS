#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_SMALL 100
#define SIZE_BLK_SMALL (128-sizeof(size_t))
#define h (2*sizeof(size_t))
#define SIZE_FIRST_BLOCK_LARGE 1024

struct bloc {
    struct bloc *head;
    char body[SIZE_BLK_SMALL];
};

struct big_bloc {
    struct big_bloc *head;
    size_t bloc_size;
    char *body;
};

typedef struct big_bloc BigBloc;
typedef struct bloc Bloc;

static Bloc small_tab[MAX_SMALL];
static BigBloc *big_free;
static Bloc *small_free;

void *myalloc(size_t);
void myfree(void *);    
void *realloc(void *, size_t);

void print_mem(void);
