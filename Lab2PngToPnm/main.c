#include "return_codes.h"
#include <malloc.h>
#include <stdio.h>

#if defined ZLIB
	#include <zlib.h>
#elif defined LIBDEFLATE
	#include "libdeflate/libdeflate.h"
#elif defined SPECIAL_OPERATION
	#error "Unsupported"
#else
	 #error "An unsupported library was trying to include. I work with zlib only! Sorry, but I couldn't work with libdeflate or isa-l. Please, use zlib instead!"
#endif

#define BIT_DEPTH			  8
#define BLACK_AND_WHITE_IMAGE 0
#define COLORED_IMAGE		  2
#define MY_DEBUG_OUTPUT		  0

#define IHDR_CHUNK_TYPE		1
#define PLTE_CHUNK_TYPE		2
#define IDAT_CHUNK_TYPE		3
#define IEND_CHUNK_TYPE		4
#define HELP_CHUNK_TYPE		0
#define INVALID_CHUNK_TYPE	(-1)
#define FILTER_TYPE_NONE	0
#define FILTER_TYPE_SUB		1
#define FILTER_TYPE_UP		2
#define FILTER_TYPE_AVERAGE 3
#define FILTER_TYPE_PAETH	4

typedef unsigned int uint;
typedef unsigned long long ull;

// Res: 1 - error. Non correct data
//      0 - success
uint checkSignature(const unsigned char *signature)
{
	if (signature[0] < 128)	   // nonASCII code
	{
		return 1;
	}
	if (!(signature[1] == 80 && signature[2] == 78 && signature[3] == 71))	  // PNG
	{
		return 1;
	}
	if (signature[4] != 13 || signature[5] != 10 || signature[6] != 26 || signature[7] != 10)	 // CRLF...
	{
		return 1;
	}
	return 0;
}

// get Number in 10 radix from 4 bytes
uint getNumber4Bytes(const unsigned char *bytes)
{
	uint res = bytes[3];
	uint tmp = 256;
	for (int k = 2; k >= 0; k--)
	{
		res += (tmp * bytes[k]);
		tmp *= 256;
	}
	return res;
}

// -1 == error, >=0 == iHDRLength
uint checkImageHeader(const unsigned char *iHDRType, const unsigned char *iHDRLength)
{
	if (iHDRType[0] != 73 || iHDRType[1] != 72 || iHDRType[2] != 68 || iHDRType[3] != 82)	 // IHDR
	{
		return -1;
	}
	return getNumber4Bytes(iHDRLength);
}

// If input data is not correct: send a message, close the file and return ERROR_CODE
int InvalidDataError(FILE *file)
{
	fprintf(stderr,
			"Error. Invalid input data.\n"
			"Looks like you tried to convert non-PNG file. Please, make sure that input file is correct!\n");
	fclose(file);
	return ERROR_INVALID_DATA;
}

struct Chunk
{
	unsigned char len[4];
	unsigned char type[4];
	unsigned char crc[4];
};

// obviously returns chunkType(integer values that was defined before)
int getChunkType(const unsigned char *chunkType)
{
	if (chunkType[0] >= 65 && chunkType[0] <= 90)
	{
		// critical chunk
		if (chunkType[0] == 73 && chunkType[1] == 72 && chunkType[2] == 68 && chunkType[3] == 82)
		{
			// IHDR
			return IHDR_CHUNK_TYPE;
		}
		else if (chunkType[0] == 80 && chunkType[1] == 76 && chunkType[2] == 84 && chunkType[3] == 69)
		{
			// PLTE
			return PLTE_CHUNK_TYPE;
		}
		else if (chunkType[0] == 73 && chunkType[1] == 68 && chunkType[2] == 65 && chunkType[3] == 84)
		{
			// IDAT
			return IDAT_CHUNK_TYPE;
		}
		else if (chunkType[0] == 73 && chunkType[1] == 69 && chunkType[2] == 78 && chunkType[3] == 68)
		{
			// IEND
			return IEND_CHUNK_TYPE;
		}
		else
		{
			// invalid chunk Type
			return INVALID_CHUNK_TYPE;
		}
	}
	else if (chunkType[0] >= 97 && chunkType[0] <= 122)
	{
		// non-critical chunk
		return HELP_CHUNK_TYPE;
	}
	else
	{
		// invalid chunk Type
		return INVALID_CHUNK_TYPE;
	}
}

