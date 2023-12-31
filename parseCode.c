#define _CRT_SECURE_NO_WARNINGS
#include "parseCode.h";

//define = 0, defineVariable = 1, variableArgs = 2, defineFunc = 3, funcVariable = 4, callFunc = 5 
//variableType.. null = -1 int = 0, float = 1, double = 2, bool = 3, string = 4, array = 5, json = 6

int IsAccessableVariable(struct Variable var) {
	if (!var.declared) return 0;
	else if (var.type > 6 || var.type <= -1) return 0;
	else return 1;
}

int getNumLength(float num) {
	int count = 1;
	while (num > 10) {
		num /= 10;
		count++;
	}
	while (num < -10) {
		num *= 10;
		count++;
	}

	return count + 1;
}

char* getDir(char* path) {
	int pathLength = strlen(path);
	unsigned short fileNameEndIdx = 0;

	char* mpFilePath;

	for (int i = pathLength; i > 0; i--) {
		if (path[i] == '\\') {
			fileNameEndIdx = i;
			break;
		}
	}

	mpFilePath = (char*)malloc(sizeof(char) * fileNameEndIdx + 2);
	memcpy_s(mpFilePath, fileNameEndIdx, path, fileNameEndIdx);
	mpFilePath[fileNameEndIdx + 1] = '\0';

	return mpFilePath;
}

int shouldFree = 0;
int libCount = 0;
int libFuncCount = 0;
char** lib;
char** libConnected;
struct LibFunc* libFuncs;
struct Variable* variables;
int varSize = 0;

void Print() {
	printf("\n%s", libFuncs[0].libWithName);
}

void Mall() {
	libFuncs = malloc(sizeof(struct LibFunc) * 1);
	variables = malloc(sizeof(struct Variable) * 1);
}

int parseMain(int argc, char* argv[]) {

	FILE* fp;
	//char* fileName = argv[1];
	//char *fileName = "./test/ex1.mcCmd";
	//printf("%s\n", fileName);

	if (argc < 2) {
		printf("\nplease enter file path");

		return 0;
	}

	char* path = getDir(argv[1]);
	printf("%s", path);
	free(path);

	//if (!fileName) return;

	fp = fopen(argv[1], "r");

	if (fp == NULL) {
		printf("\n%s doesn't exist", argv[1]);

		return 0;
	}

	int size = fsize(fp);
	char* token;
	token = malloc(sizeof(char) * size);

	if (token == NULL) {
		printf("\ntoken is NULL");
		return 0;
	}

	char c = fgetc(fp);
	int i = 0;
	while (c != EOF) {
		token[i] = c;

		c = fgetc(fp);
		i++;
	}

	variables = (struct Variable*)malloc(sizeof(struct Variable) * size);
	varSize = size;
	if (variables == NULL) return 0;

	parseCode(token, size);


	printf("\n\n");

	for (int i = 0; i <= varSize; i++) {
		if (variables[i].name == NULL) return 0;
		if (!IsAccessableVariable(variables[i])) return 0;
		if (variables[i].type == 0) printf("\n%s(int) - %d", variables[i].name, variables[i].valueInt);
		else if (variables[i].type == 1) printf("\n%s(float) - %.3lf", variables[i].name, variables[i].valueFloat);
		else if (variables[i].type == 2) printf("\n%s(double) - %.3lf", variables[i].name, variables[i].valueDouble);
		else if (variables[i].type == 3) {
			if (variables[i].valueBool == 1) printf("\n%s(bool) - true", variables[i].name);
			else printf("\n%s(bool) - false", variables[i].name);
		}
		else if (variables[i].type == 4) printf("\n%s(string) - %s", variables[i].name, variables[i].valueStr);
	}

	free(variables);
	free(token);
}

long fsize(FILE* fp) {
	int prev = ftell(fp);
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, prev, SEEK_SET);
	return sz;
}

