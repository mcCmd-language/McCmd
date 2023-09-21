#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

#ifdef __linux__
int memcpy_s(void* dest, size_t dest_size, const void* src, size_t count) {
    if (dest == NULL || src == NULL) {
        fprintf(stderr, "Error in memcpy_s: Invalid arguments.\n");
        exit(1);
    }

    if (dest_size < count) {
        // Destination buffer is too small
        memset(dest, 0, dest_size); // Clear the destination buffer
        fprintf(stderr, "Error in memcpy_s: Destination buffer is too small.\n");
        exit(1);
    }

    memcpy(dest, src, count);

    return 0; // Success
}
#endif
