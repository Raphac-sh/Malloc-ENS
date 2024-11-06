#include "myalloc.h"
#include <unistd.h>

/* Constante booléenne à true pour l'initialisation automatique de la mémoire
 * lors de la première allocation */
static bool new_mem = true; 
static BigBloc *last_allocated = NULL;

void *myalloc_big(size_t size) {
    /* Alloue un bloc de mémoire de taille size et renvoie un pointeur sur 
     * le bloc correspondant */

    BigBloc *it;
    BigBloc *prev;
    BigBloc *temp;
    size_t real_size;


    /* Cherche un bloc de taille assez grande */
    it = big_free; 
    prev = NULL;
    while (it->bloc_size < size + h && it->head != it) {
        prev = it;
        it = it->head;
    }

    if(it->bloc_size < size + h || big_free == NULL) {
        /* Pas de bloc de taille assez grande */

        real_size = size + h + sizeof(size_t) - (size + h)%sizeof(size_t);
        temp = sbrk(real_size);
        temp->bloc_size = real_size;

        temp->prev = last_allocated;
        temp->next = NULL;      
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

            real_size = size + h + sizeof(size_t) - (size + h)%sizeof(size_t);
            temp = (BigBloc *) ((char *)it + it->bloc_size - (real_size));
            temp->bloc_size = real_size;
            it->bloc_size -= real_size;

            temp->next = it->next;
            temp->prev = it;
            it->next = temp;
        }
    }


    temp->prev_big_free = NULL;
    temp->head = (BigBloc *) (1); /* Bloc alloué : bit de poid faible à 1 */

    return (char *)temp + h; 
}

void myfree_big(void *ptr) {
    /* Libère la mémoire allouée pour le gros bloc pointé par ptr */

    BigBloc *ptr_block = (BigBloc *)((char *) ptr - h);
    bool fusion = false;

    if(((size_t) ptr_block->head & 1) == 0) return; /* Bloc déjà libre */

   
    if(ptr_block->prev != NULL && ((size_t) ptr_block->prev->head & 1) == 0) {
        /* On fusionne avec le bloc précédent */

        ptr_block = ptr_block->prev;
        ptr_block->bloc_size += ptr_block->next->bloc_size;
        ptr_block->next = ptr_block->next->next;

        fusion = true;
    } 

    if(ptr_block->next != NULL && ((size_t) ptr_block->next->head & 1) == 0) {
        /* Fusion avec le bloc suivant */

        ptr_block->bloc_size += ptr_block->next->bloc_size; 
        if(ptr_block->next->prev_big_free != NULL) ptr_block->next->prev_big_free->head = ptr_block;
        ptr_block->head = ptr_block->next->head;
        ptr_block->next = ptr_block->next->next;
        fusion = true;
    }
    

    if(fusion == false) {
        if(big_free == NULL) {
            ptr_block->head = ptr_block;
            big_free = ptr_block;
        } else {
            ptr_block->head = big_free;
            big_free->prev_big_free = ptr_block;
            big_free = ptr_block;
        }
    }   
}

void copy_big(BigBloc *old_bloc, BigBloc *new_bloc) {
    /* Copie un gros bloc vers un autre de taille plus grande */

    char *old_body = (char*) old_bloc + h;
    char *new_body = (char*) new_bloc + h;
    size_t i;

    for(i = 0; i<(old_bloc->bloc_size - h); i++) {
        new_body[i] = old_body[i];
    }
}

void copy_small(Bloc *old_bloc, BigBloc *new_bloc) {
    /* Copie un petit bloc vers un gros bloc */

    char *old_body = old_bloc->body;
    char *new_body = (char*) new_bloc + h;
    size_t i;

    for(i = 0; i<SIZE_BLK_SMALL; i++) {
        new_body[i] = old_body[i];
    }
}

void *realloc_big(void *ptr, size_t size) {
    /* Réallocateur de gros bloc : réalloue le gros bloc alloué dont le corps est
     * à l'adresse ptr, à une taille de size */
    
    BigBloc *ptr_block = (BigBloc *)((char *) ptr - h);
    BigBloc *new_bloc; 
    size_t real_size;

    if(((size_t) ptr_block->head & 1) == 0) return NULL; /* NULL si le bloc est libre */

    if(size + h + SIZE_BLK_SMALL < ptr_block->bloc_size) {
        /* Assez petit : on coupe en deux */
        real_size = size + h + sizeof(size_t) - (size + h)%sizeof(size_t);

        new_bloc = (BigBloc *) ((char *)ptr_block + ptr_block->bloc_size - (real_size));
        new_bloc->head = ptr_block->head;
        new_bloc->bloc_size = real_size;
        ptr_block->bloc_size -= real_size;

        /* On ajoute le nouveau bloc aux blocs libres */
        ptr_block->head = big_free;
        big_free = ptr_block;

        new_bloc->next = ptr_block->next;
        new_bloc->prev = ptr_block;
        ptr_block->next = new_bloc;

        return (char *)new_bloc + h;
    } else if (size - h < ptr_block->bloc_size){
        /* On diminue simplement la taille, on ne réalloue pas dans small_tab (trop couteux) */

        ptr_block->bloc_size = size;
        return ptr_block; 
    } else {
        /* Redimensionnement à une taille plus grande avec un malloc-copy-free */

        new_bloc = (BigBloc *) ((char *) myalloc(size) - h);
        copy_big(ptr_block, new_bloc);
        myfree(ptr);

        return (char *)new_bloc + h;
    }
}

void print_big_free(void) {
    /* Fonction utilitaire qui affiche les blocs libres de la liste big_free */

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

/* Les fonctions suivantes sont celles de la partie 2, ainsi que des fonctions
 * générales qui appellent les fonctions associées au gros bloc ou aux petits
 * blocs en fonction de la taille (pour cela on a rajouté un champ bloc_size
 * dans la struct small_bloc). */

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



void *my_realloc(void *ptr, size_t size) {
    /* Réalloue le bloc associé à l'adresse pointée par ptr, pour qu'il ait une
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
