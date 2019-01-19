#include <iostream>
#include <type_traits>
#include <memory>
#include "Function.h"

class Sum : public Function<int(int, int)>::Call {
public:
    virtual int operator()(int a, int b) const override {
        return a + b;
    }
};

class Id : public Function<int(int)>::Call {
public:
    virtual int operator()(int arg) const override {
        return arg;
    }
};

class Mul {
public:
    virtual int operator()(int a, int b) const {
        return a * b;
    }
};

int main() {
    Sum s;
    Function<int(int, int)> f(s);
    assert(f(1, 2) == 3);

    Id i1;
    Function<int(int)> f1(i1);
    assert(f1(1) == 1);

    auto s2 = Mul();
    Function<int(int, int)> f2(s2);
    assert(f2(4, 2) == 8);

    f.swap(f2);
    assert(f(5, 6) == 30);
    assert(f2(5, 6) == 11);

    return 0;
}