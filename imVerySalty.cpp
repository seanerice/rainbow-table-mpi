#include <iostream>
#include <string>
#include <vector>
#include "salty.h"
#include "stdint.h"
#include "stddef.h"

using namespace std;

int main() {
    /* Test substring iterator */
    vector<char> alphanumeric = {'a', 'b', 'c', '1', '2', '3'}; // to be replaced by args
    vector<string> list = {"fuck", "this", "bullshit"}; // to be replaced by args
    vector<int> pivots {0, 1, 2, 3}; // to be replaced by args
    vector<int> saltLengths {0, 1, 2, 3}; // to be replaced by args
    vector<string> hashed = {}; 

    for(vector<int>::iterator salt = saltLengths.begin(); salt != saltLengths.end(); salt++) {
        vector<string> saltVec = generateSalts(alphanumeric, *salt);
        for(vector<string>::iterator currSalt = saltVec.begin(); currSalt != saltVec.end(); currSalt++) {
            for(vector<int>::iterator piv = pivots.begin(); piv != pivots.end(); piv++) {
                if((unsigned)*piv <= currSalt->length()) {
                    string salted = applySalt("REPLACE ME BITCH", *currSalt, *salt, *piv);
                    string hashed = applyHash(salted); 
                    cout << hashed << '\n';
                }
            }
        }
    }

    cout << "[";
    for (vector<string>::iterator itr = hashed.begin(); itr != hashed.end(); itr++) {
        cout << *itr << " ";
    }
    cout << "]";
    return 0;
}

