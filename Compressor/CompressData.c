#include <stdio.h>
#include <stdlib.h>
#include "Common.h"

/*************************************************
* This function writes the header data required to
* retrieve the information from the compressed file
**************************************************/
void WriteBitStreamHeader(FILE *bitStreamPtr, CommonDataStr *commonDataPtr)
{
	fwrite((unsigned char *)&commonDataPtr->numBits, sizeof(unsigned char), 1, bitStreamPtr);
	fwrite(&commonDataPtr->numVerts, sizeof(int), 1, bitStreamPtr);

	fwrite(&commonDataPtr->minX, sizeof(double), 1, bitStreamPtr);
	fwrite(&commonDataPtr->minY, sizeof(double), 1, bitStreamPtr);
	fwrite(&commonDataPtr->minZ, sizeof(double), 1, bitStreamPtr);
	
	fwrite(&commonDataPtr->maxX, sizeof(double), 1, bitStreamPtr);
	fwrite(&commonDataPtr->maxY, sizeof(double), 1, bitStreamPtr);
	fwrite(&commonDataPtr->maxZ, sizeof(double), 1, bitStreamPtr);

	return;
}

/**************************************************
* Calculate the bucket size for individual vertices,
* Bucket Size = (maxValue - minValue)/ (2**numBits).
**************************************************/
void CalculateBucketSizes(CommonDataStr *commonDataPtr)
{
	commonDataPtr->bucketSizeX = (commonDataPtr->maxX - commonDataPtr->minX) / (1 << (commonDataPtr->numBits));
	
	commonDataPtr->bucketSizeY = (commonDataPtr->maxY - commonDataPtr->minY) / (1 << (commonDataPtr->numBits));
	
	commonDataPtr->bucketSizeZ = (commonDataPtr->maxZ - commonDataPtr->minZ) / (1 << (commonDataPtr->numBits));
}

/******************************************************
* Get the bucket number where the vertex falls in. This
* bucket number is used to represent the vertex in the 
* compressed file.
******************************************************/
int  GetCompressedNumber(double vertVal, CommonDataStr *commonDataPtr, const char vert)
{
	int retVal;
	switch (vert)
	{

	case 'X':
		if (vertVal == commonDataPtr->maxX)
			retVal = ((1 << (commonDataPtr->numBits)) - 1);
		else
			retVal = (int)((vertVal - commonDataPtr->minX) / commonDataPtr->bucketSizeX);
		break;

	case 'Y':
		if (vertVal == commonDataPtr->maxY)
			retVal = ((1 << (commonDataPtr->numBits)) - 1);
		else
			retVal = (int)((vertVal - commonDataPtr->minY) / commonDataPtr->bucketSizeY);
		break;

	case 'Z':
		if (vertVal == commonDataPtr->maxZ)
			retVal = ((1 << (commonDataPtr->numBits)) - 1);
		else
			retVal = (int)((vertVal - commonDataPtr->minZ) / commonDataPtr->bucketSizeZ);
		break;

	default:
		break;
	}

	return retVal;
}


/**************************************************************
* This function fills the the individual vertex data in a
* running 32 bit buffer. The contents of the buffer are written
* to a file whenever the buffer is full. It also takes care of 
* data that is split between two buffers.
* 
* Note: This function uses two gloabal variables
* 1) buffer32
* 2) remainingBufferBits
****************************************************************/
void FillAndWrite32BitBuffer(int data, int numBits, FILE * bitStreamPtr, FILE *debugFilePtr)
{
	int bitMask = (1 << numBits ) - 1;
	int dataBitsRemaining; 

	remainingBufferBits -= numBits;

	if (remainingBufferBits > 0)
	{
		buffer32 |= (data << remainingBufferBits);
	}
	else
	{   
		//split the data between two buffers.

		//Write the available data into the first buffer
		buffer32 |= (data >> (-remainingBufferBits));

		fwrite(&buffer32, sizeof(int), 1, bitStreamPtr);
		
		//calculate the databits remaining
		dataBitsRemaining = -remainingBufferBits;

		//initialize the buffer and remaining bits
		buffer32 = 0;
		remainingBufferBits = MAX_BUFFER_BITS - dataBitsRemaining;

		//write the remaining bits in the new buffer at the appropriate place
		if (remainingBufferBits != MAX_BUFFER_BITS)	
			buffer32 |= (data << remainingBufferBits);
	}
}

/******************************************************
* This function compresses the individual vert data
* and then writes the compressed data to the bit stream 
********************************************************/
void CompressVertData(CommonDataStr *commonDataPtr, VertexDataStr *vertDataPtr, FILE *bitStreamPtr, FILE *debugFilePtr)
{
	int compX , compY, compZ;

	compX = GetCompressedNumber(vertDataPtr->vertX, commonDataPtr, 'X');
	fprintf(debugFilePtr, "CompX = %d, ", compX);

	FillAndWrite32BitBuffer(compX, commonDataPtr->numBits, bitStreamPtr, debugFilePtr);
	
	compY = GetCompressedNumber(vertDataPtr->vertY, commonDataPtr, 'Y');
	fprintf(debugFilePtr, "CompY = %d, ", compY);
	
	FillAndWrite32BitBuffer(compY, commonDataPtr->numBits, bitStreamPtr, debugFilePtr);

	compZ = GetCompressedNumber(vertDataPtr->vertZ, commonDataPtr, 'Z');
	fprintf(debugFilePtr, "CompZ = %d\n", compZ);
	
	FillAndWrite32BitBuffer(compZ, commonDataPtr->numBits, bitStreamPtr, debugFilePtr);
}

/******************************************
* Starter function to compress the data and
* create bitstream.
*****************************************/
void CompressToBitStream(CommonDataStr *commonDataPtr)
{
	LinkedList *entry;
	VertexDataStr *vertDataPtr;
	FILE *bitStreamPtr;
	FILE *debugFilePtr;

	fopen_s(&bitStreamPtr, BITSTREAM_FILE, "wb");

	if (bitStreamPtr == NULL)
	{
		printf("Fatal Error: Unable to open %s file\n", BITSTREAM_FILE);
		return;
	}

	fopen_s(&debugFilePtr, DEBUG_FILE, "w");

	if (debugFilePtr == NULL)
	{
		printf("Fatal Error: Unable to open comp debug file\n");
		return;
	}

	WriteBitStreamHeader(bitStreamPtr, commonDataPtr);

	CalculateBucketSizes(commonDataPtr);

	entry = ListFirst(&commonDataPtr->vertDataHead);

	/* Initialize the global variables */
	buffer32 = 0;
	remainingBufferBits = 32;

	while (entry)
	{
		vertDataPtr = (VertexDataStr *)entry;
		
		fprintf(debugFilePtr, "vertX = %f, vertY = %f, vertZ = %f\n", vertDataPtr->vertX, vertDataPtr->vertY, vertDataPtr->vertZ);

		CompressVertData(commonDataPtr, vertDataPtr, bitStreamPtr, debugFilePtr);
		entry = ListNext(&commonDataPtr->vertDataHead, entry);

		fprintf(debugFilePtr, "************************************\n");
	}

	//fprintf(debugFilePtr, "Writing Buffer : %d\n", buffer32);
	fwrite(&buffer32, sizeof(int), 1, bitStreamPtr);

	fclose(bitStreamPtr);
	fclose(debugFilePtr);

	return;
}