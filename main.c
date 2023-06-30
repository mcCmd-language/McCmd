#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long fsize(FILE*);
void parseCode(int*, int size);
int* splice(int*, int, int, int);

int main(int argc, char* argv[]) {

	FILE* fp;
	char* fileName = "./test/test.mcCmd";
	printf("%s", fileName);

	fp = fopen(fileName, "r");

	if (fp == NULL) {
		printf_s("파일을 찾을 수 없습니다.");

		return 0;
	}

	int size = fsize(fp);
	int* token;
	token = malloc(sizeof(int) * size);

	if (token == NULL) {
		printf("token is NULL");
		return 0;
	}

	int c = fgetc(fp);
	int i = 0;
	while (c != EOF) {
		token[i] = c;

		c = fgetc(fp);
		i++;
	}

	parseCode(token, size);
}

long fsize(FILE* fp) {
	int prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, prev, SEEK_SET);
	return sz;
}


//define = 0, defineVariable = 1, defineFunc = 2, funcVariable = 3, callFunc = 4

struct Variable {
	char* name;
	int type;
	int valueInt;
	char* valueStr;
	char valueChar;
	float valueFloat;
	double valueDouble;
};

void parseCode(int* token, int size) {
	printf("\n");
	int* parsedToken;
	parsedToken = malloc(sizeof(int) * size);
	int parseStart = -1;
	int parseEnd = -1;
	int parseType = -1;

	int varIndex = 0;
	struct Variable* variables;
	variables = (struct Variable*)malloc(sizeof(struct Variable) * size);

	for (int i = 0; i < size; i++) {
		char t = token[i];
		if (parseType == -1) {
			if (t == '@') {
				parseStart = i;
				parseType = 0;
			}
		}
		else {
			if (parseType == 0) {
				if (token[i + 2] == '/') {
					parseType = 2;
				}
				else {
					parseType = 1;
				}
			}
			else if (parseType == 1) {
				if (t == ' ' || t == ';') {
					if (t == ';') parseType = -1;
					int parseEnd = i - 1;

					struct Variable var;
					var.name = (char*)malloc(sizeof(char) * parseEnd - parseStart);

					for (int j = 0; j <= parseEnd - parseStart - 2; j++) {
						var.name[j] = token[parseStart + j + 2];
					}

					printf("\n%s", var.name);

					variables[varIndex] = var;
					varIndex++;
				}
			}
		}
		printf("\n%c - %d", t, parseType);
	}
}

int* splice(int* arr, int startAt, int endAt, int size) {
	int* arr2[100];
	memmove(arr2, arr, size * sizeof(int));

	int ii = 0;
	for (int i = 0; i < size; i++) {
		if (i >= startAt && i <= endAt) {
			continue;
		}
		else {
			arr2[ii] = arr[i];
			ii++;
		}
	}

	return arr2;
}
