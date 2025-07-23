#include <Diffie_Hellman.hpp>
#include <string.h>

// Modular exponentiation
long long int DiffieHellman::mod_exp(long long int base, long long int exp, long long int mod)
{
    long long int result = 1;
    base = base % mod;
    while (exp > 0)
    {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

// Method to generate the shared key
long long int DiffieHellman::computeSharedKey(long long int otherPublicKey)
{
    sharedKey = mod_exp(otherPublicKey, privateKey, p);
    return sharedKey;
}

// Builder (fixed parameters)
DiffieHellman::DiffieHellman()
{ // TODO random values
    p = 23;
    g = 9;
    privateKey = 4;
    publicKey = mod_exp(g, privateKey, p);
}

// Builder with parameters
DiffieHellman::DiffieHellman(long long int prime, long long int generator, long long int otherPublicKey)
{
    p = prime;
    g = generator;
    privateKey = 4; // TODO random
    publicKey = mod_exp(g, privateKey, p);
    sharedKey = computeSharedKey(otherPublicKey);
}

// Getter
long long int DiffieHellman::getP() const { return p; }
long long int DiffieHellman::getG() const { return g; }
long long int DiffieHellman::getPublicKey() const { return publicKey; }
long long int DiffieHellman::getSharedKey() const { return sharedKey; }

// Communication
void createAutenticationMessage(const String &username)
{
    
}

Auth_Message recieveAutenticationMessage()
{
}