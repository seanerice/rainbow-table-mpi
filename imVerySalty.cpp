#include <iostream>
#include <ostream>
#include "stdint.h"
#include "stddef.h"
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include "saltIter.h"

using namespace std;

int main() {
    /* Test substring iterator */
    // TODO

    /* Test cartesian product function */
    vector<char> alphanumeric = {'a', 'b', 'c', '1', '2', '3'};
    generateSalts(alphanumeric, 4);
    return 0;
}

