#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>

#define MAX_SMALL 1000
#define h (5*sizeof(size_t))
#define SIZE_BLK_SMALL (128-h)
#define SIZE_FIRST_BLOCK_LARGE 1024

/* Structure pour les petits blocs */
struct bloc {
    struct bloc *head;
    size_t bloc_size;
    char body[SIZE_BLK_SMALL];
};

/* Structure pour les gros blocs */
struct big_bloc {
    struct big_bloc *head;

    /* Utiles pour la fusion de bloc */
    struct big_bloc *prev;
    struct big_bloc *next;
    struct big_bloc *prev_big_free;

    size_t bloc_size;
};

typedef struct big_bloc BigBloc;
typedef struct bloc Bloc;

static Bloc small_tab[MAX_SMALL];
static BigBloc *big_free;
static Bloc *small_free;

void *myalloc(size_t);
void myfree(void *);    
void *my_realloc(void *, size_t);
void print_big_free(void);

void *sbrk(intptr_t increment);
