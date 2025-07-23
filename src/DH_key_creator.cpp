#include <cmath>
#include <mbedtls/dhm.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

mbedtls_dhm_context dhm;
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;

long long int p, g, a, Y, K;

long long int mod_exp(long long int base, long long int exp, long long int mod) {
    long long int result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

long long int setupDH()
{

    p = 23; // A prime number P is taken

    g = 9; // A primitive root for P, G is taken

    a = 4; // a is the chosen private key

    Y = mod_exp(g, a, p); // gets the generated key

    return Y;
}

long long int generateKey(long long int Y)
{
    K = mod_exp(Y, a, p); // Secret key for Alice

    return K;
}