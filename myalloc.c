#include "myalloc.h"

static Bloc small_tab[MAX_SMALL];

void *myalloc(size_t size) {
    /* Alloue un bloc de mémoire de taille size et renvoie un pointeur sur 
     * le bloc correspondant */

    int i = 0;

    if (size > SIZE_BLK_SMALL) return 0;

    while (i < MAX_SMALL && small_tab[i].head != 0) i++;

    if (i == MAX_SMALL) return 0;

    small_tab[i].head = 1;
    return small_tab[i].body;
}

void myfree(void *ptr) {
    /* Libère la mémoire allouée par le bloc à l'adresse pointée par ptr */

    char *ptr_typ = (char *)ptr;
    int gap = sizeof(Bloc);
    int from_origin = ptr_typ - small_tab[0].body;

    if( from_origin < 0 || (from_origin) % gap != 0 || from_origin > gap*MAX_SMALL) {
        printf("Erreur : l'adresse du bloc à libérer est incorrecte\n");
    } else {
        *(size_t *)(ptr_typ - sizeof(size_t)) = 0;
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
            if (small_tab[k].head) {
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
