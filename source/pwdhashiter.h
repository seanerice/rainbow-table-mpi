#ifndef LIB_PWDHASHITER_H_
#define LIB_PWDHASHITER_H_

#include <iostream>
#include <ostream>
#include "stdint.h"
#include "stddef.h"
#include <string>
#include <iterator>
#include <vector>
#include "salty.h"
#include "SubstringIterator.h"

using namespace std;

/**
   ============================================================
   ----------------------------Info----------------------------
   ============================================================
   Generates unique salted hashes using specified arguments

   ============================================================
   -------------------------Sample-Use-------------------------
   ============================================================

   vector<string> words = {
       "addition", "reflective", "torpid"
   };

   vector<char> alphanumeric = {
          'a', 'b', 'c', 'd', 'e', 'f',
          'g', 'h', 'i', 'j', 'k', 'l',
          'm', 'n', 'o', 'p', 'q', 'r',
          's', 't', 'u', 'v', 'w', 'x',
          'y', 'z', '0', '1', '2', '3',
          '4', '5', '6', '7', '8', '9'
   };

   vector<int> pivots{0, 1, 2, 3};
   vector<int> saltLengths{0, 1, 2, 3};

   PwdHashIter phi(words, alphanumeric, pivots, saltLengths, 2, 7);
    while (phi.hasNext()) {
        hash_pwd h = phi.nextPwdHash();
        cout << "Hash: " << h.hashed_pwd << " Plain: " << h.plain_pwd << " Salted: " << h.salted_pwd << endl;
    }

   ============================================================
   ------------------------------------------------------------
   ============================================================
 */

typedef struct hash_pwd {
    string plain_pwd;
    string salted_pwd;
    string hashed_pwd;
} hash_pwd;

class PwdHashIter {
public:

    /**
     * Initialize iterator
     */
    PwdHashIter(vector <string> words, vector<char> alphanumeric, vector<int> pivots, vector<int> saltLengths, int min_pwd, int max_pwd);

    /**
     * Reinitializes iterator
     */
    void initPwdHashIter(vector <string> words, vector<char> alphanumeric, vector<int> pivots, vector<int> saltLengths, int min_pwd, int max_pwd);


    /**
   * @return next generated hash
   */
    hash_pwd nextPwdHash();

    /**
     * @return whether nextPwdHash() can return another unique password hash
     */
    bool hasNext();

private:
    SubstringIterator si;
    int min_pwd;
    int max_pwd;
    vector <string> words;
    vector<char> alphanumeric;
    vector<int> pivots;
    vector<int> saltLengths;
    vector<string>::iterator itr;
    vector<int>::iterator salt;
    vector<string>::iterator currSalt;
    vector<int>::iterator piv;
    vector <string> saltVec;
    string substring;
    string salted;
    string hashed;
};

#endif /* LIB_PWDHASHITER_H_ */