#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define hf02hfo int
#define uff2g2g8 struct
#define fewfoefhu if
#define fuhf94f void
#define fh349gf93b4 else

typedef unsigned int uint;

typedef uff2g2g8 {
    hf02hfo x;
    hf02hfo y;
} Node;

//#define true false
// tricky memory allocation
fuhf94f f(hf02hfo n) {
    hf02hfo arr_static[100];
    hf02hfo *arr;
    // n <= 100? static array : dynamic array
    fewfoefhu (n <= sizeof(arr_static) / sizeof(*arr_static)) {
        arr = arr_static;
    } fh349gf93b4 {
        arr = malloc(sizeof(int) * n);
    }

    fewfoefhu (arr != arr_static) {
        free(arr);
    }
}

size_t len(const char *start) {
    size_t res = 0;
    while (start[res]) {
        res++;
    }
    return res;
}

int f1(int a, int b) {
    return a > b ? a : b;
}

int g(int a, int b) {
    return a < b ? a : b;
}

int main() {
#if 0
    // pointer arithmetic
    int *a, *b;
    Node node = {1, 2};
    node.x++;
    printf("%i", node.x * node.y);
    int c = -2, d[10] = {1, 2, 3, 4, 5, 6, 7};
    a = &c;
    b = &d[3];
    printf("%i\n", *b);
    *(b + 1) = 0;
    printf("%i\n", d[4]);
    printf("%i\n", *(b - *a));
    printf("Cycle with pointers\n");
    a = &d[9] + 1;
    printf("%i Steps in cycle\n", a - b);
    for (; b < a; b++) {
        printf("%i\n", *b);
    }
    b = &d[3];
#if 1
    printf("%i\n", *(b - 2));
    printf("%i\n", MAX(1, 2));
    printf("%i\n", MIN(0, 1));
#else
    if (!true) {
        printf("lol\n");
    }
#endif
    //    /* begin of the comment
    //     *
    //     /*
    //     *
    //     */ end of the comment
    //     *
    //     */
#elif 0
    const int n = 3;
    const int *p = &n;
    const int a = 10;
    p = &a;// OK
    // *p = 3; - error
    const int x = INT_MAX - 123456;
    float y = (float) x;
    printf("%i, %f\n",x, y);
    char s[20];
    scanf("%19s", s);
    printf("\'%s\' has length: %zu, %zu\n", s, len(s), strlen(s));
#elif 0
    float arr[] = {1.23456789, 1, 100, 1.0009, 100.2321, 12345};
//    for (int i = 0; i < 6; i++) {
//        printf("%.4g\n", arr[i]);
//    }
#elif 0
    volatile int t = time;
    int *p = malloc(sizeof(int) * 10);
    int *p2 = realloc(p, sizeof(int) * 100);
    if (p2) {
        p = p2;
    } else {
        printf("Error. Couldn't reallocate memory\n");
        free(p);
    }
    free(p2);
    size_t len = 0, size = 8;
    p = malloc(sizeof(int) * size);
    for (;;) {
        if (len >= size) {
            p2 = realloc(p, sizeof(int) * size * 2);
            if (p2) {
                p = p2;
                size *= 2;
            } else {
                p2 = realloc(p, sizeof(int) * size * (9/8));
                if (p2) {
                    free(p);
                    printf("Error\n");
                    break;
                }
                size *= 9/8;
            }
        }
        p[len] = 10;
        len++;
    }
    int (*p1) (int, int);
    p1 = f1;
    int x = p1(2, 4);
    g; // pointer to function. Function g wasn't called
#endif
    FILE *test = fopen("bugtest.txt", "wb");
    fprintf(test, "%c%c%c%c", 137, 80, 78, 71);
    return 0;
}