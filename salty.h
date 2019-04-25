#include <iostream>
#include <ostream>
#include <vector>
#include "source/prod.h"
#include "picosha2.h"

class salty {
    public:
    vector<string> generateSalts(vector<char> charList, int saltLength);
    vector<string> applySalts(vector<string> words, vector<string> salts, int saltLength, int pivot);
    string applyHash(string salted); 
};
