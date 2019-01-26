#include <iostream>
#include <string>
#include <type_traits>
#include <memory>
#include "Function.h"

using namespace std;

class Sum {
public:
    virtual int operator()(int a, int b) const {
        return a + b;
    }
};

class Mul {
public:
    virtual int operator()(int a, int b) const {
        return a * b;
    }
};

class Concat {
public:
    virtual string operator()(const string &a, const string &b) const {
        return a + b + "KEK";
    }
};

int main() {
    auto s = Sum();
    Function<int(int, int)> f(s);
    assert(f(4, 2) == 6);

    auto s2 = Mul();
    Function<int(int, int)> f2(s2);
    assert(f2(4, 2) == 8);

    f.swap(f2);
    assert(f(4, 2) == 8);
    assert(f2(4, 2) == 6);

    Concat c;
    Function<string(const string&, const string&)> big(c);
    assert(big("a", "b") == "abKEK");

    Function<int(int, int)> fn;
    assert(!fn);
    assert(f);

    return 0;
}