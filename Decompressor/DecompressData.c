#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "Common.h"

/******************************************************
* Parse the bit stream header and store the values in the
* common data str
********************************************************/
void ParseBitStreamHeader(CommonDataStr *commonDataPtr, FILE *bitStreamFilePtr)
{
	fread(&commonDataPtr->numBits, sizeof(unsigned char), 1, bitStreamFilePtr);
	fread(&commonDataPtr->numVerts, sizeof(int), 1, bitStreamFilePtr);

	fread(&commonDataPtr->minX, sizeof(double), 1, bitStreamFilePtr);
	fread(&commonDataPtr->minY, sizeof(double), 1, bitStreamFilePtr);
	fread(&commonDataPtr->minZ, sizeof(double), 1, bitStreamFilePtr);

	fread(&commonDataPtr->maxX, sizeof(double), 1, bitStreamFilePtr);
	fread(&commonDataPtr->maxY, sizeof(double), 1, bitStreamFilePtr);
	fread(&commonDataPtr->maxZ, sizeof(double), 1, bitStreamFilePtr);
	
	return;
}

/****************************************************
* Calculate the bucket size from the retrieved header 
****************************************************/
void CalculateBucketSizes(CommonDataStr *commonDataPtr)
{
	commonDataPtr->bucketSizeX = (commonDataPtr->maxX - commonDataPtr->minX) / (1 << (commonDataPtr->numBits));

	commonDataPtr->bucketSizeY = (commonDataPtr->maxY - commonDataPtr->minY) / (1 << (commonDataPtr->numBits));

	commonDataPtr->bucketSizeZ = (commonDataPtr->maxZ - commonDataPtr->minZ) / (1 << (commonDataPtr->numBits));
}

/********************************************************
* This function reads the bitstream 32 bits at a time and
* converts the buffer read into individual data units of 
* length num bits. It also takes care of data split across
* two buffers.

Note: It uses two global variables 
- buffer32
- remainingBufferBits
********************************************************/
int GetCompressedData(CommonDataStr *commonDataPtr, FILE *bitStreamFilePtr, FILE *debugFilePtr)
{
	int compData = 0;
	int numBits = commonDataPtr->numBits;
	int bitMask = (1 << (commonDataPtr->numBits))-1;
	int compBitMask1 = 0, compBitMask2 = 0;

	remainingBufferBits -= numBits;

	if (remainingBufferBits > 0)
	{
		compData = (buffer32 >> remainingBufferBits) & bitMask ;
	}
	else
	{
		//Data is split accross two buffers

		//get the data from first buffer
		compBitMask1 = (1 << (numBits + remainingBufferBits)) - 1;
		compData = buffer32 & compBitMask1;
		compData = compData << (-remainingBufferBits);

		fread(&buffer32, sizeof(int), 1, bitStreamFilePtr);

		//get the remaining data from the next buffer and append it to 
		// available data
		compBitMask2 = (1 << (-remainingBufferBits)) - 1;
		remainingBufferBits = MAX_BUFFER_BITS + remainingBufferBits;
		compData |= (buffer32 >> remainingBufferBits) & compBitMask2;
	}

	return compData;
}

/*************************************************
* Given a bucket number this function returns the
* data represented by the bucket. 

* Note: Data represented by the bucket is the average
* of the bucket max and bucket min.
*************************************************/
double DecompressData(int compData, double min, double bucketSize)
{
	return (min + compData * bucketSize + bucketSize/2);
}

double GetErrorSquared(double estimatedVal, double actualVal, FILE *debugFilePtr)
{
	return  (estimatedVal - actualVal) * (estimatedVal - actualVal);
}

/**********************************************
* Starter function to decompress the data from 
* bitstream.
***********************************************/
void DecompressFromBitStream(CommonDataStr *commonDataPtr)
{
	FILE *bitStreamFilePtr;
	FILE *debugFilePtr;
	int numVerts = 0;
	int compX, compY, compZ;
	double estimatedX, estimatedY, estimatedZ;
	VertexDataStr *vertData = (VertexDataStr *)ListFirst(&commonDataPtr->vertDataHead);
	double sumErrorSquared = 0;
	
	struct stat fileStats;
	stat(BITSTREAM_FILE, &fileStats);

	fopen_s(&bitStreamFilePtr, BITSTREAM_FILE, "rb");

	if (bitStreamFilePtr == NULL)
	{
		printf("Fatal Error: Unable to open %s file\n", BITSTREAM_FILE);
		return;
	}

	fopen_s(&debugFilePtr, DEBUG_FILE, "w");

	if (debugFilePtr == NULL)
	{
		printf("Unable to open decomp debug file\n");
		return;
	}

	ParseBitStreamHeader(commonDataPtr, bitStreamFilePtr);

	CalculateBucketSizes(commonDataPtr);

	buffer32 = 0;
	remainingBufferBits = MAX_BUFFER_BITS;

	fread(&buffer32, sizeof(int), 1, bitStreamFilePtr);
	//fprintf(debugFilePtr, "Buffer : %d\n", buffer32);

	if (commonDataPtr->numVerts != commonDataPtr->numActualVerts)
	{
		printf("Fatal Error: Number of Verts in bitStream does not equal actual number of verts\n");
		return;
	}

	while (numVerts != commonDataPtr->numVerts)
	{
		compX = GetCompressedData(commonDataPtr, bitStreamFilePtr, debugFilePtr);

		compY = GetCompressedData(commonDataPtr, bitStreamFilePtr, debugFilePtr);

		compZ = GetCompressedData(commonDataPtr, bitStreamFilePtr, debugFilePtr);

		if (vertData)
		{
			estimatedX = DecompressData(compX, commonDataPtr->minX, commonDataPtr->bucketSizeX);

			estimatedY = DecompressData(compY, commonDataPtr->minY, commonDataPtr->bucketSizeY);
			
			estimatedZ = DecompressData(compZ, commonDataPtr->minZ, commonDataPtr->bucketSizeZ);

			fprintf(debugFilePtr, "compX = %d, compY = %d, compZ = %d\n", compX, compY, compZ);
			fprintf(debugFilePtr, "actualX = %f, actualY = =%f, actualZ = %f\n", vertData->vertX, vertData->vertY, vertData->vertZ);
			fprintf(debugFilePtr, "estimatedX = %f, estimatedY = %f, estimatedZ = %f\n", estimatedX, estimatedY, estimatedZ);
			fprintf(debugFilePtr, "\n********************************\n");

			sumErrorSquared += GetErrorSquared(estimatedX, vertData->vertX, debugFilePtr);
			sumErrorSquared += GetErrorSquared(estimatedY, vertData->vertY, debugFilePtr);
			sumErrorSquared += GetErrorSquared(estimatedZ, vertData->vertZ, debugFilePtr);

			vertData = (VertexDataStr *)ListNext(&commonDataPtr->vertDataHead, 
											     &vertData->listHandle);
		}

		numVerts += 3;
	}

	double rootMeanSquaredError = sqrt(sumErrorSquared/numVerts);
	printf("%d, %0.8f, %d\n", commonDataPtr->numBits, rootMeanSquaredError, fileStats.st_size);

	fclose(bitStreamFilePtr);
	fclose(debugFilePtr);

	return;
}
