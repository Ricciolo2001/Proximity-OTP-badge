#ifndef AUTHENTICATION_HPP
#define AUTHENTICATION_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <esp_random.h>

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <cstdio>
#endif

typedef struct __attribute__((packed))
{
    char username[16];
    int32_t challenge;
} Auth_Message;
char *createAutenticationMessage(uint32_t otp, char *username);

Auth_Message processAutenticationMessage(uint32_t otp, char *username, uint8_t *msg);

#endif // AUTHENTICATION_HPP
