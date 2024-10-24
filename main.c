#include "project.h"

int main(void) {
    void *ptr = myalloc(2000);
    ptr = my_realloc(ptr, 3000);

    return 0;
}
