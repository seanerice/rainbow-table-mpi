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
   Generates unique substrings within specified length [optional]

   ============================================================
   -------------------------Sample-Use-------------------------
   ============================================================

   SubstringIterator si("abcdefghij");
   while (si.hasNext()) {
       cout << si.nextSubstring() << endl;
   }

   ============================================================
   -----------------------------or-----------------------------
   ============================================================

   SubstringIterator si("abcdefghij",3,7);
   while (si.hasNext()) {
       cout << si.nextSubstring() << endl;
   }

   ============================================================
   -----------------------------or-----------------------------
   ============================================================

   SubstringIterator si;
   si.initSubstringIterator("abcdefghij");
   while (si.hasNext()) {
       cout << si.nextSubstring() << endl;
   }

   si.initSubstringIterator("abcdefghij",3,7);
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
     * Initialize iterator, generating substrings from str, ranging from min_length to max_length
     * @param str string
     * @param min_length start index
     * @param max_length stop index
     */
    SubstringIterator(string str, int min_length, int max_length);

    /**
     * Initialize iterator, generating all substrings from str
     * @param str string
     */
    SubstringIterator(string str) : SubstringIterator(str, 1, str.length()) {};

    /**
     * Reinitializes iterator, generating substrings from str, within min_length - max_length
     * @param str string
     * @param min_length
     * @param max_length
     */
    void initSubstringIterator(string str, int min_length, int max_length);

    /**
     * Reinitialize iterator, generating all substrings from str
      * @param str string
     */
    void initSubstringIterator(string str);

    /**
   * @return next generated substring within the length
   */
    string nextSubstring();

    /**
     * @return whether nextSubstring() can return another unique substring
     */
    bool hasNext();

private:
    string str;
    int stop;
    int from;
    int to;
    int min_length;
    int max_length;
    bool hasNextVal;
};

#endif /* LIB_SUBSTRINGITERATOR_H_ */
