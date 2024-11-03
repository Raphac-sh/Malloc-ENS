#include "myalloc.h"
#include <unistd.h>

static bool new_mem = true; 

void *myalloc_big(size_t size) {
    /* Alloue un bloc de mémoire de taille size et renvoie un pointeur sur 
     * le bloc correspondant */

    BigBloc *it;
    BigBloc *prev;
    BigBloc *temp;


    /* Cherche un bloc de taille assez grande */
    it = big_free; 
    prev = NULL;
    while (it->bloc_size < size + h && it->head != it) {
        prev = it;
        it = it->head;
    }

    if(it->bloc_size < size + h || big_free == NULL) {
        /* Pas de bloc de taille assez grande */

        temp = sbrk(size + h + sizeof(size_t) - (size + h)%sizeof(size_t));
        temp->bloc_size = size + h + sizeof(size_t) - (size+h)%sizeof(size_t);
    } else {
        if(it->bloc_size < size + h + SIZE_BLK_SMALL) {
            /* Taille proche de celle demandée */

            if(prev == NULL) {
                /* Premier bloc libre */

                if(it->head == it) {
                    /* Un seul bloc libre */

                    temp = it;
                } else {
                    /* On fait pointer big_free sur le suivant */

                    temp = it;
                    big_free = big_free->head;
                }
            } else {
                if(it->head == it) {
                    /* Dernier bloc de la liste */

                    temp = it;
                    prev->head = prev;
                } else {
                    /* On fait pointer le précédent sur le suivant */

                    temp = it;
                    prev->head = it->head;
                }
            }
        } else {
            /* On divise le bloc en deux */

            temp = (BigBloc *) ((char *)it + it->bloc_size - (size + h + sizeof(size_t) - (size + h)%sizeof(size_t)));
            temp->bloc_size = (size + h + sizeof(size_t) - (size + h)%sizeof(size_t));

            it->bloc_size -= (size + h + (size + h)%sizeof(size_t));
        }
    }

    temp->head = (BigBloc *) (1); /* Bloc alloué : bit de poid faible à 1 */

    return (char *)temp + h; 
}

void *myalloc_small(void) {
    /* Alloue un bloc de mémoire de taille size et renvoie un pointeur sur 
     * le bloc correspondant */

    Bloc *temp;
    int j;

    if (small_free == NULL) {
        /* Plus de blocs disponibles */

        small_free = (Bloc *)sbrk(MAX_SMALL * (sizeof(Bloc)));
        for(j = 0; j<MAX_SMALL-1; j++) {
            small_free[j].head = &small_tab[j+1];
            small_free[j].bloc_size = SIZE_BLK_SMALL;
        }
        small_tab[MAX_SMALL-1].head = &small_tab[MAX_SMALL-1];
        small_free[MAX_SMALL-1].bloc_size = SIZE_BLK_SMALL;
    }

    if(small_free->head == small_free) {
        /* Fin de la liste : le dernier élément pointe sur lui même*/

        temp = small_free;
        small_free = NULL; /* Plus de blocs libres pour la prochaine allocation */
        temp->head = NULL; 
        temp->bloc_size = SIZE_BLK_SMALL;
    } else {
        temp = small_free;
        small_free = temp->head; /* Bloc suivant */
        temp->head = NULL ;
        temp->bloc_size = SIZE_BLK_SMALL;
    }

    return temp->body;
}

void *myalloc(size_t size) {
    int j;

    if (new_mem == true) {
        /* Premier appel de myalloc, on initialise la mémoire */

        big_free = sbrk(SIZE_FIRST_BLOCK_LARGE);
        big_free->bloc_size = SIZE_FIRST_BLOCK_LARGE;
        big_free->head = big_free;

        small_free = &small_tab[0];
        for(j = 0; j<MAX_SMALL-1; j++) {
            small_tab[j].head = &small_tab[j+1];
        }
        small_tab[MAX_SMALL-1].head = &small_tab[MAX_SMALL-1];
        new_mem = false;
    }

    if(size <= SIZE_BLK_SMALL) {
        /* Petit bloc */
        
        return myalloc_small();
    } else {
        /* Gros bloc */

        return myalloc_big(size);
    }
}

