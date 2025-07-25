#include "SecureOTPGenerator.hpp"

SecureOTPGenerator::SecureOTPGenerator(uint32_t seed, const String &storageNamespace)
    : seed(seed), storageKey(storageNamespace)
{
    loadCounter();
}
SecureOTPGenerator::SecureOTPGenerator(const String &storageNamespace) : storageKey(storageNamespace)
{
    loadCounter();
}

void SecureOTPGenerator::loadCounter()
{
    Preferences prefs;
    prefs.begin(storageKey.c_str(), true);
    counter = prefs.getUInt("ctr", 0);
    prefs.end();
}

void SecureOTPGenerator::saveCounter()
{
    Preferences prefs;
    prefs.begin(storageKey.c_str(), false);
    prefs.putUInt("ctr", counter);
    prefs.end();
}

uint32_t SecureOTPGenerator::generateOTP()
{
    // TODO otp using SHA256 or other cryptographic secured PRNG generators.
    uint32_t otp = seed ^ (counter * 2654435761UL); // Moltiplicazione con numero di Knuth non ideale :(
    counter++;
    saveCounter();
    return otp;
}

uint32_t SecureOTPGenerator::peekOTP() const
{
    return seed ^ (counter * 2654435761UL);
}

void SecureOTPGenerator::reset(uint32_t newCounter = 0)
{
    counter = newCounter;
    saveCounter();
}
