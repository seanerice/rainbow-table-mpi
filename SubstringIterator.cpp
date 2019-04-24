#include "SubstringIterator.h"

SubstringIterator::SubstringIterator(string str, int min_length, int max_length) {
    initSubstringIterator(str, min_length, max_length);
}

void SubstringIterator::initSubstringIterator(string str, int min_length, int max_length) {
    this->str = str;
    this->min_length = min_length;
    this->max_length = max_length;
    stop = str.length() - min_length;
    from = 0;
    to = min_length - 1; // will be incremented in nextSubstring()
    hasNextVal = true;
}

void SubstringIterator::initSubstringIterator(string str) {
    SubstringIterator::initSubstringIterator(str, 1, str.length());
}

string SubstringIterator::nextSubstring() {
    if (from < stop) {
        if (to < max_length) {
            to++;
        } else {
            to = min_length;
            from++;
        }
    }
    return str.substr(from, to);
}

bool SubstringIterator::hasNext() {
    return from < stop;
}