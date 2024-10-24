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
        temp->body = (char *)temp + h; 
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
            printf("Divise \n");

            temp = (BigBloc *) ((char *)it + it->bloc_size - (size + h + sizeof(size_t) - (size + h)%sizeof(size_t)));
            temp->bloc_size = size+h;
            temp->body = (char *)temp + h;

            it->bloc_size -= (size + h);
        }
    }

    temp->head = (BigBloc *) (1); /* Bloc alloué : bit de poid faible à 1 */

    printf("Block : %p\n", temp);
    printf("head : %p\n", temp->head);
    printf("size : %lu\n", temp->bloc_size);

    return temp->body;
}

void *myalloc_small(void) {
    /* Alloue un bloc de mémoire de taille size et renvoie un pointeur sur 
     * le bloc correspondant */

    Bloc *temp;

    if (small_free == NULL) return 0; /* Plus de blocs disponibles */

    if(small_free->head == small_free) {
        /* Fin de la liste : le dernier élément pointe sur lui même*/

        temp = small_free;
        small_free = NULL; /* Plus de blocs libres pour la prochaine allocation */
        temp->head = NULL; 
    } else {
        temp = small_free;
        small_free = temp->head; /* Bloc suivant */
        temp->head = NULL ;
    }

    return temp->body;
}

void *myalloc(size_t size) {
    int j;

    if (new_mem == true) {
        /* Premier appel de myalloc, on initialise la mémoire */

        big_free = sbrk(SIZE_FIRST_BLOCK_LARGE);
        big_free->bloc_size = SIZE_FIRST_BLOCK_LARGE;
        big_free->body = (char *)big_free + h;
        big_free->head = big_free;

        small_free = &small_tab[0];
        for(j = 0; j<MAX_SMALL-1; j++) {
            small_tab[j].head = &small_tab[j+1];
        }
        small_tab[MAX_SMALL-1].head = &small_tab[MAX_SMALL-1];
        new_mem = false;
    }

    if(size < SIZE_BLK_SMALL) {
        /* Petit bloc */
        
        return myalloc_small();
    } else {
        /* Gros bloc */

        return myalloc_big(size);
    }
}

void myfree_small(void *ptr) {
    Bloc *ptr_block = (Bloc *)((char *) ptr - sizeof(Bloc *));

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
    BigBloc *temp; 

    printf("ptr_block : %p\n", ptr_block);
    printf("head : %p\n", ptr_block->head);

    if(((size_t) ptr_block->head & 1) == 0) return; /* Bloc déjà libre */

    printf("big_free: %p\n", big_free);

    temp = big_free;
    big_free = ptr_block;
    big_free->head = temp;
    printf("new big_free: %p\n", big_free);
    printf("big_free head: %p\n", big_free->head);

}

void myfree(void *ptr) {
    /* Libère la mémoire allouée par le bloc à l'adresse pointée par ptr */

    char *ptr_typ = (char *)ptr; 
    int gap = sizeof(Bloc);
    int from_origin = ptr_typ - small_tab[0].body;

    if( from_origin < 0 || (from_origin) % gap != 0 || from_origin > gap*MAX_SMALL) {
        /* Gros bloc */
        myfree_big(ptr); 
    } else {
        /* Petit bloc */
        myfree_small(ptr);
    }
}

void copy(BigBloc *old_bloc, BigBloc *new_bloc) {
    char *old_body = old_bloc->body;
    char *new_body = new_bloc->body;
    size_t i;

    printf("copy : %p, %p\n", old_bloc, new_bloc);

    for(i = 0; i<old_bloc->bloc_size; i++) {
        new_body[i] = old_body[i];
    }
}

void *realloc_big(void *ptr, size_t size) {
    BigBloc *ptr_block = (BigBloc *)((char *) ptr - h);
    BigBloc *new_bloc; 
    BigBloc *temp;

    if(((size_t) ptr_block->head & 1) == 0) return NULL;

    if(size + h + SIZE_BLK_SMALL < ptr_block->bloc_size) {
        /* Assez petit : on coupe en deux */
        
        new_bloc = ptr_block + ptr_block->bloc_size - size - h;
        new_bloc->head = ptr_block->head;
        new_bloc->bloc_size = size+h;
        new_bloc->body = (char *)new_bloc + h;

        ptr_block->bloc_size -= (size + h);

        /* On ajoute le nouveau bloc aux blocs libres */
        temp = big_free;
        big_free = ptr_block;
        big_free->head = temp;

        return new_bloc->body;
    } else if (size - h< ptr_block->bloc_size){
        /* On diminue simplement la taille, on ne réalloue pas dans small_tab (trop couteux) */

        ptr_block->bloc_size = size;
        return ptr_block; 
    } else {
        /* Redimensionnement à une taille plus grande avec un malloc-copy-free */
        printf("Redimensionnement\n");

        new_bloc = (BigBloc *) ((char *) myalloc(size) - h);
        copy(ptr_block, new_bloc);
        myfree(ptr);


        return new_bloc->body;
    }
}

void *my_realloc(void *ptr, size_t size) {
    /* Réalloue le bloc associé à l'adresse pointée par ptr, pour qui'l ait une
     * taille de size */

    char *ptr_typ = (char *)ptr;
    int gap = sizeof(Bloc);
    int from_origin = ptr_typ - small_tab[0].body;

    if( from_origin < 0 || (from_origin) % gap != 0 || from_origin > gap*MAX_SMALL) {
        /* Gros bloc */

        return realloc_big(ptr, size);
    }
    else {
        /* Petit bloc */

        return ptr;
    }
}

void print_mem(void) {
    /* Affiche la mémoire sous forme d'un tableau */

    int i, j, k;

    for (i = 0; i < 10; i++) {
        for (j = 0; j < MAX_SMALL/10; j++) {
            k = i * (MAX_SMALL/10) + j;

            if(i==0) printf("0");
            if (small_tab[k].head == NULL) {
                printf("%d : U ", k);
            } else {
                printf("%d : _ ", k);
            }
        }
        printf("\n");
    }
}

char my_access(void *ptr) {
    /* Accède à la donnée à l'adresse pointée par ptr si elle correspond bien à
     * l'intérieur d'un bloc  : accès sécurisé */

    char *ptr_typ = ptr;
    int gap = sizeof(Bloc);
    int from_origin = ptr_typ - small_tab[0].body;

    if(from_origin < 0 || from_origin % gap > (int)SIZE_BLK_SMALL || from_origin > gap*MAX_SMALL) {
        /* Pointeur en dehors de la zone, ou ne pointe pas sur l'intérieur un body */

        printf("La mémoire à l'adresse pointée n'est pas allouée\n");
        return -1;
    } else {
        /* Le pointeur convient */

        return *ptr_typ;
    }
}
