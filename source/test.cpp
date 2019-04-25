#include "prod.h"
#include <iostream>
#include <ostream>
#include "stdint.h"
#include "stddef.h"
#include <string>
#include <vector>
#include <tuple>
#include <array>
#include "SubstringIterator.h"
#include "salty.h"

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

    vector<vector<char>> prods = prod(alphanumeric, 2);

    /* Test substring iterator */
    vector<string> list = {"fuck", "this", "bullshit"}; // to be replaced by args
    vector<int> pivots {0, 1, 2, 3}; // to be replaced by args
    vector<int> saltLengths {0, 1, 2, 3}; // to be replaced by args
    vector<string> hashed = {}; 

    /* Test loop */
    SubstringIterator si;
    vector<string> words = {"fuck", "shit", "bitch", "young", "sheck", "wes", "and", "im", "getting", "really", "rich" };
    for (vector<string>::iterator itr = words.begin(); itr != words.end(); itr++) {
        
        si.initSubstringIterator(*itr, 2, 7);
        while (si.hasNext()) {
            string pt_password = si.nextSubstring();
            // Salt len
            for(vector<int>::iterator salt = saltLengths.begin(); salt != saltLengths.end(); salt++) {
                vector<string> saltVec = generateSalts(alphanumeric, *salt);
                // salts
                for(vector<string>::iterator currSalt = saltVec.begin(); currSalt != saltVec.end(); currSalt++) {
                    // pivots
                    for(vector<int>::iterator piv = pivots.begin(); piv != pivots.end(); piv++) {
                        if((unsigned)*piv <= currSalt->length()) {
                            string salted = applySalt(pt_password, *currSalt, *salt, *piv);
                            string hashed = applyHash(salted); 
                            cout << hashed << " " << pt_password << " " << salted << " " << *currSalt << '\n';

                            // Check hash match against db
                        }
                    }
                }
            }
        }   
    }



    return 0;
}


