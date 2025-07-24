#include <stdint.h>
#include <string.h>
#include <XTEA-Cipher.h>

#ifndef CFB_CIPHER_HPP
#define CFB_CIPHER_HPP

#define XTEA_ROUNDS 32

class CfbCipher
{
private:
    xteaCipherCtx_t ctx;
    uint8_t iv[XTEA_IV_SIZE];

public:
    CfbCipher(const uint8_t *key, const uint8_t *init_iv);
    void resetIv(const uint8_t *newIv);
    void encrypt(uint8_t *data, size_t len);
    void decrypt(uint8_t *data, size_t len);

private:
    void process(uint8_t *data, size_t len);
};

#endif // CFB_CIPHER_HPP
