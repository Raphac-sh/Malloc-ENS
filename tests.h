#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_SMALL 1000

void test_ext_1(void);
void test_ext_2(void);
void test_1(void);
void test_2(void);
void test_3(void);
void test_4(void);
void test_5(void);
void test_perf(void);

void *myalloc(size_t);
void myfree(void *);    
void *my_realloc(void *, size_t);
void print_big_free(void);
