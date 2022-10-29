#include "quicksort.h"
#include <random>

void quickSort(int *arr, const int l,const int r) {
    if (r - l == 2) {
        if (arr[l] > arr[l+1]) {
            int c = arr[l];
            arr[l] = arr[l+1];
            arr[l+1] = c;
        }
        return;
    }
    if (r - l <= 1) {
        return;
    }
    int l_less = 0, l_equal = 0, l_more = 0;
    int x = arr[l + rand() % (r - l)];
//    printf("quicksort is from %i to %i x is %i\n", l, r, x);
    for (int i = l; i < r; i++) {
//        printf("%i step of cycle: arr[i] == %i, x == %i\n", i, arr[i], x);
        if (arr[i] > x) {
            l_more++;
            continue;
        }
        if (arr[i] < x && l_equal >= 1) {
            int c = arr[i];
            arr[i] = arr[l + l_less];
            arr[l+l_less] = c;
            l_less++;
            l_equal--;
        }
        // arr[i] < x
        int c = arr[i];
        arr[i] = arr[l + l_less + l_equal];
        arr[l + l_less + l_equal] = c;
        if (arr[l+l_less + l_equal] < x) {
            l_less++;
        } else {
            l_equal++;
        }
//        printf("%i step of cycle: l1 = %i, l2 = %i, l3 = %i\n", i, l1, l2, l3);
    }
//    printf("after this recursive call array is\n");
//    for (int i = l; i < r; i++) {
//        printf("%i ", arr[i]);
//    }
//    printf("\n");
    quickSort(arr, l, l+ l_less);
    quickSort(arr, r - l_more, r);
}