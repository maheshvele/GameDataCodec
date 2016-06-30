#include <stdio.h>
#include <stdlib.h>
#include "Common.h"

int main()
{
	CommonDataStr commonDataStr;
	CommonDataStr *commonDataPtr = &commonDataStr;
	FILE *vertFilePtr;
	fopen_s(&vertFilePtr, VERTEX_FILE, "r");

	if (vertFilePtr == NULL)
	{
		printf("Fatal Error: Unable to open %s File\n", VERTEX_FILE);
		return 1;
	}
	
	//Initialise the common data structure
	ListInit(&commonDataPtr->vertDataHead);
	commonDataPtr->numBits = 0;
	commonDataPtr->numVerts = 0;

	//Parse the vert file and create the vert data list
	ParseVertFile(vertFilePtr, commonDataPtr);

	//Decompress the bitstream file and find RMS error
	DecompressFromBitStream(commonDataPtr);

	//Free the vert data
	FreeVertData(commonDataPtr);

	fclose(vertFilePtr);
	return 0;
}