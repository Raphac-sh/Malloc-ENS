#include "tests.h"
#include "myalloc.h"

void test_1(void) {
    /* Vérifie que myalloc alloue bien des blocs initialement libres */

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
    /* Vérifie que myalloc renvoie un pointeur NULL quand la mémoire est pleine */

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
    /* Vérifie que myalloc n'alloue pas de mémoire quand size est supérieure à
     * la taille maximale */
    
    void *ptr = myalloc(SIZE_BLK_SMALL+1);

    if (ptr != NULL) {
        printf("test_3 : erreur, on alloue plus que la capacité maximale");
    } else {
        printf("test_3 : validé, on ne peut pas allouer plus que la taille maximale\n");
    }
    printf("\n");
}

void test_4(void) {
    /* Vérifie que myfree libère bien les blocs */

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
    /* Teste les performances de la fonction myalloc lorsqu'on alloue tous les blocs */

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
