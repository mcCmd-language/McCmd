#include "parseCode.h";

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