void parseCode(char* token, int size) {

	int* parsedToken;
	parsedToken = malloc(sizeof(int) * size);
	int parseStart = -1;
	int parseEnd = -1;
	int parseType = -1;

	int varIndex = 0;
	int varDeclared = 0;
	int parsingString = 0;
	int parsingBlock = 0;

	int funcCalled = 0;

	int inCodeOfBlock = 0;

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
			} else if (t == '#' && token[i+1] == '#') {
				while (token[i] != '\n') {
					i++;
				}
				parseStart = i;
				continue;
			}
		}
		else {
			if (parseType == 0) {
				if (token[i + 1] == '/') {
					parseType = 3;
					printf("defining function");
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
					var.name = (char*)malloc(sizeof(char) * (parseEnd - parseStart + 2));
					var.type = -1;

					for (int j = 0; j <= parseEnd - parseStart - 2; j++) {
						var.name[j] = token[parseStart + j + 2];
					}
					var.name[parseEnd - parseStart - 1] = '\0';

					//printf("\n%s - var.name %d", var.name, varIndex);

					var.declared = 1;

					variables[varIndex] = var;
					varIndex++;
					varDeclared = 1;
				}

				if (varDeclared) {
					if (t == '[') {
						parseStart = i;
					}
					else if (t == '"') {
						if (token[i - 1] == '\\') {
							continue;
						}
						if (!parsingString) parsingString = 1;
						else parsingString = 0;
					}
					else if ((t == ',' || t == ']') && !parsingString) {
						if (t == ']') parseType = -1;
						parseEnd = i - 1;

						char varArg[20];
						char* varValue = (char*)malloc(sizeof(char) * (parseEnd - parseStart + 2));
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

						//printf("\n%s - varArg", varArg);
						//printf("\n%s - varValue", varValue);

						struct ParseTextResult parse;


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
									parse = ParseString(varValue, 0, parseEnd - parseStart);

									variables[varIndex - 1].valueStr = (char*)malloc(sizeof(char) * parse.size);

									for (int j = 0; j < parse.size; j++) {
										variables[varIndex - 1].valueStr[j] = parse.parsed[j];
									}
									variables[varIndex - 1].valueStr[parse.size + 1] = '\0';
									variables[varIndex - 1].strSize = parse.size + 1;

									break;
								default:
									break;
							}
						}

						if (t == ',') parseStart = i;
					}
				}
			}
			else if (parseType == 3) {
				if (token[i] == ' ') {
					parseEnd = i - 1;
				}
			}
			else if (parseType == 5) {
				if (parsingBlock && !parsingString && token[i] == '/') {
					inCodeOfBlock++;
				}

				if (inCodeOfBlock && token[i - 1] == ';') {
					inCodeOfBlock--;
				}

				if (t == '#' && !inCodeOfBlock) {
					if (parsingBlock) {
						parsingBlock = 0;
					}
					else {
						parsingBlock = 1;

					}

					continue;
				}
				if (t == ';' && !parsingBlock) {
					parseType = -1;
					parseEnd = i - 1;

					char** args = (char**)malloc(sizeof(char*) * (parseEnd - parseStart));
					char** argOrigins = (char**)malloc(sizeof(char*) * (parseEnd - parseStart));
					int* argSizes = (int*)malloc(sizeof(int) * (parseEnd - parseStart));
					for (int i = 0; i < (parseEnd - parseStart); i++) {
						args[i] = (char*)malloc(sizeof(char) * (parseEnd - parseStart));
						argOrigins[i] = (char*)malloc(sizeof(char) * (parseEnd - parseStart));
					}

					int argStart = 0;
					int argEnd = -1;

					int argIndex = 0;
					char to;
					char to2;

					int commenting = 0;
					int inCode = 0;
					for (int j = 0; j <= parseEnd - parseStart + 1; j++) {
						to = token[parseStart + j + 1];
						to2 = token[parseStart + j + 2];

						if (commenting && to == '\n') commenting = 0;

						if (commenting) {
							continue;
						}

						if (parsingBlock && !parsingString && to == '/') {
							inCode++;
						}

						if (inCode && to == ';') {
							inCode--;
						}

						if (inCode && to == '#' && to2 != '#') {
							continue;
						}

						if (to == '"') {
							if (parsingString) {
								parsingString = 0;
							}
							else {
								parsingString = 1;

							}
						}

						if (to == '#' && to2 == '#') {
							commenting = 1;
							continue;
						}

						if (to == '#' && to2 != '#' && !inCode) {
							if (parsingBlock) {
								parsingBlock = 0;
							}
							else {
								parsingBlock = 1;

							}

							continue;
						}

						if ((to == ' ' || to == ';') && !parsingString && !parsingBlock) {
							argEnd = j - 1;

							struct ParseTextResult parsedStr = ParseString(token, parseStart + argStart + 1, parseStart + argEnd + 1);

							args[argIndex] = (char*)malloc(sizeof(char) * parsedStr.size);
							argOrigins[argIndex] = (char*)malloc(sizeof(char) * (argEnd - argStart + 1));

							for (int s = 0; s <= parsedStr.size; s++) {
								args[argIndex][s] = parsedStr.parsed[s];
							}
							for (int s = 0; s <= argEnd - argStart + 1; s++) {
								argOrigins[argIndex][s] = token[parseStart + s + argStart + 1];
							}
							argSizes[argIndex] = argEnd - argStart + 1;

							//printf("\n--%s", argOrigins[argIndex]);

							args[argIndex][parsedStr.size] = '\0';
							argOrigins[argIndex][argEnd - argStart + 1] = '\0';
							argIndex++;
							argStart = j + 1;
						}
					}

					//printf("\n%s - %s, %s", args[0], args[1], args[2]);
					//printf("\n%s - %s, %s", argOrigins[0], argOrigins[1], argOrigins[2]);

					if (strcmp(argOrigins[0], "say") == 0) {
						printf("\n%s", args[1]);
					} else if (strcmp(argOrigins[0], "replaceitem") == 0) {
						char* name = getVariableName(argOrigins[1]);
						struct Variable var = findVariableByName(name);
						if (var.type == -1) {
							printf("\nCan't replace at null");
						}
						else if (var.type == 0) {
							scanf("%d", &var.valueInt);
						}
						else if (var.type == 1) {
							scanf("%f", &var.valueFloat);
						}
						else if (var.type == 2) {
							scanf("%lf", &var.valueDouble);
						}
						else if (var.type == 3) {
							char type[5];
							scanf("%s", type);
							if (strcmp(type, "true")) {
								var.valueBool = 1;
							}
							else {
								var.valueBool = 0;
							}
						}
						else if (var.type == 4) {
							char* input;

							if (strlen(args[2]) > 1) input = (char*)malloc(sizeof(char) * atoi(args[2]));
							else input = (char*)malloc(sizeof(char) * 1000000);
							scanf("%s", input);

							strcpy(var.valueStr, input);
						}
					}
					else if (strcmp(argOrigins[0], "scoreboard") == 0) {
						char* name = getVariableName(argOrigins[1]);
						struct Variable var = findVariableByName(name);

						scoreMath(args, argOrigins, 0, var);
					}
					else if (strcmp(argOrigins[0], "execute") == 0) {
						struct Variable var;
						int once = 1;
						int repeat = 0;
						while (once || repeat) {
							for (int a = 1; a < argIndex; a++) {
								//printf("\n%s", argOrigins[a]);
								if (strcmp(argOrigins[a], "at") == 0) {
									char* name = getVariableName(argOrigins[a + 1]);
									var = findVariableByName(name);

									a++;
								}
								else if (strcmp(argOrigins[a], "score") == 0) {
									scoreMath(args, argOrigins, a - 1, var);

									if (strcmp(argOrigins[a + 1], "random")) a += 2;
									else a++;
								}
								else if (strcmp(argOrigins[a], "while") == 0) {
									if (strcmp(argOrigins[a + 1], "true") == 0) {
										repeat = 1;
										a++;
									} else if (strcmp(argOrigins[a + 1], "false") == 0) {
										repeat = 0;
										a++;
									}
									else {
										repeat = scoreTest(args, argOrigins, a - 1, var);
										a += 2;
									}
									
								}
								else if (strcmp(argOrigins[a], "if") == 0 && once) {
									if (strcmp(argOrigins[a + 1], "true") == 0) {
										once = 1;
										a++;
									}
									else if (strcmp(argOrigins[a + 1], "false") == 0) {
										once = 0;
										a++;
									}
									else {
										once = scoreTest(args, argOrigins, a - 1, var);
										a += 2;
									}

									if (!once) break;
								}
								else if (strcmp(argOrigins[a], "run") == 0) {
									argOrigins[a + 1][0] = ' ';
									argOrigins[a + 1][argSizes[a+1] - 1] = '\0';

									parseCode(argOrigins[a + 1], varSize);

									a++;
								}
							}

							if (once) once = 0;
						}
					}
					else {
						int isLib = 0;
						for (int i = 0; i < argSizes[0]; i++) {
							if (argOrigins[0][i] == '$') {
								isLib = 1;
								break;
							}
						}

						printf("\n== %d", isLib);
						if (isLib) {
							struct LibFunc func = FindLibFunction(argOrigins[0]);

							char** args_ = (char**)malloc(sizeof(char*) * func.argCount);
							char** argOrigins_ = (char**)malloc(sizeof(char*) * func.argCount);

							for (int i = 0; i < func.argCount; i++) {
								args_[i] = args[1 + i];
								argOrigins_[i] = argOrigins[1 + i];
							}

							func.func(&args_, &argOrigins_);

						}
					}

					free(args);
					free(argOrigins);
					free(argSizes);
				}
			}
		}
		//printf("\n%c - %d", t, parseType);
	}

	free(parsedToken);
}

