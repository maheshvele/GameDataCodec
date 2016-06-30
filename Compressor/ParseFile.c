#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "Common.h"

/***********************************************
* This function frees the vertex data structure
***********************************************/
void FreeVertData(CommonDataStr *commonDataPtr)
{
	VertexDataStr  *vertDataPtr;

	while (ListFirst(&commonDataPtr->vertDataHead))
	{
		vertDataPtr = (VertexDataStr *)ListRemoveHead(&commonDataPtr->vertDataHead);
		if (vertDataPtr)
		{
			free(vertDataPtr);
			vertDataPtr = NULL;
		}
	}
}

/******************************************
* Inserts vertex data information into a
* data structure
*******************************************/
void InsertVertData(CommonDataStr *commonDataPtr, double vertX, double vertY, double vertZ)
{
	VertexDataStr *vertDataPtr;

	if (vertX > commonDataPtr->maxX)
		commonDataPtr->maxX = vertX;

	if (vertX < commonDataPtr->minX)
		commonDataPtr->minX = vertX;

	if (vertY > commonDataPtr->maxY)
		commonDataPtr->maxY = vertY;

	if (vertY < commonDataPtr->minY)
		commonDataPtr->minY = vertY;

	if (vertZ > commonDataPtr->maxZ)
		commonDataPtr->maxZ = vertZ;

	if (vertZ < commonDataPtr->minZ)
		commonDataPtr->minZ = vertZ;

	vertDataPtr = (VertexDataStr *) malloc(sizeof(VertexDataStr));

	if (!vertDataPtr)
	{
		printf("Unablwae to allocate memory for the vertex data \n");
		return;
	}

	memset(vertDataPtr, 0, sizeof(VertexDataStr));

	vertDataPtr->vertX = vertX;
	vertDataPtr->vertY = vertY;
	vertDataPtr->vertZ = vertZ;

	ListInsertTail(&commonDataPtr->vertDataHead, &vertDataPtr->listHandle);

	//printf("InsetVertData : vertX = %f, vertY = %f, vertZ = %f\n", vertX, vertY, vertZ);

	return;
}

/****************************************
This function parses the vertex data line 
and stores the individual vertices
*****************************************/
void ParseVertLine(char *line, CommonDataStr *commonDataPtr)
{
	double id, vertX, vertY, vertZ;

	char *strIdEnd;
	char *strVertXEnd;
	char *strVertYEnd;

	id = strtod(line, &strIdEnd);
	strIdEnd++;

	vertX = strtod(strIdEnd, &strVertXEnd);
	strVertXEnd++;

	vertY = strtod(strVertXEnd, &strVertYEnd);
	strVertYEnd++;

	vertZ = strtod(strVertYEnd, NULL);

	InsertVertData(commonDataPtr, vertX, vertY, vertZ);

	return;
}

void ParseVertFile(FILE *vertFilePtr, CommonDataStr *commonDataPtr)
{
	char line[MAX_CHARS_IN_LINE];
	int numLines = 0;

	while (!feof(vertFilePtr))
	{
		numLines++;
		fgets(line, MAX_CHARS_IN_LINE, vertFilePtr);
		ParseVertLine(line, commonDataPtr);
	}

	commonDataPtr->numVerts = 3 * numLines;
	return;
}