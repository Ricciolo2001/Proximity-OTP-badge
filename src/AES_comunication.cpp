#include "AES_comunication.hpp"

CfbCipher::CfbCipher(const uint8_t *key, const uint8_t *init_iv)
{
    memcpy(iv, init_iv, XTEA_IV_SIZE);
    xteaInit(&ctx, key, iv, XTEA_ROUNDS);
}

void CfbCipher::resetIv(const uint8_t *newIv)
{
    memcpy(iv, newIv, XTEA_IV_SIZE);
    xteaSetIv(&ctx, iv);
}

void CfbCipher::encrypt(uint8_t *data, size_t len)
{
    xteaSetOperation(&ctx.base, xteaEncrypt);
    process(data, len);
}

void CfbCipher::decrypt(uint8_t *data, size_t len)
{
    xteaSetOperation(&ctx.base, xteaDecrypt);
    process(data, len);
}

void CfbCipher::process(uint8_t *data, size_t len)
{
    uint8_t block[XTEA_BLOCK_SIZE];
    for (size_t i = 0; i < len; i += XTEA_BLOCK_SIZE)
    {
        memcpy(block, data + i, XTEA_BLOCK_SIZE);
        xteaCfbBlock(&ctx, block);
        memcpy(data + i, block, XTEA_BLOCK_SIZE);
    }
}
