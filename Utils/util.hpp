//this file mainly involves extra computations that will be neeeded for generating the cryptographic keys.

#ifndef UTILITIES
#define UTILITIES

#include "../headers.hpp"
using namespace std;

// Binary Exponetiation with Modulus
//compute a^b mod m in logn time , as we are using the binary representation of b to speed up compputation.
//refer : //cp-algorithms.com/algebra/binary-exp.html
ll modExp(ll a, ll b, ll mod) {
    ll prod = 1;
    a %= mod;
    while (b > 0) {
        if (b & 1) {
            prod = prod * a % mod;
        }
        a = a * a % mod;
        b >>= 1;
    }
    return prod;
}

// Prime Number Check
bool isPrime(long long n) {
    if (n <= 1) {
        return false;
    }
    ll k = 0, sqt = sqrt(n);
    for(ll i = 2; i <= sqt; i++) {
        if(n % i == 0) {
            return false;
        }
    }
    return true;
}

// Generate a Random Prime Number
ll getRandomPrime(ll mod) {
    ll x = rand() % mod;
    while(!isPrime(x)) {
        x = rand() % mod;
    }
    return x;
}

// Display Error
void err(string s) {
    perror(s.c_str());
    exit(1);
}

#endif