struct ParseTextResult ParseString(char* token, int start, int end) {
	char to;
	int parsing = 0;
	int varParse = 0;
	int offset = 0;
	int varIndex = 0;
	int varEnd = -1;

	char* txt = (char*)malloc(sizeof(char) * varSize);
	int txtIndex = 0;
	for (int i = 0; i <= end - start + 1; i++) {
		to = token[start + i];

		if (to == '"') {
			if (parsing) {
				parsing = 0;
			}
			else {
				parsing = 1;
				offset = i;
			}
			continue;
		}
		
		if (to == '@' && !varParse) {
			varParse = 1;
			varIndex = i + 1;
		}
		else if ((to == '*' && varParse))
		{ 
			varEnd = i - 1;
			varParse = 0;

			char* varName = (char*)malloc(sizeof(char) * (varEnd - varIndex + 2));

			for (int k = 0; k <= varEnd - varIndex + 1; k++) {
				varName[k] = token[start + varIndex + k];
			}
			
			varName[varEnd - varIndex + 1] = '\0';

			char* toText;
			toText = (char*)malloc(sizeof(char) * 20);
			int textSize = 20;

			struct Variable var = findVariableByName(varName);
			if (var.name == NULL || var.type == -1) {
				textSize = 5;
				toText = "null\0";
			}
			else if (var.type == 0) {
				int length = getNumLength((float)var.valueInt) + 4;
				textSize = length;
				free(toText);
				toText = (char*)malloc(sizeof(char) * length);
				sprintf(toText, "%d", var.valueInt);
			}
			else if (var.type == 1) {
				int length = getNumLength(var.valueFloat);
				textSize = length;
				free(toText);
				toText = (char*)malloc(sizeof(char) * length);
				sprintf(toText, "%.3lf", var.valueFloat);
			}
			else if (var.type == 2) {
				int length = getNumLength((float)var.valueDouble);
				textSize = length;
				free(toText);
				toText = (char*)malloc(sizeof(char) * length);
				sprintf(toText, "%.3lf", var.valueDouble);
			}
			else if (var.type == 3) {
				textSize = 5;
				free(toText);
				toText = (char*)malloc(sizeof(char) * 5);
				
				if (var.valueBool == 1) strcpy(toText, "true");
				else strcpy(toText, "false");
			}
			else if (var.type == 4) {
				textSize = var.strSize + 1;
				free(toText);
				toText = (char*)malloc(sizeof(char) * (var.strSize + 1));
				strcpy(toText, var.valueStr);
			}

			for (int j = 0; j <= textSize; j++) {
				if (toText[j] == '\0') break;
				txt[txtIndex] = toText[j];
				txtIndex++;
			}

			continue;
		}

		if (to == ';') to = ' ';

		if (!varParse) {
			txt[txtIndex] = to;
			txtIndex++;
		}
	}
	txt[txtIndex + 1] = '\0';