// Function for FilterType Paeth
unsigned char paethPredictor(unsigned char left, unsigned char above, unsigned char leftAbove)
{
	int p = left + above - leftAbove;
	uint pa = abs(p - left);
	uint pb = abs(p - above);
	uint pc = abs(p - leftAbove);
	if (pa <= pb && pa <= pc)
	{
		return left;
	}
	if (pb <= pc)
	{
		return above;
	}
	return leftAbove;
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr,
				"Wrong count of arguments.\n"
				"You should tell me the name of input data file and output data file only!\n");
		return ERROR_INVALID_PARAMETER;
	}
	FILE *in = fopen(argv[1], "rb");
	if (!in)
	{
		fprintf(stderr,
				"Error. Couldn't open input data file.\n"
				"Please, make sure that name of input file is correct!\n");
		return ERROR_FILE_NOT_FOUND;
	}
	//###################################################################
	// parsing input data
	//###################################################################
	//---------------------------------------------------------
	// check signature
	//---------------------------------------------------------
	unsigned char signature[8] = { 0 };
	fread(&signature, sizeof(unsigned char), 8, in);
	uint checkSign = checkSignature(signature);
	if (checkSign)
	{
		return InvalidDataError(in);
	}
	// Post: signature is correct
	//---------------------------------------------------------
	// parse Image Header
	//---------------------------------------------------------
	unsigned char iHDRLength[4];
	unsigned char iHDRType[4];
	ull c = fread(&iHDRLength, sizeof(unsigned char), 4, in);
	if (c != 4)
	{
		return InvalidDataError(in);
	}
	c = fread(&iHDRType, sizeof(unsigned char), 4, in);
	if (c != 4)
	{
		return InvalidDataError(in);
	}
	uint iHDRLen = checkImageHeader(iHDRType, iHDRLength);
	if (iHDRLen != 13)
	{
		return InvalidDataError(in);
	}
	unsigned char *iHDRData = malloc(sizeof(unsigned char) * iHDRLen);
	if (iHDRData == NULL)
	{
		fprintf(stderr, "Error. Not enough memory to read ImageHeader!\n");
		fclose(in);
		return ERROR_MEMORY;
	}
	c = fread(iHDRData, sizeof(unsigned char), iHDRLen, in);
	if (c != iHDRLen)
	{
		free(iHDRData);
		return InvalidDataError(in);
	}
	unsigned char iHDRDataWidth[4] = { iHDRData[0], iHDRData[1], iHDRData[2], iHDRData[3] };
	unsigned char iHDRDataHeight[4] = { iHDRData[4], iHDRData[5], iHDRData[6], iHDRData[7] };
	uint width = getNumber4Bytes(iHDRDataWidth);
	uint height = getNumber4Bytes(iHDRDataHeight);
	uint bitDepth = iHDRData[8];
	if (bitDepth != BIT_DEPTH)
	{
		free(iHDRData);
		return InvalidDataError(in);
	}
	unsigned char colorType = iHDRData[9];
#if MY_DEBUG_OUTPUT
	if (colorType == BLACK_AND_WHITE_IMAGE)
	{
		// black and white image
		printf("This is black and white image\n");
	}
	else if (colorType == COLORED_IMAGE)
	{
		// colored image
		printf("This is colored image\n");
	}
	else
#else
	if (colorType != COLORED_IMAGE && colorType != BLACK_AND_WHITE_IMAGE)
