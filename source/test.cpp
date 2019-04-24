#include "prod.h"
#include <iostream>
#include <ostream>
#include "stdint.h"
#include "stddef.h"
#include <string>
#include <vector>
#include <tuple>
#include <array>

using namespace std;

template <typename T, typename A>
void print_vec(vector<T, A> a) {
    cout << "[";
    for (typename vector<T>::iterator itr = a.begin(); itr != a.end(); itr++) {
        cout << *itr << " ";
    }
    cout << "]";
}

int main() {
    /* Test substring iterator */
    // TODO

    /* Test cartesian product function */
    vector<char> alphanumeric = {'a', 'b', 'c', '1', '2', '3'};
    print_vec(alphanumeric);

    vector<vector<char>> prods = prod(alphanumeric, 2);
    for (vector<vector<char>>::iterator itr = prods.begin(); itr != prods.end(); itr++) {
        print_vec(*itr);
        cout << endl;
    }
    return 0;
}


