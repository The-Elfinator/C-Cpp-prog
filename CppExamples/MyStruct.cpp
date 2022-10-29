#include "MyStruct.h"
#include <cmath>
#include <stdio.h>

int MyStruct::myAbs() const {
    return abs(this->x);
}

int MyStruct::myAbs(int c) const {
    return abs(this->x) + c;
}

int MyStruct::f() {
    // f не видит поля MyStruct
    // f можно вызывать без создания объекта
    return 1;
}

MyStruct MyStruct::operator+(const MyStruct &b) const {
    MyStruct res{};
    res.x = this->x + b.x;
    res.y = this->y + b.y;
    return res;
}

int MyStruct::operator() () const {
    return -100000000;
}

MyStruct::MyStruct() {
    this->x = 0;
    this->y = 10;
    printf("Here is call of constructor without args\n");
}

MyStruct::MyStruct(int a, int b) : x(a), y(b) {
    printf("Here is call of constructor with %i, %i\n", a, b);
}

MyStruct::~MyStruct() {
    printf("Here is call of destructor for MyStruct with x = %i, y = %i\n", this->x, this->y);
}
