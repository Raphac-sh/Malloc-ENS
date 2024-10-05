#include "myalloc.h"

void *myalloc(size_t size) {
    int i = 0;

    if (size > SIZE_BLK_SMALL) return 0;

    while (i < MAX_SMALL && small_tab[i].head != 0) i++;

    if (i == MAX_SMALL) return 0;

    small_tab[i].head = 1;
    return small_tab[i].body;
}

void myfree(void *ptr) {
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

void test_1(void) {
    int i = 0;
    char *ptr;
    Bloc copy[MAX_SMALL];

    printf("test_1 : affichage de la mémoire avant allocation : \n");
    print_mem();

    memcpy(copy,small_tab, MAX_SMALL);
    ptr = (char *)myalloc(10);

    printf("test_1 : affichage de la mémoire après allocation : \n");
    print_mem();

    while (i < MAX_SMALL && small_tab[i].body != ptr ) i++;

    if(copy[i].head != 0) printf("test_1 : erreur, le bloc alloué n'était pas initialement libre\n");
    else printf("test_1 : validé, myalloc alloue un bloc initialement libre\n");
    printf("\n"); 

    myfree(ptr);
}

void test_2(void) {
    int i;
    void *ptr_tab[MAX_SMALL];

    for(i=0; i<MAX_SMALL; i++) {
        ptr_tab[i] = myalloc(10);
    }
    printf("test_2 : affichage de la mémoire : \n");
    print_mem();

    if (myalloc(10) != NULL) {
        printf("test_2 : erreur, on alloue alors que la mémoire est pleine");
    } else {
        printf("test_2 : validé, myalloc renvoie NULL quand la mémoire est pleine\n");
    }
    printf("\n");

    for(i=0; i<MAX_SMALL; i++) {
        myfree(ptr_tab[i]);
    }
}


void test_3(void) {
    void *ptr = myalloc(SIZE_BLK_SMALL+1);

    if (ptr != NULL) {
        printf("test_3 : erreur, on alloue plus que la capacité maximale");
    } else {
        printf("test_3 : validé, on ne peut pas allouer plus que la taille maximale\n");
    }
    printf("\n");
}

void test_4(void) {
    int i = 0;
    char *ptr = (char *)myalloc(10);

    printf("test_4 : affichage de la mémoire avant free : \n");
    print_mem();

    myfree(ptr);

    printf("test_4 : affichage de la mémoire après free : \n");
    print_mem();

    while (i < MAX_SMALL && small_tab[i].body != ptr ) i++;

    if(small_tab[i].head != 0) printf("test_4 : erreur, le bloc libéré n'est pas vide\n");
    else printf("test_4 : validé, free libère bien les blocs\n");
    printf("\n"); 
}

void test_perf(void) {
    int i;
    clock_t time1, time2;
    float diff;

    time1 = clock();

    for(i=0; i<MAX_SMALL; i++) {
        myalloc(SIZE_BLK_SMALL);
    }

    time2 = clock();

    diff = ((float)(time2 - time1) / 1000000.0F ) * 1000;
    printf("Allocations myalloc : %f ms\n", diff);
}

int main(void) {
    test_1();
    test_2();
    test_3();
    test_4();
    test_perf();
    return 0;
}

