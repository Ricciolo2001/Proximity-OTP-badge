#ifndef DIFFIE_HELLMAN_H
#define DIFFIE_HELLMAN_H

#include <stdint.h> // Per uint32_t

typedef struct __attribute__((packed))
{
    uint32_t p;
    uint32_t g;
    uint32_t publicKey;
} DH_Message;

typedef struct __attribute__((packed))
{
    char username[16]; // nome utente
    uint32_t otp; // otp XOR chiave condivisa
} Auth_Message;

class DiffieHellman
{
private:
    long long int p, g;       // p, g
    long long int privateKey; // a
    long long int publicKey;  // g^a mod p
    long long int sharedKey;  // (Y^a mod p)
    

    // Modular exponentiation (base^exp mod mod)
    long long int mod_exp(long long int base, long long int exp, long long int mod);

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

    // Communication
    void createAutenticationMessage(Auth_Message msg);
    Auth_Message recieveAutenticationMessage();
};

#endif // DIFFIE_HELLMAN_H
