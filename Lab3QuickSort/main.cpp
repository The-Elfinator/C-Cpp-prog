#include <iostream>
#include "return_codes.h"
//#include "quicksort.h"

using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
		fprintf(stderr, "Wrong count of command line arguments!\n"
						"You need to give me the name of input and output file.\n");
		return ERROR_INVALID_PARAMETER;
	}
	FILE *in = fopen(argv[1], "r");
	if (!in) {
		fprintf(stderr, "Couldn't open input file or input file not found!\n"
						"Please, make sure that your command line arguments is correct!\n");
		return ERROR_FILE_NOT_FOUND;
	}
	char type[9];
    unsigned long long c = fread(&type, sizeof(char), 4, in);
//    printf("%llu\n", c);
    if (c != 4) {
        fprintf(stderr, "Error! Invalid type of data\n");
        fclose(in);
        return ERROR_INVALID_DATA;
    }
    bool typeInt = false;
    bool typeFloat = false;
    bool typePhonebook = false;
    if (type[0] == 'i' && type [1] == 'n' && type[2] == 't') {
        typeInt = true;
    } else {
        fprintf(stderr, "Sorry but I can't work with that data type right now!\n"
                        "Please, try again later\n");
        fclose(in);
        return ERROR_NOT_IMPLEMENTED;
    }
//    printf("typeInt = %i\n", typeInt ? 1: 0);
    bool descending = true;
    char first[1];
    fread(&first, sizeof(char), 1, in);
//    printf("%c\n", first[0]);
    if (first[0] == 'a') {
//        printf("We are here!\n");
        descending = false;
        char rest[9];
        fread(&rest, sizeof(char), 9, in);
    } else {
        // size of buffer is one more than str.length because of LineSeparator
        char rest[10];
        fread(&rest, sizeof(char), 10, in);
    }
//    printf("%i\n", descending ? 1: 0);
    int n;
    fscanf(in, "%i\n", &n);
    int *arr = (int *) (malloc(sizeof(int) * n));
    for (int i = 0; i < n; i++) {
        fscanf(in, "%i\n", &arr[i]);
    }
    printf("Our array before sort is:\n");
    for (int i = 0; i < n; i++) {
        printf("%i\n", arr[i]);
    }
    //quickSort(arr, 0, n);
    printf("Our array after sort is:\n");
    for (int i = 0; i < n; i++) {
        printf("%i\n", arr[i]);
    }
    return 0;
}