#endif
	{
		free(iHDRData);
		return InvalidDataError(in);
	}
	free(iHDRData);
	unsigned char iHDRCrc[4];
	c = fread(&iHDRCrc, sizeof(unsigned char), 4, in);
	if (c != 4)
	{
		return InvalidDataError(in);
	}
	// Post: successfully parsed ImageHeader chunk. No errors occurred
	//       remembered all useful information: width, height, colorType
	//---------------------------------------------------------
	// parsing chunks
	//---------------------------------------------------------
	struct Chunk chunk;
	c = fread(&chunk.len, sizeof(unsigned char), 4, in);
	if (c != 4)
	{
		// chunk.length is not 4 bytes;
		return InvalidDataError(in);
	}
	c = fread(&chunk.type, sizeof(unsigned char), 4, in);
	if (c != 4)
	{
		// chunk.type is not 4 bytes
		return InvalidDataError(in);
	}
	int chunkType = getChunkType(chunk.type);
	if (chunkType == INVALID_CHUNK_TYPE)
	{
		// invalid chunk type error
		return InvalidDataError(in);
	}
	unsigned char *allData = malloc(sizeof(unsigned char) * 8);
	uint allDataLength = 8;
	uint cntOfBytes = 0;
	while (chunkType != IEND_CHUNK_TYPE)
	{
		if (chunkType == IDAT_CHUNK_TYPE)
		{
			// parse data
			uint chunkLength = getNumber4Bytes(chunk.len);
			unsigned char *chunkData = malloc(sizeof(unsigned char) * chunkLength);
			if (chunkData == NULL)
			{
				fprintf(stderr, "Memory error occurred!\n");
				free(allData);
				fclose(in);
				return ERROR_MEMORY;
			}
			c = fread(chunkData, sizeof(unsigned char), chunkLength, in);
			if (c != chunkLength)
			{
				// chunkData.length is not chunkLength
				free(chunkData);
				free(allData);
				return InvalidDataError(in);
			}
			while (chunkLength + cntOfBytes > allDataLength)
			{
				unsigned char *allData1 = realloc(allData, sizeof(unsigned char) * allDataLength * 2);
				if (allData1 == NULL)
				{
					allData1 = realloc(allData, sizeof(unsigned char) * allDataLength * (5 / 4));
					if (allData1 == NULL)
					{
						free(allData);
						free(chunkData);
						fprintf(stderr, "Error. Not enough memory\n");
						fclose(in);
						return ERROR_NOT_ENOUGH_MEMORY;
					}
					else
					{
						allDataLength *= (5 / 4);
					}
				}
				else
				{
					allDataLength *= 2;
				}
				allData = allData1;
			}
			for (uint i = 0; i < chunkLength; i++)
			{
				allData[i + cntOfBytes] = chunkData[i];
			}
			cntOfBytes += chunkLength;
			free(chunkData);
			c = fread(&chunk.crc, sizeof(unsigned char), 4, in);
			if (c != 4)
			{
				// chunk.crc is not 4 bytes
				free(allData);
				return InvalidDataError(in);
			}
		}
		else
		{
			if (chunkType == IHDR_CHUNK_TYPE || chunkType == INVALID_CHUNK_TYPE)
			{
				// Have met invalid chunk type
				free(allData);
				return InvalidDataError(in);
			}
			else
			{
				// skip chunk
				uint skipCount = getNumber4Bytes(chunk.len) + 4;
				unsigned char *skippedBytes = malloc(sizeof(unsigned char) * skipCount);
				if (skippedBytes == NULL)
				{
					fprintf(stderr, "Not enough memory error!\n");
					free(allData);
					fclose(in);
					return ERROR_MEMORY;
				}
				c = fread(skippedBytes, sizeof(unsigned char), skipCount, in);
				if (c != skipCount)
				{
					free(skippedBytes);
					free(allData);
					return InvalidDataError(in);
				}
				free(skippedBytes);
			}
		}
		c = fread(chunk.len, sizeof(unsigned char), 4, in);
		if (c != 4)
		{
			// chunk.len is not 4 bytes
			free(allData);
			return InvalidDataError(in);
		}
		c = fread(chunk.type, sizeof(unsigned char), 4, in);
		if (c != 4)
		{
			// chunk.type is not 4 bytes
			free(allData);
			return InvalidDataError(in);
		}
		chunkType = getChunkType(chunk.type);
	}
	if (getNumber4Bytes(chunk.len) != 0)
	{
		// IEND.length not 0
		free(allData);
		return InvalidDataError(in);
	}
	c = fread(&chunk.crc, sizeof(unsigned char), 4, in);
	if (c != 4)
	{
		// IEND.crc is not 4 bytes
		free(allData);
		return InvalidDataError(in);
	}
	unsigned char finalByte[1];
	c = fread(&finalByte, sizeof(unsigned char), 1, in);
	if (c != 0)
	{
		// IEND is NOT END OF FILE
		free(allData);
		return InvalidDataError(in);
	}
	unsigned char *allData1 = realloc(allData, cntOfBytes);
	if (allData1 == NULL)
	{
		fprintf(stderr, "Memory error!\n");
		fclose(in);
		free(allData);
		return ERROR_MEMORY;
	}
	allData = allData1;
	allDataLength = cntOfBytes;
	fclose(in);
	// Post: successfully parsed all chunks. allData - array with all compressed bytes
	//---------------------------------------------------------
	// SUCCESSFULLY PARSED ALL INPUT DATA. NOW NEED TO CONVERT OUR BYTES TO PNM-FORMAT
	//###################################################################
	// convert to pnm
	//###################################################################
	//---------------------------------------------------------
	// debug output
