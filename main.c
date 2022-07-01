#include <stdio.h>
#include <string.h>
#include "cf_interface.h"
int main()
{
	double testd = 1.1;
	int testi = 1;
	char *tests = StrMalloc("hello",strlen("hello"));
	AddVar("TEST_DOUBLE",&testd,VAR_DOUBLE);
	AddVar("TEST_INT",&testi,VAR_INT);
	AddVar("TEST_STR",&tests,VAR_STR);

	printf("testd=%f\r\n",testd);
	printf("testi=%d\r\n",testi);
	printf("tests=%s\r\n",tests);

	RunInterpreter("./cf.txt");

	printf("testd=%f\r\n", testd);
	printf("testi=%d\r\n", testi);
	printf("tests=%s\r\n", tests);
}
