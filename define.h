#pragma once
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
//#define malloc malloc_s
#define realloc realloc_s