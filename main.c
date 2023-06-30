#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long fsize(FILE*);
void parseCode(int*, int size);
int* splice(int*, int, int, int);

int main(int argc, char* argv[]) {

	FILE* fp;
	char* fileName = "../test/test.mcCmd";
	printf("%s", fileName);

	fp = fopen(fileName, "r");

	if (fp == NULL) {
		printf_s("\n파일을 찾을 수 없습니다.");

		return 0;
	}

	int size = fsize(fp);
	int* token;
	token = malloc(sizeof(int) * size);

	if (token == NULL) {
		printf("\ntoken is NULL");
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


//define = 0, defineVariable = 1, variableArgs = 2, defineFunc = 3, funcVariable = 4, callFunc = 5 
//variableType.. null ＝ -1 int = 0, float = 1, double = 2, bool = 3, string = 4, array = 5, json = 6
struct Variable {
	char* name;
	int type;
	int valueBool;
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
	int varDeclared = 0;
	int parsingString = 0;
	struct Variable* variables;
	variables = (struct Variable*)malloc(sizeof(struct Variable) * size);
	if (variables == NULL) return;

	int funcCalled = 0;

	for (int i = 0; i < size; i++) {
		char t = token[i];
		if (parseType == -1) {
			if (t == '@') {
				parseStart = i;
				parseType = 0;
			}
			else if (t == '/') {
				parseStart = i;
				parseType = 5;
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
				if (t == '[' || t == ';') {
					if (t == ';') {
						parseType = -1;
						varDeclared = 0;
						parsingString = 0;
					}
					parseEnd = i - 1;

					struct Variable var;
					var.name = (char*)malloc(sizeof(char) * parseEnd - parseStart + 2);
					var.type = -1;

					for (int j = 0; j <= parseEnd - parseStart - 2; j++) {
						var.name[j] = token[parseStart + j + 2];
					}
					var.name[parseEnd - parseStart - 1] = '\0';

					printf("\n%s - var.name %d", var.name, varIndex);

					variables[varIndex] = var;
					varIndex++;
					varDeclared = 1;
				}

				if (varDeclared) {
					if (t == '[') {
						parseStart = i;
					}
					else if (t == '"') {
						if (!parsingString) parsingString = 1;
						else parsingString = 0;
					}
					else if ((t == ',' || t == ']') && !parsingString) {
						if (t == ']') parseType = -1;
						parseEnd = i - 1;

						char varArg[20];
						char* varValue = (char*)malloc(sizeof(char) * parseEnd - parseStart + 2);
						int a_index = 0;

						int valIndex;
						int vvIndex;
						int isValue = 0;
						for (int j = 0; j <= parseEnd - parseStart; j++) {
							int to = token[parseStart + j + 1];

							if (to == '=') {
								valIndex = j;
								isValue = 1;
								vvIndex = j - valIndex;
								continue;
							}

							if (isValue) {
								if (to == '\\') continue;
								if (token[parseStart + j] != '\\' && to == '"') {
									continue;
								}
								varValue[vvIndex] = to;
								vvIndex++;
							}
							else {
								if (to == ' ' || to == '\n') continue;

								varArg[a_index] = to;
								a_index++;
							}
						}

						varArg[a_index] = '\0';
						varValue[vvIndex - 1] = '\0';

						printf("\n%s - varArg", varArg);
						printf("\n%s - varValue", varValue);


						if (strcmp(varArg, "type") == 0) {
							if (strcmp(varValue, "int") == 0) {
								variables[varIndex - 1].type = 0;
							}
							else if (strcmp(varValue, "float") == 0) {
								variables[varIndex - 1].type = 1;
							}
							else if (strcmp(varValue, "double") == 0) {
								variables[varIndex - 1].type = 2;
							}
							else if (strcmp(varValue, "bool") == 0) {
								variables[varIndex - 1].type = 3;
							}
							else if (strcmp(varValue, "string") == 0) {
								variables[varIndex - 1].type = 4;
							}
						}
						else if (strcmp(varArg, "value") == 0) {
							switch (variables[varIndex - 1].type) {
								case 0:
									variables[varIndex - 1].valueInt = atoi(varValue);
									break;
								case 1:
									variables[varIndex - 1].valueFloat = (float)atof(varValue);
									break;
								case 2:
									variables[varIndex - 1].valueDouble = (double)atof(varValue);
									break;
								case 3:
									if (strcmp(varValue, "true") == 0) variables[varIndex - 1].valueBool = 1;
									else if (strcmp(varValue, "false") == 0) variables[varIndex - 1].valueBool = 0;

									break;
								case 4:
									variables[varIndex - 1].valueStr = varValue;
									break;
								default:
									break;
							}
						}

						if (t == ',') parseStart = i;
					}
				}
			}
			else if (parseType == 5) {
				printf("\nparse2");
				if (t == ';') {
					parseType = -1;

					char** args = (char**)malloc(sizeof(char*)* parseEnd - parseStart);
					int* argsSizes = (int*)malloc(sizeof(int)* parseEnd - parseStart);

					int argStart = 0;
					int argEnd;
					char to;

					int argIndex = 0;
					for (int j = 0; j <= parseEnd - parseStart + 1; j++) {
						to = token[parseStart + j + 1];

						if (to == ' ') {
							argEnd = j - 1;

							argsSizes[argIndex] = argEnd - argStart;
							for (int s = 0; s <= argEnd - argStart; s++) {
								args[argIndex][s] = token[parseStart + argStart + 1];
							}

							args[argIndex][parseStart + argEnd + 1] = '\0';
							printf("%s", args[argIndex]);
							argIndex++;
						}
					}
					printf("\n%s", args);

					if (strcmp(args[0], "say") == 0) {
						parsingString = 0;

						char* string = (char*)malloc(sizeof(char)* argsSizes[1]);
						int strIndex = 0;

						for (int j = 0; j <= argsSizes[1]; j++) {
							to = args[1][j];

							if (to == '"') {
								if (parsingString) {
									parsingString = 0;
								}
								else {
									parsingString = 1;
								}
							}
							else if (parsingString) {
								string[strIndex] = to;
								strIndex++;
							}
						}

						puts(string);
					}
				}
			}
		}
		printf("\n%c - %d", t, parseType);
	}

	for (int i = 0; i <= varIndex; i++) {
		if (variables[i].name == NULL) return;
		if (variables[i].type == 0) printf("\n%s(int) - %d", variables[i].name, variables[i].valueInt);
		else if (variables[i].type == 1) printf("\n%s(float) - %.3lf", variables[i].name, variables[i].valueFloat);
		else if (variables[i].type == 2) printf("\n%s(double) - %.3lf", variables[i].name, variables[i].valueDouble);
		else if (variables[i].type == 3) {
			if (variables[i].valueBool == 1) printf("\n%s(bool) - true", variables[i].name);
			else printf("\n%s(bool) - false", variables[i].name);
		}
		//else if (variables[i].type == 4) printf("\n%s(string) - %s", variables[i].name, variables[i].valueStr);
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