#if MY_DEBUG_OUTPUT
	printf("Width: %u, Height: %u\n", width, height);
	printf("Summary IDAT.data length is %u\n", allDataLength);
	printf("All IDAT.data is below:\n");
	for (uint i = 0; i < allDataLength; i++)
	{
		printf("%u ", allData[i]);
	}
	printf("\n");
#endif
	//---------------------------------------------------------
	// uncompress
	//---------------------------------------------------------
	uint coef = (colorType == COLORED_IMAGE ? 3 : 1);
	ull uncompressedDataLength = (width * coef + 1) * height;
	unsigned char *uncompressedData = malloc(sizeof(unsigned char) * uncompressedDataLength);
	if (uncompressedData == NULL)
	{
		fprintf(stderr,
				"Memory error. Couldn't allocate memory for uncompressed data!\n"
				"Please, try again later or reduce the size of the input data!\n");
		free(allData);
		return ERROR_MEMORY;
	}
#if MY_DEBUG_OUTPUT
	printf("There wasn't error before uncompress!\n");
	printf("%u, %u\n", uncompressedDataLength, allDataLength);
#endif
#if defined ZLIB
	int code = uncompress(uncompressedData, (uLongf *)&uncompressedDataLength, allData, allDataLength);
	if (code != Z_OK)
	{
		fprintf(stderr,
				"Some error occurred while were uncompressing input data!\n"
				"Please, make sure that your input data is correct!\n");
		free(uncompressedData);
		free(allData);
		return ERROR_INVALID_DATA;
	}
	// Post: code == Z_OK
#else
	#error "Couldn't uncompress bytes because an unsupported library was trying to include!"
#endif
	free(allData);

#if MY_DEBUG_OUTPUT
	printf("%i, %u, %u\n", code == Z_OK ? 1 : 0, uncompressedDataLength, (width * (colorType == COLORED_IMAGE ? 3 : 1) + 1) * height);
	printf("Uncompressed data before filtering is below:\n");
	for (uint i = 0; i < uncompressedDataLength; i++)
	{
		printf("%u ", uncompressedData[i]);
	}
	printf("\n");
