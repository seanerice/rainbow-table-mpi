#include "SubstringIterator.h"

SubstringIterator::SubstringIterator(string str, int i_start, int i_stop) {
    initSubstringIterator(str, i_start, i_stop);
}

void SubstringIterator::initSubstringIterator(string str, int i_start, int i_stop) {
    this->str = str;
    start = i_start;
    stop = i_stop;
    from = start;
    to = 1 - 1;
    hasNextVal = true;
}

void SubstringIterator::initSubstringIterator(string str) {
    SubstringIterator::initSubstringIterator(str, 0, str.length() - 1);
}

string SubstringIterator::nextSubstring() {
    if (from < stop + 1) {
        if (to <= stop - from) {
            to++;
        } else {
            to = 1;
            from++;
        }
    }
    return str.substr(from, to);
}

bool SubstringIterator::hasNext() {
    return from < stop;
}