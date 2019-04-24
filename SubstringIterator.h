#ifndef LIB_SUBSTRINGITERATOR_H_
#define LIB_SUBSTRINGITERATOR_H_

#include <iostream>
#include <ostream>
#include "stdint.h"
#include "stddef.h"
#include <string>

using namespace std;

/**
   ============================================================
   ----------------------------Info----------------------------
   ============================================================
   Generates unique substrings within optional indices

   ============================================================
   -------------------------Sample-Use-------------------------
   ============================================================

   SubstringIterator si("abcdefghij"); // a-j
   while (si.hasNext()) {
       cout << si.nextSubstring() << endl;
   }

   ============================================================
   -----------------------------or-----------------------------
   ============================================================

   SubstringIterator si("abcdefghij",3,7); // d-h
   while (si.hasNext()) {
       cout << si.nextSubstring() << endl;
   }

   ============================================================
   -----------------------------or-----------------------------
   ============================================================

   SubstringIterator si;
   si.initSubstringIterator("abcdefghij"); // a-j
   while (si.hasNext()) {
       cout << si.nextSubstring() << endl;
   }

   si.initSubstringIterator("abcdefghij",3,7); // d-h
   while (si.hasNext()) {
       cout << si.nextSubstring() << endl;
   }
   ============================================================
   ------------------------------------------------------------
   ============================================================
 */

class SubstringIterator {
public:

    /**
     * Initialize iterator, requires consecutive initSubstringIterator()
     */
    SubstringIterator() : SubstringIterator("", 1, 0) {};

    /**
     * Initialize iterator, generating substrings from str, ranging from i_start to i_stop
     * @param str string
     * @param i_start start index
     * @param i_stop stop index
     */
    SubstringIterator(string str, int i_start, int i_stop);

    /**
     * Initialize iterator, generating all substrings from str
     * @param str string
     */
    SubstringIterator(string str) : SubstringIterator(str, 0, str.length() - 1) {};

    /**
     * Reinitializes iterator, generating substrings from str, ranging from i_start to i_stop
     * @param str string
     * @param i_start start index
     * @param i_stop stop index
     */
    void initSubstringIterator(string str, int i_start, int i_stop);

    /**
     * Reinitialize iterator, generating all substrings from str
      * @param str string
     */
    void initSubstringIterator(string str);

    /**
     * @return next generated substring
     */
    string nextSubstring();

    /**
     * @return whether nextSubstring() can return another unique substring
     */
    bool hasNext();

private:
    string str;
    int start;
    int stop;
    int from;
    int to;
    bool hasNextVal;
};

#endif /* LIB_SUBSTRINGITERATOR_H_ */