#endif
	// Post: successfully uncompressed all bytes!
	//       uncompressedData is array that contains uncompressed bytes
	//       uncompressedDataLength == (width*coef + 1) * height, where
	//                                  coef == 3 for colored images (red, green and blue chanel)
	//                                  coef == 1 for black and white images
	//---------------------------------------------------------
	// apply filters
	//---------------------------------------------------------
	uint i = 0;
	while (i < uncompressedDataLength)
	{
		if (uncompressedData[i] == FILTER_TYPE_NONE)
		{
			// do nothing
			i += (coef * width + 1);
		}
		else if (uncompressedData[i] == FILTER_TYPE_SUB)
		{
			// + left
			uint i1 = coef;
			i++;
			i += coef;
			while (i1 < width * coef)
			{
				uncompressedData[i] += uncompressedData[i - coef];
				i++;
				i1++;
			}
		}
		else if (uncompressedData[i] == FILTER_TYPE_UP)
		{
			// + above
			uint i1 = 0;
			i++;
			while (i1 < width * coef)
			{
				if (i >= (width * coef + 1))
				{
					uncompressedData[i] += uncompressedData[i - (width * coef + 1)];
				}
				i++;
				i1++;
			}
		}
		else if (uncompressedData[i] == FILTER_TYPE_AVERAGE)
		{
			// (above + left) / 2
			uint i1 = 0;
			i++;
			while (i1 < width * coef)
			{
				unsigned char above = i >= (width * coef + 1) ? uncompressedData[i - (width * coef + 1)] : 0;
				unsigned char left = i1 < coef ? 0 : uncompressedData[i - coef];
				uncompressedData[i] += ((above + left) / 2);
				i++;
				i1++;
			}
		}
		else if (uncompressedData[i] == FILTER_TYPE_PAETH)
		{
			// func(left, above, leftAbove);
			uint i1 = 0;
			i++;
			while (i1 < width * coef)
			{
				unsigned char above = i >= (width * coef + 1) ? uncompressedData[i - (width * coef + 1)] : 0;
				unsigned char left = i1 < coef ? 0 : uncompressedData[i - coef];
				unsigned char leftAbove =
					(i1 >= coef && i >= (width * coef + 1)) ? uncompressedData[i - (width * coef + 1) - coef] : 0;
				uncompressedData[i] += paethPredictor(left, above, leftAbove);
				i++;
				i1++;
			}
		}
		else
		{
			fprintf(stderr,
					"Error. Wrong format of input data.\n"
					"Please, make sure that your input file is correct!\n");
			free(uncompressedData);
			return ERROR_INVALID_DATA;
		}
	}
#if MY_DEBUG_OUTPUT
	printf("Uncompressed data after filtering is below:\n");
	for (uint i1 = 0; i1 < uncompressedDataLength; i1++)
	{
		printf("%u ", uncompressedData[i1]);
	}
	printf("\n");
#endif
	// Post: uncompressed data contains true pixels bytes with filter byte in the beginning of each row
	//       uncompressedDataLength is still equals (width*coef+1) * height
	//---------------------------------------------------------
	// SUCCESSFULLY UNCOMPRESSED ALL PIXEL BYTES! NOW WE NEED ONLY TO PUSH THEM INTO OUTPUT FILE
	//###################################################################
	// output
	//###################################################################
	FILE *out = fopen(argv[2], "wb");
	if (out == NULL)
	{
		fprintf(stderr,
				"Error. Couldn't open output data file.\n"
				"Please, make sure that name of output file is correct!\n");
		free(uncompressedData);
		return ERROR_FILE_NOT_FOUND;
	}
	fprintf(out, "P%i\n%i %i\n255\n", (colorType == COLORED_IMAGE ? 6 : 5), width, height);
	for (uint i1 = 0; i1 < uncompressedDataLength; i1++)
	{
		if (i1 % (width * coef + 1) != 0)
		{
			fprintf(out, "%c", uncompressedData[i1]);
		}
	}
	fclose(out);
	free(uncompressedData);
	return ERROR_SUCCESS;
}


