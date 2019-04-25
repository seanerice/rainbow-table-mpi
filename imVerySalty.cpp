#include <iostream>
#include <string>
#include <vector>
#include "saltIter.h"
#include "stdint.h"
#include "stddef.h"

using namespace std;

int main() {
    /* Test substring iterator */
    /*vector<char> alphanumeric = {'a', 'b', 'c', '1', '2', '3'}; // to be replaced by args
    vector<string> list = {"fuck", "this", "bullshit"}; // to be replaced by args
    vector<int> pivots {0, 1, 2, 3}; // to be replaced by args
    vector<int> saltLengths {0, 1, 2, 3}; // to be replaced by args
    for (vector<int>::iterator itr = saltLengths.begin(); itr != saltLengths.end(); itr++) {
        vector<string> salts = generateSalts(alphanumeric, *itr);
        for(vector<int>::iterator itr2 = pivots.begin(); itr2 != pivots.end(); itr2++) {
            applySalts(list, salts, *itr, *itr2);
        }
    }*/
    vector<char> alphanumeric = {'a', 'b', 'c', '1', '2', '3'};
    generateSalts(alphanumeric, 4);
    return 0;
}

