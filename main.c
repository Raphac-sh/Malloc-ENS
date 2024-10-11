#include "project.h"

int main(void) {
    myfree(myalloc(10));

    test_1();
    test_2();
    test_3();
    test_4();
    test_perf();

    print_mem();
    return 0;
}
