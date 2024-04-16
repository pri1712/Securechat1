#include "../headers.hpp"
using namespace std;

// Key Scheduling Algorithm
void key_scheduling(array<int, 256> &S, const string &key) {
    for (int i = 0; i < 256; i++) {
        S[i] = i;
    }
    for (int i = 0, j = 0; i < 256; i++) {
        int temp = (int)key[i % key.length()];
        j = (j + S[i] + temp) % 256;
        swap(S[i], S[j]);
    }
}

// Pseudo-Random Generation Algorithm
int pgra(array<int, 256> &S, int &i, int &j) {
    i = (i + 1) % 256;
    j = (j + S[i]) % 256;
    swap(S[i], S[j]);
    int k = (S[i] + S[j]) % 256;
    return S[k];
}

// Encrypt or Decrypt
void encrypt(string &data, const string &key) {
    if (key.length() > 256) {
        cout << "Key size shouldn't be more than 256" << endl;
        exit(1);
    }
    array<int, 256> S;
    key_scheduling(S, key);
    int i = 0, j = 0;
    for (char &c : data) {
        c ^= pgra(S, i, j);
    }
}