void myfree_small(void *ptr) {
    Bloc *ptr_block = (Bloc *)((char *) ptr - h);

    if(small_free == NULL) {
        /* Aucun bloc disponible avant le free */

        ptr_block->head = ptr_block; /* C'est le dernier bloc disponible : pointe sur lui même */
        small_free = ptr_block;
    } else {
        /* On ajoute le bloc libéré à la tête de liste */

        ptr_block->head = small_free;
        small_free = ptr_block; 
    }
}

void myfree_big(void *ptr) {
    BigBloc *ptr_block = (BigBloc *)((char *) ptr - h);

    if(((size_t) ptr_block->head & 1) == 0) return; /* Bloc déjà libre */

    if(big_free == NULL) {
        ptr_block->head = ptr_block;
        big_free = ptr_block;
    } else {
        ptr_block->head = big_free;
        big_free = ptr_block;
    }
}

void myfree(void *ptr) {
    /* Libère la mémoire allouée par le bloc à l'adresse pointée par ptr */

    BigBloc *ptr_block = (BigBloc *)((char *) ptr - h);

    if(ptr_block->bloc_size > SIZE_BLK_SMALL) {
        /* Gros bloc */
        myfree_big(ptr); 
    } else {
        /* Petit bloc */
        myfree_small(ptr);
    }
}

void my_copy(BigBloc *old_bloc, BigBloc *new_bloc) {
    char *old_body = (char*) old_bloc + h;
    char *new_body = (char*) new_bloc + h;
    size_t i;

    for(i = 0; i<(old_bloc->bloc_size - h); i++) {
        new_body[i] = old_body[i];
    }
}

void copy_small(Bloc *old_bloc, BigBloc *new_bloc) {
    char *old_body = old_bloc->body;
    char *new_body = (char*) new_bloc + h;
    size_t i;

    for(i = 0; i<SIZE_BLK_SMALL; i++) {
        new_body[i] = old_body[i];
    }
}

void *realloc_big(void *ptr, size_t size) {
    BigBloc *ptr_block = (BigBloc *)((char *) ptr - h);
    BigBloc *new_bloc; 

    if(((size_t) ptr_block->head & 1) == 0) return NULL;

    if(size + h + SIZE_BLK_SMALL < ptr_block->bloc_size) {
        /* Assez petit : on coupe en deux */

        new_bloc = (BigBloc *) ((char *)ptr_block + ptr_block->bloc_size - (size + h + sizeof(size_t) - (size + h)%sizeof(size_t)));
        new_bloc->head = ptr_block->head;
        new_bloc->bloc_size = size + h + sizeof(size_t) - (size + h)%sizeof(size_t);

        ptr_block->bloc_size -= size + h + (size + h)%sizeof(size_t);

        /* On ajoute le nouveau bloc aux blocs libres */
        ptr_block->head = big_free;
        big_free = ptr_block;

        return (char *)new_bloc + h;
    } else if (size - h < ptr_block->bloc_size){
        /* On diminue simplement la taille, on ne réalloue pas dans small_tab (trop couteux) */

        ptr_block->bloc_size = size;
        return ptr_block; 
    } else {
        /* Redimensionnement à une taille plus grande avec un malloc-copy-free */

        new_bloc = (BigBloc *) ((char *) myalloc(size) - h);
        my_copy(ptr_block, new_bloc);
        myfree(ptr);

        return (char *)new_bloc + h;
    }
}

void print_big_free(void) {
    BigBloc *it;
    int i = 0;

    it = big_free; 
    if (it->head == it) printf("big_free vide\n");

    while (it->head != it) {
        printf("Bloc libre %d : %lu\n", i, it->bloc_size);
        it = it->head;
        i++;
    }
}

void *my_realloc(void *ptr, size_t size) {
    /* Réalloue le bloc associé à l'adresse pointée par ptr, pour qui'l ait une
     * taille de size */

    BigBloc *new_bloc; 
    BigBloc *ptr_block = (BigBloc *)((char *) ptr - h);
    Bloc *small_bloc;

    if(ptr_block->bloc_size > SIZE_BLK_SMALL) {
        /* Gros bloc */

        return realloc_big(ptr, size);
    }
    else {
        /* Petit bloc */
        if (size < SIZE_BLK_SMALL) return ptr;
        else {
            small_bloc = (Bloc *)((char *) ptr - h);

            new_bloc = (BigBloc *) ((char *) myalloc(size) - h);
            copy_small(small_bloc, new_bloc);
            myfree(ptr);

            return (char *) new_bloc + h;
        }
    }
}
