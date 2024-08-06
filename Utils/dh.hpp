#include "../headers.hpp"
#include "util.hpp"
using namespace std;

const ll MOD = 1e9 + 7;
const ll SMOD = 1e3 + 9;
ll G, P;

// generates the generator g and modulus p.
//used by the server.
void generatePrimitiveKeys() {
    G = getRandomPrime(MOD);
    P = getRandomPrime(MOD);
}

// For Clients
void setPrimitiveKeys(ll g, ll p) {
    G = g;
    P = p;
}

// Client Private Key-b.
ll getPrivateKey() {
    return (getRandomPrime(SMOD));
}

// Client Public Key
//creates g^x mod p.
ll createPublicKey(ll x) {
    return modExp(G, x, P);
}

// Shared Secret - Diffie Hellman Key Exchange
//creates A^b mod p.
ll createSecretKey(ll A, ll y) {
    return modExp(A, y, P);
}
