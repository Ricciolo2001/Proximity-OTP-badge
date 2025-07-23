#ifndef DIFFIE_HELLMAN_H
#define DIFFIE_HELLMAN_H

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
    DiffieHellman(long long int prime, long long int generator);

    long long int computeSharedKey(long long int otherPublicKey);

    // Getter
    long long int getP() const;
    long long int getG() const;
    long long int getPublicKey() const;
    long long int getSharedKey() const;
};

#endif // DIFFIE_HELLMAN_H