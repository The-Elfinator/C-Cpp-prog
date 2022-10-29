#ifndef CPPEXAMPLES_MYSTRUCT_H
#define CPPEXAMPLES_MYSTRUCT_H
#endif

struct MyStruct {
    int x;
    int y;
    static int z;
    MyStruct();
    MyStruct(int a, int b = 0);
    int myAbs() const;
    int myAbs(int c) const;
    MyStruct operator+ (const MyStruct &b) const;
    int operator() () const;
    static int f();
    ~MyStruct();
};