	struct ParseTextResult result = { txt, txtIndex };

	return result;
}

char* getVariableName(char* name) {
	int size = strlen(name);

	char* txt = (char*)malloc(sizeof(char) * (size));
	int txtIndex = 0;
	for (int i = 0; i < size; i++) {
		if (name[i] == '@' || name[i] == '*' || name[i] == ' ') continue;
		txt[txtIndex] = name[i];
		txtIndex++;
	}

	txt[txtIndex] = '\0';

	return txt;
}

struct Variable findVariableByName(char* name) {
	for (int i = 0; i < varSize; i++) {
		struct Variable var = variables[i];

		if (!IsAccessableVariable(var)) continue;

		if (strcmp(var.name, name) == 0) {
			return var;
		}
	}

	struct Variable var;
	var.type = -1;
	var.name = NULL;
	
	return var;
}

int findVariableIndex(struct Variable var_) {
	for (int i = 0; i < varSize; i++) {
		struct Variable var = variables[i];

		if (!IsAccessableVariable(var)) continue;

		if (strcmp(var.name, var_.name) == 0) {
			return i;
		}
	}

	return -1;
}

void scoreMath(char** args, char** argOrigins, int argI, struct Variable var) {
	int index = findVariableIndex(var);

	if (index == -1) {
		printf("\nunknown index error");
	}
	else if (var.type == 0) {
		if (strcmp(argOrigins[2 + argI], "+=") == 0) {
			variables[index].valueInt += atoi(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "-=") == 0) {
			variables[index].valueInt = var.valueInt - atoi(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "=") == 0) {
			variables[index].valueInt = atoi(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "*=") == 0) {
			variables[index].valueInt *= atoi(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "/=") == 0) {
			variables[index].valueInt /= atoi(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "%=") == 0) {
			variables[index].valueInt %= atoi(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "random") == 0) {
			srand((unsigned int)time(NULL));
			int min = atoi(args[3 + argI]);
			int max = atoi(args[4 + argI]);
			variables[index].valueInt = (int)rand() % (max - min) + min;
		}
	}
	else if (var.type == 1) {
		if (strcmp(argOrigins[2 + argI], "+=") == 0) {
			variables[index].valueFloat += (float)atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "-=") == 0) {
			variables[index].valueFloat -= (float)atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "=") == 0) {
			variables[index].valueFloat = (float)atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "*=") == 0) {
			variables[index].valueFloat *= (float)atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "/=") == 0) {
			variables[index].valueFloat /= (float)atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "%=") == 0) {
			variables[index].valueFloat = (float)((int)(var.valueFloat) % (int)atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "random") == 0) {
			srand((unsigned int)time(NULL));
			float min = (float)atof(args[3 + argI]);
			float max = (float)atof(args[4 + argI]);
			variables[index].valueFloat = (rand() % (int)((max - min) + min) * 1000) / 1000.0f;
		}
	}
	else if (var.type == 2) {
		if (strcmp(argOrigins[2 + argI], "+=") == 0) {
			var.valueDouble += atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "-=") == 0) {
			var.valueDouble -= atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "=") == 0) {
			var.valueDouble = atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "*=") == 0) {
			var.valueDouble *= atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "/=") == 0) {
			var.valueDouble /= atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "%=") == 0) {
			var.valueDouble = (int)(var.valueFloat) % (int)atof(args[3 + argI]);
		}
		else if (strcmp(argOrigins[2 + argI], "random") == 0) {
			srand((unsigned int)time(NULL));
			double min = atof(args[3 + argI]);
			double max = atof(args[4 + argI]);
			variables[index].valueDouble = (rand() % (int)((max - min) + min) * 1000) / 1000.0;
		}
	}
	else {
		printf("\nscoreboard can only used by number type variable");
	}
}

