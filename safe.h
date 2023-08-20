#include <stdlib.h>

void* malloc_s(size_t size);
void* realloc_s(void* memblock, size_t size);
#ifdef __linux__
int memcpy_s(void* dest, size_t dest_size, const void* src, size_t count);
#endif
