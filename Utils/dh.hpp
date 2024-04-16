#include "../headers.hpp"
#include "util.hpp"
using namespace std;

const ll MOD = 1e9 + 7;
const ll SMOD = 1e3 + 9;
ll G, P;

// For Server - Generator and Modulus
void generatePrimitiveKeys() {
    G = getRandomPrime(MOD);
    P = getRandomPrime(MOD);
}

// For Clients
void setPrimitiveKeys(ll g, ll p) {
    G = g;
    P = p;
}

// Client Private Key
ll getPrivateKey() {
    return (getRandomPrime(SMOD));
}

// Client Public Key
ll createPublicKey(ll x) {
    return modExp(G, x, P);
}

// Shared Secret - Diffie Hellman Key Exchange
ll createSecretKey(ll A, ll y) {
    return modExp(A, y, P);
}