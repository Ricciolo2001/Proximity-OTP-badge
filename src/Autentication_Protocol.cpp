#include "Autentication_Protocol.hpp"

// Used to create the message that a clients want to send to the server
char *createAutenticationMessage(uint32_t challengeVal, char *username)
{
    char challengeStr[12];
    sprintf(challengeStr, "%lu", (unsigned long)challengeVal);

    size_t totalLen = strlen(username) + 1 + strlen(challengeStr) + 1;
    char *message = (char *)malloc(totalLen);
    if (!message)
        return nullptr;

    sprintf(message, "%s|%lu", username, (unsigned long)challengeVal);

    return message; // Da liberare dal chiamante
}

// Not used yet by the server, you can find a partial implementation from line 72 of the light_device.cpp file
Auth_Message processAutenticationMessage(char *username, uint8_t *msg)
{
    Auth_Message auth;

    // Parsing: expected format "username|challenge"
    char *sep = strchr((char *)msg, '|');
    if (!sep)
    {
#ifdef ARDUINO
        Serial.println("Errore: formato messaggio non valido.");
#else
        printf("Errore: formato messaggio non valido.\n");
#endif
        strcpy(auth.username, "");
        auth.challenge = -1;
        return auth;
    }

    size_t unameLen = sep - (char *)msg;
    if (unameLen >= sizeof(auth.username))
        unameLen = sizeof(auth.username) - 1;

    strncpy(auth.username, (char *)msg, unameLen);
    auth.username[unameLen] = '\0';

    auth.challenge = strtoul(sep + 1, (char **)NULL, 10);

    if (strcmp(auth.username, username) != 0)
    {
#ifdef ARDUINO
        Serial.println("Username mismatch!");
#else
        printf("Username mismatch!\n");
#endif
        auth.challenge = -1;
    }

    return auth; // In case of success, challenge >= 0
}
