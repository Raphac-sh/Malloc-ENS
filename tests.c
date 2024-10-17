#include "tests.h"
#include "myalloc.h"

void test_1(void) {
    /* Vérifie que myalloc alloue bien des blocs initialement libres */

    char *ptr;
    char *prev_free = ptr_free->body;
    Bloc *prev_head = ptr_free->head;

    printf("test_1 : affichage de la mémoire avant allocation : \n");
    print_mem();

    ptr = (char *)myalloc(10);

    printf("test_1 : affichage de la mémoire après allocation : \n");
    print_mem();


    if(ptr == prev_free && prev_head != NULL) printf("test_1 : erreur, le bloc alloué n'était pas initialement libre\n");
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

    char *ptr = (char *)myalloc(10);
    Bloc *ptr_block = (Bloc *)(ptr - sizeof(Bloc *));

    printf("test_4 : affichage de la mémoire avant free : \n");
    print_mem();

    myfree(ptr);

    printf("test_4 : affichage de la mémoire après free : \n");
    print_mem();

    if(ptr_block->head == NULL) printf("test_4 : erreur, le bloc libéré n'est pas vide\n");
    else printf("test_4 : validé, free libère bien les blocs\n");

    printf("(Le bloc alloué est le dernier car on a libéré les blocs dans l'ordre croissant, ce qui est cohérent avec la structure de liste chainée \n");
    printf("\n"); 
}

void test_perf(void) {
    /* Teste les performances de la fonction myalloc lorsqu'on alloue tous les blocs */

    int i;
    clock_t time1, time2;
    float diff1, diff2;
    void *ptr_tab[MAX_SMALL];

    time1 = clock();

    for(i=0; i<MAX_SMALL; i++) {
        myalloc(SIZE_BLK_SMALL);
    }

    time2 = clock();

    diff1 = ((float)(time2 - time1) / 1000000.0F ) * 1000;
    printf("Allocations myalloc : %f ms\n", diff1);

    time1 = clock();

    for(i=0; i<MAX_SMALL; i++) {
        ptr_tab[i] = malloc(SIZE_BLK_SMALL);
    }

    time2 = clock();

    diff2 = ((float)(time2 - time1) / 1000000.0F ) * 1000;
    printf("Allocations malloc : %f ms\n", diff2);

    for(i=0; i<MAX_SMALL; i++) {
        free(ptr_tab[i]);
    }
    
    printf("Notre version est %.1f fois plus rapide !", diff2/diff1);
}
