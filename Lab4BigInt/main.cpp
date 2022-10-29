#include "LN.h"
#include "return_codes.h"

#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		fprintf(stderr,
				"Wrong count of arguments!\n"
				"You should tell me only name of input and output file\n");
		return ERROR_INVALID_DATA;
	}
	FILE *in = fopen(argv[1], "r");
	if (!in)
	{
		fprintf(stderr, "Input file not found OR couldn't been opened!\n");
		return ERROR_FILE_NOT_FOUND;
	}
	// reading input file
	int numberMaxLength = 10000;
	int stackSize = 8;
	char *line = (char *)malloc(sizeof(char) * numberMaxLength);
	if (!line)
	{
		fprintf(stderr, "Couldn't allocate memory for reading input data\n");
		fclose(in);
		return ERROR_MEMORY;
	}
	LN *myStack = (LN *)malloc(sizeof(LN) * stackSize);
	if (!myStack)
	{
		fprintf(stderr, "Couldn't allocate memory for saving input data\n");
		fclose(in);
		free(line);
		return ERROR_MEMORY;
	}
	int ind = 0;
	int c = fscanf(in, "%s", line);
	char *line1 = (char *)malloc(sizeof(char) * numberMaxLength);
	for (int i = 0; i < numberMaxLength; i++)
	{
		line1[i] = line[i];
	}
	LN num1(line1);
	free(line1);
	myStack[0] = num1;
	ind++;
	c = fscanf(in, "%s", line);
	while (c != -1)
	{
		if (line[0] == '+' && line[1] == 0)
		{
			// add
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r = a + b;
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else if (line[0] == '-' && line[1] == 0)
		{
			// sub
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r = a - b;
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else if (line[0] == '_' && line[1] == 0)
		{
			// negate
			LN a = myStack[ind - 1];
			LN r = -a;
			myStack[ind - 1] = r;
		}
		else if (line[0] == '*' && line[1] == 0)
		{
			// notImplemented
			fprintf(stderr, "Multiply operand is not supported yet!\n");
			free(myStack);
			free(line);
			fclose(in);
			return ERROR_NOT_IMPLEMENTED;
		}
		else if (line[0] == '/' && line[1] == 0)
		{
			// notImplemented
			fprintf(stderr, "Division operand is not supported yet!\n");
			free(myStack);
			free(line);
			fclose(in);
			return ERROR_NOT_IMPLEMENTED;
		}
		else if (line[0] == '%' && line[1] == 0)
		{
			// notImplemented
			fprintf(stderr, "Module operand is not supported yet!\n");
			free(myStack);
			free(line);
			fclose(in);
			return ERROR_NOT_IMPLEMENTED;
		}
		else if (line[0] == '~' && line[1] == 0)
		{
			// notImplemented
			fprintf(stderr, "Sqrt operand is not supported yet!\n");
			free(myStack);
			free(line);
			fclose(in);
			return ERROR_NOT_IMPLEMENTED;
		}
		else if (line[0] == '<' && line[1] == 0)
		{
			// less
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r;
			if (a < b)
			{
				r = LN(1);
			}
			else
			{
				r = LN{};
			}
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else if (line[0] == '<' && line[1] == '=' && line[2] == 0)
		{
			// less or equal
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r;
			if (a <= b)
			{
				r = LN(1);
			}
			else
			{
				r = LN{};
			}
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else if (line[0] == '>' && line[1] == 0)
		{
			// more
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r;
			if (a > b)
			{
				r = LN(1);
			}
			else
			{
				r = LN{};
			}
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else if (line[0] == '>' && line[1] == '=' && line[2] == 0)
		{
			// more or equal
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r;
			if (a >= b)
			{
				r = LN(1);
			}
			else
			{
				r = LN{};
			}
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else if (line[0] == '=' && line[1] == '=' && line[2] == 0)
		{
			// equal
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r;
			if (a == b)
			{
				r = LN(1);
			}
			else
			{
				r = LN{};
			}
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else if (line[0] == '!' && line[1] == '=' && line[2] == 0)
		{
			// not equal
			LN b = myStack[ind - 1];
			LN a = myStack[ind - 2];
			LN r;
			if (a != b)
			{
				r = LN(1);
			}
			else
			{
				r = LN{};
			}
			myStack[ind - 2] = r;
			myStack[ind - 1] = LN{};
			ind--;
		}
		else
		{
			// number
			try
			{
				LN num(line);
				myStack[ind] = num;
				ind++;
				if (ind == stackSize)
				{
					LN *myStack1 = (LN *)realloc(myStack, sizeof(LN) * stackSize * 2);
					if (!myStack1)
					{
						myStack1 = (LN *)realloc(myStack, sizeof(LN) * stackSize * (5 / 4));
						if (!myStack1)
						{
							fprintf(stderr, "Not enough memory for saving input data\n");
							free(myStack);
							free(line);
							fclose(in);
							return ERROR_MEMORY;
						}
						stackSize *= (5 / 4);
					}
					else
					{
						stackSize *= 2;
					}
					myStack = myStack1;
				}
			} catch (char *message)
			{
				fprintf(stderr, "Error. Not enough memory to create LN\n");
				free(myStack);
				free(line);
				fclose(in);
				return ERROR_MEMORY;
			}
		}
		c = fscanf(in, "%s", line);
	}
	int n = ind;
	fclose(in);
	free(line);
	FILE *out = fopen(argv[2], "w");
	if (!out)
	{
		fprintf(stderr, "Output file not found OR couldn't been opened\n");
		free(myStack);
		return ERROR_FILE_NOT_FOUND;
	}
	for (int i = n - 1; i >= 0; i--)
	{
		if (myStack[i].sign == '-')
		{
			fprintf(out, "-");
		}
		for (int j = (int)myStack[i].numberLength - 1; j >= 0; j--)
		{
			fprintf(out, "%c", (char)myStack[i].digits[j]);
		}
		fprintf(out, "\n");
	}
	fclose(out);
	free(myStack);
	return ERROR_SUCCESS;
}
