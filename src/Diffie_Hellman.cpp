#include <Diffie_Hellman.hpp>
#include <random>
#include <ctime>

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

// test di primalità di Fermat (non l'ho ovviamente scritto io...)
bool DiffieHellman::is_prime(uint64_t n, int iterations = 5)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint64_t> dist(2, n - 2);
    for (int i = 0; i < iterations; ++i)
    {
        uint64_t a = dist(rng);
        if (mod_exp(a, n - 1, n) != 1)
            return false;
    }
    return true;
}

uint64_t DiffieHellman::generate_prime(uint64_t min = 1000, uint64_t max = 50000)
{
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint64_t> dist(min, max);

    int attempts = 0;
    while (attempts++ < 100000)
    {
        uint64_t candidate = dist(rng) | 1; // makes it not even
        if (is_prime(candidate))
            return candidate;
    }
    return 1; // Dovrebbe essere un errore...ma preferisco infastidire i matematici
}

uint64_t DiffieHellman::find_generator(uint64_t p)
{
    for (uint64_t g = 2; g < p; ++g)
    {
        if (mod_exp(g, 2, p) != 1 && mod_exp(g, (p - 1) / 2, p) != 1)
            return g;
    }
    return 2; // fallback
}

// Method to generate the shared key
long long int DiffieHellman::computeSharedKey(long long int otherPublicKey)
{
    sharedKey = mod_exp(otherPublicKey, privateKey, p);
    return sharedKey;
}

// Builder (fixed parameters)
DiffieHellman::DiffieHellman()
{
    p = 23;
    g = 9;
    privateKey = 4;

    // p = generate_prime();
    // g = find_generator(p);
    // privateKey = esp_random(); // TRNG
    // !Disabilitato per test, mi piacciono i valori prevedibiliii
    publicKey = mod_exp(g, privateKey, p);
}

// Builder with parameters
DiffieHellman::DiffieHellman(long long int prime, long long int generator, long long int otherPublicKey)
{
    p = prime;
    g = generator;
    // privateKey = esp_random(); // TRNG
    // !Disabilitato per test, mi piacciono i valori prevedibiliii
    privateKey = 4;

    publicKey = mod_exp(g, privateKey, p);
    sharedKey = computeSharedKey(otherPublicKey);
}

// Getter
long long int DiffieHellman::getP() const { return p; }
long long int DiffieHellman::getG() const { return g; }
long long int DiffieHellman::getPublicKey() const { return publicKey; }
long long int DiffieHellman::getSharedKey() const { return sharedKey; }