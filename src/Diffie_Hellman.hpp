#ifndef DIFFIE_HELLMAN_H
#define DIFFIE_HELLMAN_H

#include <stdint.h>
#include <Arduino.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <XTEA-Cipher.h>

typedef struct __attribute__((packed))
{
    uint32_t p;
    uint32_t g;
    uint32_t publicKey;
} DH_Message;

class DiffieHellman
{
private:
    long long int p, g;       // p, g
    long long int privateKey; // a
    long long int publicKey;  // g^a mod p
    long long int sharedKey;  // (Y^a mod p)

    // Modular exponentiation (base^exp mod mod)
    long long int mod_exp(long long int base, long long int exp, long long int mod);

    bool is_prime(uint64_t n, int iterations);

    uint64_t generate_prime(uint64_t min, uint64_t max);

    uint64_t find_generator(uint64_t p);

public:
    // Builders
    DiffieHellman();
    DiffieHellman(long long int prime, long long int generator, long long int otherPublicKey);
    long long int computeSharedKey(long long int otherPublicKey);

    // Getter
    long long int getP() const;
    long long int getG() const;
    long long int getPublicKey() const;
    long long int getSharedKey() const;
};

#endif // DIFFIE_HELLMAN_H
