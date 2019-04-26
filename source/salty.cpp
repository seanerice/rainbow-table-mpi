#include "salty.h"

// returns a list of all possible salts.
vector<string> generateSalts(vector<char> charList, int saltLength) {
    // create all random alphanumeric combinations of letters of salt length
    vector<vector<char>> saltProds = prod(charList, saltLength);
    vector<string> salts = {};

    if (saltLength <= 0) {
        salts.push_back("");
        return salts;
    }

	// convert cartesian set of characters into strings
    for (vector<vector<char>>::iterator itr = saltProds.begin(); itr != saltProds.end(); itr++) {
		string salt = "";
		for(vector<char>::iterator itr2 = itr->begin(); itr2 != itr->end(); itr2++) {
			salt += *itr2;
		}
		salts.push_back(salt);
	}
    return salts;
}

// concatenates  salts to strings according to provided pivot
string applySalt(string word, string salt, int saltLength, int pivot) {
    string saltedPassword = salt.substr(0,pivot) + word + salt.substr(pivot,salt.length());
    return saltedPassword;
}

string applyHash(string salted) {
    string hashedPassword = picosha2::hash256_hex_string(salted);
    return hashedPassword;
}
