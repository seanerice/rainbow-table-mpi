#include <iostream>
#include <ostream>
#include <vector>
#include "source/prod.h"
#include "cryptopp/cryptlib.h"
#include "cryptopp/sha.h"

// returns a list of all possible salts.
vector<string> generateSalts(vector<char> charList, int saltLength) {
    // create all random alphanumeric combinations of letters of salt length
    vector<vector<char>> saltProds = prod(charList, saltLength);
    vector<string> salts = {};
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

// concatenates salts to strings according to provided pivot
vector<string> applySalts(vector<string> words, vector<string> salts, int saltLength, int pivot) {
	if (saltLength == 0) return words;
	vector<string> saltedPasswords = {};
	for (vector<string>::iterator itr = words.begin(); itr != words.end(); itr++) {
		for(vector<string>::iterator itr2 = salts.begin(); itr2 != salts.end(); itr2++) {
			string saltedPassword = itr2->substr(0,pivot) + *itr + itr2->substr(pivot,saltLength);
			saltedPasswords.push_back(saltedPassword);
		}
	}
    return saltedPasswords;
}

/*vector<CryptoPP::SHA256> applyHash(vector<string> salted) {
    vector<CryptoPP::SHA256> hashedPasswords = {};
	for (vector<string>::iterator itr = salted.begin(); itr != salted.end(); itr++) {
        CryptoPP::SHA256 hashedPassword;
        hashedPassword.Update((const byte *) itr->data(), itr->size());
    return;
}*/
