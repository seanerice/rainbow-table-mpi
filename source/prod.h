#ifndef PROD_H
#define PROD_H

#include <vector>

using namespace std;

template<typename T, typename A>
vector<vector<T>> prod(vector<T, A> &a, vector<T, A> &b) {
    vector<vector<T>> out;
    for (typename vector<T>::iterator itra = a.begin(); itra != a.end(); itra++) {
        for (typename vector<T>::iterator itrb = b.begin(); itrb != b.end(); itrb++) {
            vector<T> tmp;
            tmp.push_back(*itra);
            tmp.push_back(*itrb);
            out.push_back(tmp);
        }
    }
    return out;
}

template<typename T, typename A>
vector<vector<T>> prod(vector<T, A> c, int n) {
    // Initialize out
    vector<vector<T>> out;
    for (typename vector<T>::iterator itr = c.begin(); itr != c.end(); itr++) {
        vector<T> tmp;
        tmp.push_back(*itr);
        out.push_back(tmp);
    }

    for (int i = 1; i < n; i++) {
        vector<vector<T>> next;

        for (typename vector<vector<T>>::iterator itra = out.begin(); itra != out.end(); itra++) {
            for (typename vector<T>::iterator itrb = c.begin(); itrb != c.end(); itrb++) {
                vector<T> res = *itra;
                res.push_back(*itrb);
                next.push_back(res);
            }
        }

        out = next;
    }
    return out;
}


#endif /* PROD_H */
