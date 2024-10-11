#include "myalloc.h"

static bool new_mem = true; 

void *myalloc(size_t size) {
    /* Alloue un bloc de mémoire de taille size et renvoie un pointeur sur 
     * le bloc correspondant */

    int j;
    Bloc *temp;

    if (new_mem == true) {
        ptr_free = &small_tab[0];
        for(j = 0; j<MAX_SMALL-1; j++) {
            small_tab[j].head = &small_tab[j+1];
        }
        small_tab[MAX_SMALL-1].head = &small_tab[MAX_SMALL-1];
        new_mem = false;
    }


    if (size > SIZE_BLK_SMALL) return 0;    
    if (ptr_free == NULL) return 0;

    if(ptr_free->head == ptr_free) {
        /* End of list */
        temp = ptr_free;
        ptr_free = NULL;
        temp->head = NULL ;
    } else {
        temp = ptr_free;
        ptr_free = temp->head;
        temp->head = NULL ;
    }

    return temp->body;
}

void myfree(void *ptr) {
    /* Libère la mémoire allouée par le bloc à l'adresse pointée par ptr */

    char *ptr_typ = (char *)ptr;
    Bloc *ptr_block = (Bloc *)(ptr_typ - sizeof(Bloc *));
    int gap = sizeof(Bloc);
    int from_origin = ptr_typ - small_tab[0].body;

    if( from_origin < 0 || (from_origin) % gap != 0 || from_origin > gap*MAX_SMALL) {
        printf("Erreur : l'adresse du bloc à libérer est incorrecte\n");
    } else {
        if(ptr_free == NULL) {
            ptr_block->head = ptr_block;
            ptr_free = ptr_block;
        } else {
            ptr_block->head = ptr_free;
            ptr_free = ptr_block;
        }
    }
}

void *realloc(void *ptr, size_t size) {
    /* Réalloue le bloc associé à l'adresse pointée par ptr, pour qui'l ait une
     * taille de size */

    char *ptr_typ = (char *)ptr;
    int gap = sizeof(Bloc);
    int from_origin = ptr_typ - small_tab[0].body;

    if( from_origin < 0 || (from_origin) % gap != 0 || from_origin > gap*MAX_SMALL) {
        printf("Erreur : l'adresse du bloc à libérer est incorrecte\n");
        return 0;
    }

    if(size > SIZE_BLK_SMALL) {
        printf("Erreur : réallocation d'un bloc à une taille trop grande\n");
        return 0;
    }

    return ptr;
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

char access(void *ptr) {
    /* Accède à la donnée à l'adresse pointée par ptr si elle correspond bien à
     * l'intérieur d'un bloc */

    char *ptr_typ = ptr;
    int gap = sizeof(Bloc);
    int from_origin = ptr_typ - small_tab[0].body;

    if(from_origin < 0 || from_origin % gap > (int)SIZE_BLK_SMALL || from_origin > gap*MAX_SMALL) {
        printf("La mémoire à l'adresse pointée n'est pas allouée\n");
        return -1;
    } else {
        return *ptr_typ;
    }
}
