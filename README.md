# boloq

BDD/ZDD library to replesent binary functions using modern C++ futures

## Getting started

### Requirements

* C++11
* boost

### Install

    git clone https://github.com/utisam/boloq.git
    cd boloq/build
    cmake ..
    make -j4 && sudo make install

### Sample

```c++
#include <array>
#include <iostream>
#include <unordered_set>
#include <boloq.h>
using namespace std;
using namespace boloq;

int main() {
    boolean_function x('x'), y('y');

    array<boolean_function, 4> fns = {{
        ~x | ~y, ~(x & y),
        ~x & ~y, ~(x | y),
    }};

    // equivalence: O(1)
    cout << boolalpha << (fns[0] == fns[1]) << endl; // true

    // hashable
    unordered_set<boolean_function> fn_set(fns.begin(), fns.end());
    cout << fn_set.size() << endl; // 2
    return 0;
}
```

## Develop

    cmake -DCMAKE_BUILD_TYPE=Debug ..
    make -j4
    make test

## Documentation

    cd src
    doxygen
