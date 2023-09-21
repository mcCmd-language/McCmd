#include "parseCode.h"
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "safe.h"
#ifdef __linux__
#define scanf_s scanf
#define printf_s printf
#endif
#define malloc malloc_s
#define realloc realloc_s
#define memcpy memcpy_s

void test(struct Variable* var, struct Variable* varOrigin) {
	printf("%d + %d", var[0].valueInt, var[1].valueInt);
}

int main(int argc, char* argv[]) {
	Mall();

	addLib("test");
	addLibFunction("test", "add", 2, test);

	parseMain(argc, argv);

	Print();
}