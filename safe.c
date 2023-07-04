#include <stdlib.h>
#include <stdio.h>

void* malloc_s(size_t size) {
    void* ptr = malloc(size);
    if (ptr == NULL) {
        printf("malloc_s error while allocation %lu sized memory", size);
        exit(1);
    }
    return ptr;
};
void* realloc_s(void* memblock, size_t size) {
    void* ptr = realloc(memblock, size);
    if (ptr == NULL) {
        printf("realloc_s error while allocation %lu sized memory", size);
        exit(1);
    }
    return ptr;
};
