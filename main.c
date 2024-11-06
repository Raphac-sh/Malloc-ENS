#include "project.h"

int main(void) {
    myfree(my_realloc(myalloc(5000),7000));

    return 0;
}
