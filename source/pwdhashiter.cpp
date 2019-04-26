#include "pwdhashiter.h"

PwdHashIter::PwdHashIter(vector <string> words, vector<char> alphanumeric, vector<int> pivots, vector<int> saltLengths, int min_pwd, int max_pwd) {
    initPwdHashIter(words, alphanumeric, pivots, saltLengths, min_pwd, max_pwd);
}

void PwdHashIter::initPwdHashIter(vector <string> words, vector<char> alphanumeric, vector<int> pivots, vector<int> saltLengths, int min_pwd, int max_pwd) {
    this->min_pwd = min_pwd;
    this->max_pwd = max_pwd;
    this->words = words;
    itr = this->words.begin();
    si.initSubstringIterator(*itr, min_pwd, max_pwd);
    this->alphanumeric = alphanumeric;
    this->pivots = pivots;
    this->saltLengths = saltLengths;
    salt = this->saltLengths.begin();
    saltVec = generateSalts(this->alphanumeric, *salt);
    currSalt = saltVec.begin();
    piv = this->pivots.begin();
    substring = si.nextSubstring();
}

hash_pwd PwdHashIter::nextPwdHash() {
    hash_pwd ret;
    string word = *itr;
    if ((unsigned) *piv > currSalt->length()) {
        currSalt++;
        piv = this->pivots.begin();
    }
    if (itr != words.end() && si.hasNext() && salt != saltLengths.end() && currSalt != saltVec.end() && piv != pivots.end()) {
        if ((unsigned) *piv <= currSalt->length()) {
            salted = applySalt(substring, *currSalt, *salt, *piv);
            hashed = applyHash(salted);
            ret.plain_pwd = substring;
            ret.salted_pwd = salted;
            ret.hashed_pwd = hashed;
        }
    }
    if (itr != words.end() && si.hasNext() && salt != saltLengths.end() && currSalt != saltVec.end() && piv != pivots.end()) {
        piv++;
    } else if (itr != words.end() && si.hasNext() && salt != saltLengths.end() && currSalt != saltVec.end()) {
        currSalt++;
        piv = this->pivots.begin();
    } else if (itr != words.end() && si.hasNext() && salt != saltLengths.end()) {
        salt++;
        saltVec = generateSalts(this->alphanumeric, *salt);
        currSalt = saltVec.begin();
        piv = this->pivots.begin();
    } else if (itr != words.end() && si.hasNext()) {
        substring = si.nextSubstring();
        salt = saltLengths.begin();
        saltVec = generateSalts(this->alphanumeric, *salt);
        currSalt = saltVec.begin();
        piv = this->pivots.begin();
    } else if (itr != words.end()) {
        salt = saltLengths.begin();
        saltVec = generateSalts(this->alphanumeric, *salt);
        currSalt = saltVec.begin();
        piv = this->pivots.begin();
        itr++;
    } else {
        cout << "Something happened" << endl;
    }
    return ret;
}

bool PwdHashIter::hasNext() {
    return itr != words.end();
}