#include "tests.h"
#include "myalloc.h"

void test_1(void) {
    /* Vérifie que l'on utilise bien une division de bloc lorsque c'est possible */
    void *ptr;
    printf("----- TEST_1 -----\n");

    myfree(myalloc(5000));
    printf("On alloue et on libère un bloc de taille 5000 :\n");
    print_big_free();

    ptr = myalloc(3000);
    printf("On alloue un bloc de taille 3000 :\n");
    print_big_free();
    printf("test_1 : Validé, On a bien divisé le bloc libre\n");

    myfree(ptr);
}

void test_2(void) {
    /* Réutilisation d'un bloc de taille proche */
    void *ptr;
    printf("----- TEST_2 -----\n");

    myfree(myalloc(5000));
    printf("On alloue et on libère un bloc de taille 5000 :\n");
    print_big_free();

    ptr = myalloc(4950);
    printf("On alloue un bloc de taille 4950 :\n");
    print_big_free();
    printf("test_2 : Validé, On a bien réalloué le bloc précédemment libéré\n");
    myfree(ptr);
}

void test_3(void) {
    /* Un bloc libre n'est pas libéré */
    void *ptr = myalloc(5000);
    printf("----- TEST_3 -----\n");

    print_big_free();
    myfree(ptr);
    printf("Première libération :\n");
    print_big_free();
    myfree(ptr);
    printf("Deuxième libération :\n");
    print_big_free();
    printf("test_3 : Validé, on ne peut pas libérer un bloc libre\n");
}

void test_4(void) {
    /* Realloc d'une taille plus grande */

    void *ptr = myalloc(5000);
    int i = 0;
    bool test_copy = true;

    for(i = 0; i<5000; i++) {
        ((char *) ptr)[i] = 'a';
    }

    printf("----- TEST_4 -----\n");

    ptr = my_realloc(ptr, 7000);
    printf("On réalloue un bloc de taille 500 à une taille plus grande :\n");
    print_big_free();

    for(i = 0; i<5000; i++) {
        if (((char *) ptr)[i] != 'a') test_copy = false;
    }

    myfree(ptr);
    printf("On libère le bloc réalloué :\n");
    print_big_free();

    if(test_copy) printf("test_4 : Validé, la réallocation d'un bloc de taille fonctionne comme attendu\n");
    else printf("test_4 : Erreur, le contenu du bloc n'est pas bien copié\n");
}

void test_5(void) {
    void *ptr = myalloc(5000);
    printf("----- TEST_5 -----\n");

    print_big_free();
    ptr = my_realloc(ptr, 3000);
    printf("On réalloue un bloc de taille 500 à une taille plus petite, le bloc est divisé :\n");
    print_big_free();
    myfree(ptr);
    printf("Après libération :\n");
    print_big_free();
}

void test_ext_1(void) {
    /* Vérifie que myalloc permet d'allouer plus de petits blocs */

    printf("----- TEST_EXT_1 -----\n");
    int i;
    void *ptr_tab[MAX_SMALL];
    void *ptr;

    for(i=0; i<MAX_SMALL; i++) {
        ptr_tab[i] = myalloc(10);
    }

    if ((ptr = myalloc(10)) != NULL) {
        printf("test_ext_1 : validé, on augmente le nombre de petits blocs lorsque la mémoire est pleine\n");
    } else {
        printf("test_2 : erreur, myalloc renvoie NULL quand la mémoire est pleine\n");
    }
    printf("\n");

    for(i=0; i<MAX_SMALL; i++) {
        myfree(ptr_tab[i]);
    }
    myfree(ptr);
}

void test_perf(void) {
    /* Teste les performances de la fonction myalloc lorsqu'on alloue tous les blocs */

    int i;
    clock_t time1, time2;
    float diff1, diff2;
    void *ptr_tab[MAX_SMALL];

    time1 = clock();

    for(i=0; i<MAX_SMALL; i++) {
        ptr_tab[i] = myalloc(500);
    }

    for(i=0; i<MAX_SMALL; i++) {
        myfree(ptr_tab[i]);
    }

    for(i=0;i<MAX_SMALL; i++) {
        myfree(myalloc(500));
    }

    time2 = clock();

    diff1 = ((float)(time2 - time1) / 1000000.0F ) * 1000;
    printf("Allocations myalloc : %f ms\n", diff1);

    time1 = clock();

    for(i=0; i<MAX_SMALL; i++) {
        ptr_tab[i] = malloc(500);
    }

    for(i=0; i<MAX_SMALL; i++) {
        free(ptr_tab[i]);
    }

    for(i=0;i<MAX_SMALL; i++) {
        free(malloc(500));
    }

    time2 = clock();

    diff2 = ((float)(time2 - time1) / 1000000.0F ) * 1000;
    printf("Allocations malloc : %f ms\n", diff2);
    
    printf("Notre version est %.1f fois plus rapide !", diff2/diff1);
}
