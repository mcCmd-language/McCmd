#include "define.h";

long fsize(FILE*);
int parseMain(int argc, char* argv[]);
void parseCode(char*, int size);
struct ParseTextResult ParseString(char*, int, int);
struct Variable findVariableByName(char*);
int findVariableIndex(struct Variable);
char* getVariableName(char*);
void scoreMath(char**, char**, int, struct Variable);
int scoreTest(char**, char**, int, struct Variable);
void addLib(char* lib);
void addLibFunction(char* lib, char* name, int argCount, void (*action)(struct Variable* variables, struct Variable* varOrigins));
struct LibFunc FindLibFunction(char* libWIthName);
int IsLibConnected(char* lib);
void Print();
void Mall();

struct Variable {
	char* name;
	int type;
	int valueBool;
	int valueInt;
	char* valueStr;
	int strSize;
	float valueFloat;
	double valueDouble;
	int declared;
};

struct ParseTextResult {
	char* parsed;
	int size;
};

struct LibFunc {
	char* lib;
	char* name;
	char* libWithName;
	int LNlen;
	int argCount;
	void (*func)(struct Variable* variables, struct Variable* varOrigins);
};

struct DefinedFunc {
	char* name;
	int argCount;
	char* actionToken;
};