//#include "return_codes.h"
//
//#include <stdio.h>
//#include <stdlib.h>
//
//#if defined(ZLIB)
//	#include <zlib.h>
//#endif
//#if defined(LIBDEFLATE)
//	#error "This code don't support libdeflate"
//#endif
//#if defined(ISAL)
//	#error "This code don't support isa-l"
//#endif
//struct Chunk
//{
//	int correct_ending;
//	int length;
//	unsigned char *data;
//	unsigned char *name;
//};
//
//int read_four_bytes(const unsigned char *chunk, int start)
//{
//	int k = 1;
//	int result = 0;
//	for (int i = 3; i >= 0; i--)
//	{
//		result += (int)chunk[start + i] * k;
//		k *= 256;
//	}
//	return result;
//}
//
//struct Chunk parse_chunk(FILE *in)
//{
//	struct Chunk chunk;
//	unsigned char *lenData = malloc(sizeof(unsigned char) * 4);
//	if (lenData == NULL)
//	{
//		fprintf(stderr, "Not enough memory resources are available to process this command");
//		chunk.correct_ending = ERROR_NOT_ENOUGH_MEMORY;
//		return chunk;
//	}
//	unsigned int check = fread(lenData, sizeof(unsigned char), 4, in);
//	if (check < 4)
//	{
//		fprintf(stderr, "Incorrect structure of chunk");
//		chunk.correct_ending = ERROR_INVALID_DATA;
//		free(lenData);
//		return chunk;
//	}
//	chunk.length = read_four_bytes(lenData, 0);
//	free(lenData);
//
//	chunk.name = malloc(sizeof(unsigned char) * 4);
//	if (chunk.name == NULL)
//	{
//		fprintf(stderr, "Not enough memory resources are available to process this command");
//		chunk.correct_ending = ERROR_NOT_ENOUGH_MEMORY;
//		return chunk;
//	}
//	check = fread(chunk.name, sizeof(unsigned char), 4, in);
//	if (check < 4)
//	{
//		fprintf(stderr, "Incorrect structure of chunk");
//		chunk.correct_ending = ERROR_INVALID_DATA;
//		free(chunk.name);
//		return chunk;
//	}
//
//	chunk.data = malloc(sizeof(unsigned char) * chunk.length);
//	if (chunk.data == NULL)
//	{
//		fprintf(stderr, "Not enough memory resources are available to process this command");
//		chunk.correct_ending = ERROR_NOT_ENOUGH_MEMORY;
//		free(chunk.name);
//		return chunk;
//	}
//	check = fread(chunk.data, sizeof(unsigned char), chunk.length, in);
//	if (check < chunk.length)
//	{
//		fprintf(stderr, "Incorrect structure of chunk");
//		chunk.correct_ending = ERROR_INVALID_DATA;
//		free(chunk.name);
//		free(chunk.data);
//		return chunk;
//	}
//
//	unsigned char *temp = malloc(sizeof(unsigned char) * 4);
//	if (temp == NULL)
//	{
//		fprintf(stderr, "Not enough memory resources are available to process this command");
//		chunk.correct_ending = ERROR_NOT_ENOUGH_MEMORY;
//		free(chunk.name);
//		free(chunk.data);
//		return chunk;
//	}
//	check = fread(temp, sizeof(unsigned char), 4, in);
//	if (check < 4)
//	{
//		fprintf(stderr, "Incorrect structure of chunk");
//		chunk.correct_ending = ERROR_INVALID_DATA;
//		free(chunk.name);
//		free(chunk.data);
//		return chunk;
//	}
//	free(temp);
//	chunk.correct_ending = ERROR_SUCCESS;
//	return chunk;
//}
//
//unsigned char PaethPredictor(a, b, c)
//{
//	int p = a + b - c;
//	int pa = abs(p - a);
//	int pb = abs(p - b);
//	int pc = abs(p - c);
//	if (pa <= pb && pa <= pc)
//	{
//		return a;
//	}
//	else if (pb <= pc)
//	{
//		return b;
//	}
//	else
//	{
//		return c;
//	}
//}
//
//int main(int argc, char *argv[])
//{
//	if (argc != 3)
//	{
//		fprintf(stderr, "Not enough arguments");
//		return ERROR_INVALID_PARAMETER;
//	}
//	FILE *in = fopen(argv[1], "rb");
//	if (in == NULL)
//	{
//		fprintf(stderr, "The system cannot find the in file");
//		return ERROR_NOT_FOUND;
//	}
//
//	// Work with IHDR and Beginning
//	unsigned char *beginning = malloc(sizeof(unsigned char) * 8);
//	if (beginning == NULL)
//	{
//		fprintf(stderr, "Not enough memory resources are available to process this command");
//		fclose(in);
//		return ERROR_NOT_ENOUGH_MEMORY;
//	}
//	int n = 0;
//	while (n != 8 && !feof(in))
//	{
//		fscanf(in, "%c", &beginning[n]);
//		n++;
//	}
//	if (n != 8)
//	{
//		fprintf(stderr, "Incorrect structure of beginning");
//		fclose(in);
//		return ERROR_INVALID_DATA;
//	}
//	if (!(beginning[0] == 137 && beginning[1] == 80 && beginning[2] == 78 && beginning[3] == 71 && beginning[4] == 13 &&
//		  beginning[5] == 10 && beginning[6] == 26 && beginning[7] == 10))
//	{
//		free(beginning);
//		fprintf(stderr, "Incorrect structure of beginning");
//		fclose(in);
//		return ERROR_INVALID_DATA;
//	}
//	free(beginning);
//	struct Chunk IHDR = parse_chunk(in);
//	if (IHDR.correct_ending != ERROR_SUCCESS)
//	{
//		fclose(in);
//		fclose(in);
//		return IHDR.correct_ending;
//	}
//
//	if (IHDR.length != 13 || IHDR.name[0] != 'I' || IHDR.name[1] != 'H' || IHDR.name[2] != 'D' || IHDR.name[3] != 'R')
//	{
//		free(IHDR.name);
//		free(IHDR.data);
//		fclose(in);
//		fprintf(stderr, "Incorrect structure of IHDR");
//		return ERROR_INVALID_DATA;
//	}
//
//	int height = read_four_bytes(IHDR.data, 0), width = read_four_bytes(IHDR.data, 4);
//	int colorType = (int)IHDR.data[9];
//	free(IHDR.data);
//	free(IHDR.name);
//
//	unsigned char *allData = malloc(sizeof(unsigned char) * 4);
//	int size = 4;
//	int currInd = 0;
//	// Read chunks
//	struct Chunk chunk;
//	do
//	{
//		chunk = parse_chunk(in);
//		if (chunk.correct_ending != ERROR_SUCCESS)
//		{
//			fclose(in);
//			free(allData);
//			return chunk.correct_ending;
//		}
//		if (chunk.name[0] == 'I' || chunk.name[1] == 'D' || chunk.name[2] == 'A' || chunk.name[3] == 'T')
//		{
//			while (chunk.length + currInd > size)
//			{
//				unsigned char *copy = realloc(allData, sizeof(unsigned char) * size * 2);
//				size = size * 2;
//				if (copy)
//				{
//					allData = copy;
//				}
//				else
//				{
//					free(chunk.data);
//					free(chunk.name);
//					free(allData);
//					fclose(in);
//					fprintf(stderr, "Not enough memory resources are available to process this command");
//					return ERROR_NOT_ENOUGH_MEMORY;
//				}
//			}
//
//			for (int i = 0; i < chunk.length; i++)
//			{
//				allData[currInd + i] = chunk.data[i];
//			}
//			currInd += chunk.length;
//			size = currInd;
//		}
//		free(chunk.data);
//	} while ((chunk.name[0] != 'I' || chunk.name[1] != 'E' || chunk.name[2] != 'N' || chunk.name[3] != 'D') && !feof(in));
//
//	if (chunk.name[0] != 'I' || chunk.name[1] != 'E' || chunk.name[2] != 'N' || chunk.name[3] != 'D')
//	{
//		fprintf(stderr, "Incorrect structure of file");
//		free(chunk.name);
//		free(allData);
//		fclose(in);
//		return ERROR_INVALID_DATA;
//	}
//	free(chunk.name);
//	uLong newDataLen = 2;
//	unsigned char *newData = malloc(sizeof(unsigned char) * newDataLen);
//	if (!newData)
//	{
//		fprintf(stderr, "Not enough memory resources are available to process this command");
//		free(allData);
//		fclose(in);
//		return ERROR_NOT_ENOUGH_MEMORY;
//	}
//	int resul = uncompress(newData, &newDataLen, allData, size);
//	if (resul == Z_DATA_ERROR)
//	{
//		fprintf(stderr, "Incorrect structure of PNG");
//		free(newData);
//		free(allData);
//		fclose(in);
//		return ERROR_INVALID_DATA;
//	}
//	if (resul == Z_MEM_ERROR)
//	{
//		fprintf(stderr, "Not enough memory resources are available to process this command");
//		free(newData);
//		free(allData);
//		fclose(in);
//		return ERROR_NOT_ENOUGH_MEMORY;
//	}
//	while (resul == Z_BUF_ERROR)
//	{
//		newDataLen *= 2;
//		free(newData);
//		newData = malloc(sizeof(unsigned char) * newDataLen);
//		if (!newData)
//		{
//			fprintf(stderr, "Not enough memory resources are available to process this command");
//			free(allData);
//			fclose(in);
//			return ERROR_NOT_ENOUGH_MEMORY;
//		}
//		resul = uncompress(newData, &newDataLen, allData, size);
//	}
//	free(allData);
//	fclose(in);
//
//	int pixMult;
//	if (colorType == 0)
//	{
//		pixMult = 1;
//	}
//	else if (colorType == 2)
//	{
//		pixMult = 3;
//	}
//	else
//	{
//		fprintf(stderr, "Unknown color type");
//		free(newData);
//		fclose(in);
//		return ERROR_CALL_NOT_IMPLEMENTED;
//	}
//	if (newDataLen != width * height * pixMult + height)
//	{
//		fprintf(stderr, "Incorrect structure of PNG");
//		free(newData);
//		fclose(in);
//		return ERROR_INVALID_DATA;
//	}
//	for (int i = 0; i < newDataLen; i += (int)width * pixMult + 1)
//	{
//		int alg = (int)newData[i];
//		for (int j = 1; j <= width * pixMult; j++)
//		{
//			if (alg == 1)
//			{
//				if (j > pixMult)
//				{
//					newData[i + j] = newData[i + j] + newData[i + j - pixMult];
//				}
//			}
//			else if (alg == 2)
//			{
//				if (i > 0)
//				{
//					newData[i + j] = newData[i + j] + newData[i - (width * pixMult + 1) + j];
//				}
//			}
//			else if (alg == 3)
//			{
//				unsigned char first = 0;
//				unsigned char second = 0;
//				if (j > pixMult)
//				{
//					first = newData[i + j - pixMult];
//				}
//				if (i > 0)
//				{
//					second = newData[i - (width * pixMult + 1) + j];
//				}
//				newData[i + j] = newData[i + j] + (first + second) / 2;
//			}
//			else if (alg == 4)
//			{
//				unsigned char first = 0;
//				unsigned char second = 0;
//				unsigned char third = 0;
//				if (j > pixMult)
//				{
//					first = newData[i + j - pixMult];
//				}
//				if (i > 0)
//				{
//					second = newData[i - (width * pixMult + 1) + j];
//				}
//				if (i > 0 && j > pixMult)
//				{
//					third = newData[i - (width * pixMult + 1) + j - pixMult];
//				}
//				newData[i + j] = newData[i + j] + PaethPredictor(first, second, third);
//			}
//			else if (alg != 0)
//			{
//				fprintf(stderr, "Mistake in structure of ");
//				free(newData);
//				fclose(in);
//				return ERROR_INVALID_DATA;
//			}
//		}
//	}
//
//	// Output
//	FILE *out = fopen(argv[2], "wb");
//	if (out == NULL)
//	{
//		fprintf(stderr, "The system cannot find the out file");
//		fclose(in);
//		return ERROR_FILE_NOT_FOUND;
//	}
//	if (colorType == 0)
//	{
//		fprintf(out, "P5\n");
//	}
//	else
//	{
//		fprintf(out, "P6\n");
//	}
//	fprintf(out, "%i %i\n255\n", width, height);
//
//	for (int i = 0; i < width * height * pixMult + height; i++)
//	{
//		if (i % (width * pixMult + 1) != 0)
//		{
//			fprintf(out, "%c", newData[i]);
//		}
//	}
//	fclose(out);
//	free(newData);
//	fclose(out);
//
//	return ERROR_SUCCESS;
//}