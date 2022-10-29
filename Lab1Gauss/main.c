// lab1

#include <malloc.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

bool deb = false;
float eps;

// debugging output. Just for checking for myself (if deb than dump, else nothing)
void dump(int n, float *arr)
{
	if (deb)
	{
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				printf("%g ", arr[i * (n + 1) + j]);
			}
			printf("%g\n", arr[i * (n + 1) + n]);
		}
	}
}

// Res:
// 1 - no solutions
// 2 - many solutions
// 0 - single solution
int straightMove(int n, float *arr)
{
	bool noSolutions = false;
	bool manySolutions = false;
	for (int k = 0; k < n; k++)
	{
		if (deb)
		{
			printf("current row is: %i\n", k);
		}
		dump(n, arr);
		int stepByRow = 0;
		if (fabsf(arr[k * (n + 1) + k]) <= eps)
		{
			// swapping with non-zero element

			// find non-zero element in column
			int ind = -1;
			for (int i = k + 1; i < n; i++)
			{
				if (fabsf(arr[i * (n + 1) + k]) > eps)
				{
					ind = i;
					break;
				}
			}
			if (ind != -1)
			{
				// swap
				float tmp;
				for (int j = 0; j < n + 1; j++)
				{
					tmp = arr[ind * (n + 1) + j];
					arr[ind * (n + 1) + j] = arr[k * (n + 1) + j];
					arr[k * (n + 1) + j] = tmp;
				}
			}
			else
			{
				// Go to another column if all elements in a column below our current row is zero
				for (int j = k + 1; j < n; j++)
				{
					if (fabsf(arr[k * (n + 1) + j]) > eps)
					{
						stepByRow = j - k;
						break;
					}
				}
				if (stepByRow == 0)
				{
					if (fabsf(arr[k * (n + 1) + n]) > eps)
					{
						noSolutions = true;
						break;
					}
					else
					{
						manySolutions = true;
						continue;
					}
				}
			}
		}
		// division by k-th element
		float tmp = arr[k * (n + 1) + k + stepByRow];	 // + k --- if we go to another column in "else upper" than we
														 // need to have a step
		for (int j = k + stepByRow; j < n + 1; j++)
		{
			arr[k * (n + 1) + j] /= tmp;
		}
		// subtracting strings
		for (int i = k + 1; i < n; i++)
		{
			bool flag = true;
			float koef = arr[i * (n + 1) + k + stepByRow];
			for (int j = k + stepByRow; j < n + 1; j++)
			{
				arr[i * (n + 1) + j] -= arr[k * (n + 1) + j] * koef;
				if (j < n)
				{
					flag = flag && (arr[i * (n + 1) + j] == 0);
				}
			}
			// after we subtract j-st row we have 2 variants:
			// 1) 0*X_i = const where const != 0. It means that we have no solutions
			// 2) 0*X_i = 0 It means that rank(A) < n => many solutions(?)But we need to check that in rows below won't
			// be 1-st variant
			if (flag)
			{
				if (fabsf(arr[i * (n + 1) + n]) > eps)
				{
					// NO SOLUTIONS
					noSolutions = true;
					manySolutions = false;
					break;
				}
				else
				{
					// Probably many solutions
					manySolutions = true;
				}
			}
		}
		if (noSolutions || manySolutions)
		{
			break;
		}
	}
	dump(n, arr);
	if (noSolutions)
	{
		return 1;
	}
	if (manySolutions)
	{
		return 2;
	}
	return 0;
}

// only for single solution
void backMove(int n, float *arr)
{
	for (int j = n - 1; j >= 0; j--)
	{
		for (int i = j - 1; i >= 0; i--)
		{
			arr[i * (n + 1) + n] -= arr[i * (n + 1) + j] * arr[j * (n + 1) + n];
			arr[i * (n + 1) + j] = 0;
		}
	}
}

int main(int argsCnt, char *args[])
{
	if (argsCnt != 3)
	{
		printf("Wrong count of arguments!\n");
		printf("There must be only name of input and output files!\n");
		return 0;
	}
	// input
	FILE *in = fopen(args[1], "r");
	if (in == NULL)
	{
		// Error: FileNotFound
		printf("Couldn't open input file OR input file not found\n");
		printf("Please, make sure that command line arguments are correct\n");
		return 1;
	}
	int n;
	fscanf(in, "%i", &n);
	float *arr = malloc(sizeof(float) * (n + 1) * n);
	if (arr == NULL)
	{
		// MemoryAllocationError
		fclose(in);
		printf("N is too large. Couldn't allocate memory. Please, change input data!\n");
		return 2;
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n + 1; j++)
		{
			fscanf(in, "%g", &arr[i * (n + 1) + j]);
		}
	}
	fclose(in);
	eps = 0.00001f;

	// Straight move
	int res = straightMove(n, arr);
	bool noSolutions = false;
	bool manySolutions = false;
	if (res == 1)
	{
		noSolutions = true;
	}
	else if (res == 2)
	{
		manySolutions = true;
	}

	// output
	FILE *out = fopen(args[2], "w");
	if (out == NULL)
	{
		// FileNotFound
		free(arr);
		printf("Couldn't open output file\n");
		printf("Please, make sure that command line arguments are correct\n");
		return 1;
	}
	if (noSolutions)
	{
		// Print no solutions
		fprintf(out, "no solution");
	}
	else if (manySolutions)
	{
		// Print many solutions
		fprintf(out, "many solutions");
	}
	else
	{
		// Back move (we have triangle matrix where on main diag '1' or '0')
		backMove(n, arr);
		// Print the answer. 4 signs after '.'
		for (int i = 0; i < n - 1; i++)
		{
			fprintf(out, "%.4g\n", arr[i * (n + 1) + n]);
		}
		fprintf(out, "%.4g", arr[(n - 1) * (n + 1) + n]);
	}
	fclose(out);
	// final matrix we had
	dump(n, arr);
	free(arr);
	return 0;
}
