#include <stdio.h>
#include <stdlib.h>
#include "Common.h"

void DebugPrintDouble(const char *debugStr, double value)
{
	printf("%s%f\n", debugStr, value);
}

void DebugPrintInt(const char *debugStr, int value)
{
	printf("%s%d\n", debugStr, value);
}