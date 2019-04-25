#include <iostream>
#include <ostream>
#include <vector>
#include "prod.h"
#include "picosha2.h"

vector<string> generateSalts(vector<char> charList, int saltLength);
string applySalt(string word, string salt, int saltLength, int pivot);
string applyHash(string salted);
