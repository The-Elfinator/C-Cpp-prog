#include <iostream>
#include <cmath>
#include "MyStruct.h"

// #include <math.h> - don't include math.h in C++ file

#define arguments 0

using namespace std;
//
// #ifdef __cplusplus
// extern "C"
// {
// #endif
//     int f(int i);
// }

//int f(int) {
//    return 1;
//}

//float f(float) {
//    return 1.0f;
//}
// //
// //int f(int, float x = 0) {
// //    return 1 + 1.0f;
// //}
//

//int g(int a, int b = 0) {
//    return a + b;
//}
//int f(int a);
//
//int x = f(2);
//int y = x + 1;
//
//int f(int a) {
//    return y + a;
//}
static int y = 10;

int f(int a) {
    static int b = a + y;
    return b;
}

int g() {
    return 1;
}

namespace my {
    int g() {
        return 2;
    }
}

//namespace {
//    int g() {
//        return 3;
//    }
//}

void f1(int &a) {
    a += 3;
}

template <typename T>
T myAbs (T x) {
    return x < 0 ? -x : x;
}

template <typename T>
T add(T a, T b) {
    return a + b;
}

struct My {
    int m_x;
    static const int y = 2;
//    int myAbs(int c) const; //prototype
    int myAbs() const {
        return abs(this->m_x);
    }
} a, b;

struct NotMy {
    int m_x;
    int myAbs() const {
        return abs(this->m_x);
    }
} a1, b1;

struct Struct {
    int x;
    int y;
    Struct(int _a, int _b) : x(_a), y(_b) {}
//    Struct(int _a): x(_a), y(0) {}
//    explicit Struct(double _b): x(_b*10), y(0) {}
//    operator int() {return x + y;}
    int operator[] (int _a) const {return _a + x;}
    explicit operator bool() const {return x != 0;}
};
//
//MyStruct operator+ (const MyStruct &first, const MyStruct &second) {
//    MyStruct res{};
//    res.x = first.x + second.x;
//    res.y = first.y + second.y;
//    return res;
//}

int operator+ (const MyStruct &first, int &second) {
    second = 10;
    return 0;
}

int operator> (const MyStruct &first, const float &second) {
    return 0;
}

MyStruct operator++ (MyStruct &first, int) {
    MyStruct res = first;
    first.x++;
    first.y++;
    return res;
}

const My c{-100};
const NotMy c1{-123};
//int myAbs (const My &z) {
//    return abs(z.x);
//}

#if arguments
int main(int argc, char **argv) {
#else
int main(){
#endif
    printf("Hello, World!\n");
#if 0
    int a = -2;
    float b = -3.0f;
    printf("%i, %f\n", abs(a), abs(b));
    cout << g(1) << ' ' << g(1, 1) << '\n';
    cout << f(1.0f) << '\n';

    int n = 10;
    int *p = (int *) malloc(sizeof(int)*n);
    for (int i = 0; i < n; i++) {
        p[i] = i;
    }
    int *p2 = (int *) realloc(/*(void *)*/ p, n*2);
    free(p);
    cout << (p2 != nullptr) << '\n';
    free(p2);
#elif 0
    cout << f(2) <<'\n';
    cout << f(12412) << '\n';
    cout << g() << '\n';
    cout << my::g() << '\n';
    int x = 2;
    int &y1 = x;
    f1(x);
    cout << x << '\n';
    cout << myAbs(-2) << ' ' << myAbs(4.0) << '\n';
    cout << add(2, 3) << ' ' << add<float>(0, 4.0) << '\n';
#elif 0
    a.m_x = 10;
    b.m_x = -2;
    a1.m_x = -10;
    b1.m_x = 2;
//    My::y = 2;
    cout << My::y << '\n';
    cout << a.myAbs() << ' ' << b.myAbs() << ' ' << c.myAbs() << '\n';
    cout << a1.myAbs() << ' ' << b1.myAbs() << ' ' << c1.myAbs() << '\n';
    MyStruct myStruct{-100};
    cout << myStruct.myAbs() << ' ' << myStruct.myAbs(10) << '\n';
    cout << MyStruct::f() << '\n';
    MyStruct myStruct1{-10, 20};
    MyStruct myStruct2{10, -10};
    MyStruct res = myStruct1 + myStruct2;
    cout << res.x << ' ' << res.y << '\n';
    MyStruct abac = myStruct1++;
    cout << abac.x << ' ' << myStruct1.x << '\n';
    cout << myStruct1() << '\n';
    int var{2};
    cout << var << '\n';
    // MyStruct *p = new(nothrow) MyStruct;
#elif 1
    Struct z(2, 3);
//  operator bool() has `explicit` so this line not compiled:
//  int a = z;
//  cout << a << '\n';
    if (z) {
        cout << z.x << ' ' << z.y << '\n';
    } else {
        cout << "z.x is zero!\n";
    }
    int a = z[10];
    // int a = z[10, 3]; <---- operator ','. expression equals to `int a = z[3]`;
    cout << a << '\n';
    if (int x = -1; float y = f(-10)) {
        cout << "Yes\n";
    }
#endif
    return 0;
}
