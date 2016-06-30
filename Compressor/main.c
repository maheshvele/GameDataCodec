#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <float.h>
#include "Common.h"

/******************************************************
* This function Initialises the common data structure 
* with default values.
********************************************************/
void InitCommonData(CommonDataStr *commonDataPtr)
{
	commonDataPtr->minX = DBL_MAX;
	commonDataPtr->minY = DBL_MAX;
	commonDataPtr->minZ = DBL_MAX;

	commonDataPtr->maxX = DBL_MIN;
	commonDataPtr->maxY = DBL_MIN;
	commonDataPtr->maxZ = DBL_MIN;

	ListInit(&commonDataPtr->vertDataHead);
}

int main(int argc, char *argv[])
{
	FILE *vertFilePtr;
	CommonDataStr commonData;
	CommonDataStr *commonDataPtr = &commonData;
	int numBits;

	sscanf_s(argv[1], "%d", &numBits);

	printf("numBits = %d\n", numBits);

	fopen_s(&vertFilePtr, VERTEX_FILE, "r");

	if (!vertFilePtr)
	{
		printf("Fatal Error : Unable to open %s file!\n", VERTEX_FILE); 
		return 1;
	}

	/*Initialize the common data structure*/
	InitCommonData(commonDataPtr);
	
	/*Store numBits*/
	commonDataPtr->numBits = numBits;

	/*Parse the verts data file and create the vert data list*/
	ParseVertFile(vertFilePtr, commonDataPtr);

#if DEBUG 
	printf("minX = %f, minY = %f, minZ = %f\n", commonDataPtr->minX, commonDataPtr->minY, commonDataPtr->minZ);
	printf("maxX = %f, maxY = %f, maxZ = %f\n", commonDataPtr->maxX, commonDataPtr->maxY, commonDataPtr->maxZ);
	printf("********************************\n");
#endif

	/*Read the vert data list and write the bitstream*/
	CompressToBitStream(commonDataPtr);
	
	/*Free up the vert data linked list*/
	FreeVertData(commonDataPtr);
	
	fclose(vertFilePtr);
	return 0;
}