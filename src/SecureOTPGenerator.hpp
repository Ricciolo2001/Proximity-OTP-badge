#ifndef SECURE_OTP_GENERATOR_H
#define SECURE_OTP_GENERATOR_H

#include <Arduino.h>
#include <Preferences.h>

class SecureOTPGenerator
{
private:
    uint32_t seed;     // Initial key generator
    uint32_t counter;  // Status
    String storageKey; // Name of the stored key

    // Load adnd saves status from NVS (internal persistant memory)
    // In a future implementation i will use a file saved on memory so it will be updated more easly

    void loadCounter();
    void saveCounter();

public:
    SecureOTPGenerator(uint32_t seed, const String &storageNamespace);
    SecureOTPGenerator(const String &storageNamespace);

    uint32_t generateOTP();
    uint32_t peekOTP() const;         // Current OTP code
    void reset(uint32_t counter = 0); // Set Counter to zero or counter val
    int getCounter();
};

#endif // SECURE_OTP_GENERATOR_H