int scoreTest(char** args, char** argOrigins, int argI, struct Variable var) {
	int index = findVariableIndex(var);

	//printf("\n%s", argOrigins[2 + argI]);
	//printf(" %s", argOrigins[3 + argI]);

	if (index == -1) {
		printf("\nunknown index error");
	}
	else if (var.type == 0) {
		if (strcmp(argOrigins[2 + argI], ">") == 0) {
			return (variables[index].valueInt > atoi(args[3 + argI]));
		} else if (strcmp(argOrigins[2 + argI], "<") == 0) {
			return (variables[index].valueInt < atoi(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "<=") == 0) {
			return (variables[index].valueInt <= atoi(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "==") == 0) {
			return (variables[index].valueInt == atoi(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "!=") == 0) {
			return (variables[index].valueInt != atoi(args[3 + argI]));
		}
	}
	else if (var.type == 1) {
		if (strcmp(argOrigins[2 + argI], ">") == 0) {
			return (variables[index].valueFloat > (float)atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "<") == 0) {
			return (variables[index].valueFloat < (float)atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "<=") == 0) {
			return (variables[index].valueFloat <= (float)atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "==") == 0) {
			return (variables[index].valueFloat == (float)atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "!=") == 0) {
			return (variables[index].valueFloat != (float)atof(args[3 + argI]));
		}
	}
	else if (var.type == 2) {
		if (strcmp(argOrigins[2 + argI], ">") == 0) {
			return (variables[index].valueDouble > atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "<") == 0) {
			return (variables[index].valueDouble < atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "<=") == 0) {
			return (variables[index].valueDouble <= atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "==") == 0) {
			return (variables[index].valueDouble == atof(args[3 + argI]));
		}
		else if (strcmp(argOrigins[2 + argI], "!=") == 0) {
			return (variables[index].valueDouble != atof(args[3 + argI]));
		}
	}
	else {
		printf("\nscoreboard can only used by number type variable");
	}

	return 0;
}

void addLib(char* id) {
	if (lib == NULL) return;
	char* libCopy = lib;

	lib = (char**)malloc(((sizeof(char*)) * libCount) + sizeof(id));

	for (int i = 0; i < libCount; i++) {
		lib[i] = libCopy[i];
	}

	lib[libCount] = id;
	libCount++;
}

void addLibFunction(char* lib, char* name, int argCount, void (*action)(struct Variable* variables, struct Variable* varOrigins)) {
	struct Variable* variables;
	variables = (struct Variable*)malloc(sizeof(struct Variable) * argCount);

	int libLen = 0;
	int nameLen = 0;

	while (*lib) {
		lib++;
		libLen++;
	}

	lib -= libLen;

	while (*name) {
		name++;
		nameLen++;
	}

	name -= nameLen;

	char* libWithName = (char*)malloc(sizeof(char*) * (libLen + nameLen + 1));

	for (int i = 0; i < libLen; i++) {
		libWithName[i] = lib[i];
	}
	libWithName[libLen] = '$';

	for (int i = 0; i < nameLen; i++) {
		libWithName[libLen + 1 + i] = name[i];
	}

	libWithName[libLen + 1 + nameLen] = '\0';

	struct LibFunc libFunc;
	libFunc.lib = lib;
	libFunc.name = name;
	libFunc.libWithName = libWithName;
	libFunc.LNlen = libLen + nameLen + 1;
	libFunc.argCount = argCount;
	libFunc.func = action;

	struct LibFunc* libCopy = libFuncs;
	libFuncs = (char**)malloc((sizeof(char*)) * (libFuncCount + 1));

	for (int i = 0; i < libFuncCount; i++) {
		libFuncs[i] = libCopy[i];
	}

	libFuncs[libFuncCount] = libFunc;
	libFuncCount++;
}

struct LibFunc FindLibFunction(char* libWIthName) {
	for (int i = 0; i < libFuncCount; i++) {
		struct LibFunc func = libFuncs[i];

		if (strcmp(func.name, libWIthName) == 0) {
			return func;
		}
	}

	struct LibFunc func;
	func.argCount = -1;

	return func;
}

int IsLibConnected(char* lib) {
	int connected = 0;
	for (int i = 0; i < libCount; i++) {
		if (strcmp(libConnected[i], lib) == 0) {
			connected = 1;
			break;
		}
	}

	return connected;
}
