// ESP32_B_Scanner_Display.ino
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <map>
#include "Diffie_Hellman.hpp"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

#define BUTTON_PIN 46

typedef struct __attribute__((packed))
{
    uint32_t p;
    uint32_t g;
    uint32_t publicKey;
} DH_Message;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Scanned devices

struct DeviceInfo
{
    int rssi;
    float distance;
    unsigned long lastSeen;
};

std::map<String, DeviceInfo> devices;

#define DEVICE_TIMEOUT 3000 // Device freshness

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Display

TwoWire I2C_OLED = TwoWire(0);

#define I2C_SDA 8
#define I2C_SCL 19

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2C_OLED, OLED_RESET);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// BLE

BLEScan *pBLEScan;

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_WRITE "abcd1234-1234-1234-1234-abcdefabcdef"
#define CHARACTERISTIC_NOTIFY "abcd5678-1234-1234-1234-abcdefabcdef"

float estimateDistance(int rssi)
{
    int txPower = -60; // valore tipico, puoi tararlo
    return pow(10.0, ((float)txPower - rssi) / (10 * 2));
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) override
    {
        String name = String(advertisedDevice.getName().c_str());
        if (name.length() == 0)
            return; // ignora se senza nome

        int rssi = advertisedDevice.getRSSI();
        float dist = estimateDistance(rssi);

        devices[name] = {
            .rssi = rssi,
            .distance = dist,
            .lastSeen = millis()};
    }
};

uint32_t connectAndSendDHMessage(const String &deviceName, DiffieHellman &d, const DH_Message &msg)
{
    Serial.printf("Cerco di connettermi a %s...\n", deviceName.c_str());

    BLEScanResults results = pBLEScan->getResults();
    for (int i = 0; i < results.getCount(); ++i)
    {
        BLEAdvertisedDevice dev = results.getDevice(i);
        if (String(dev.getName().c_str()) == deviceName)
        {
            BLEClient *pClient = BLEDevice::createClient();

            if (!pClient->connect(&dev))
            {
                Serial.println("Connessione fallita.");
                return 0;
            }

            BLERemoteService *pService = pClient->getService(SERVICE_UUID);
            if (!pService)
            {
                Serial.println("Servizio non trovato.");
                pClient->disconnect();
                return 0;
            }

            // Scrittura messaggio DH
            BLERemoteCharacteristic *pWriteChar = pService->getCharacteristic(CHARACTERISTIC_WRITE);
            BLERemoteCharacteristic *pNotifyChar = pService->getCharacteristic(CHARACTERISTIC_NOTIFY);

            if (!pWriteChar || !pNotifyChar)
            {
                Serial.println("Caratteristica non trovata.");
                pClient->disconnect();
                return 0;
            }

            // Invia messaggio DH
            pWriteChar->writeValue((uint8_t *)&msg, sizeof(DH_Message), false);
            Serial.println("Messaggio DH inviato. Attendo risposta...");

            // Leggi risposta Y
            std::string value = pNotifyChar->readValue();
            if (value.length() != sizeof(uint32_t))
            {
                Serial.println("Risposta DH non valida.");
                pClient->disconnect();
                return 0;
            }

            uint32_t receivedY;
            memcpy(&receivedY, value.data(), sizeof(uint32_t));

            // Calcolo chiave condivisa
            uint32_t sharedKey = d.computeSharedKey(receivedY);
            Serial.printf("Y ricevuto: %lu, Chiave condivisa: %lu\n", receivedY, sharedKey);

            pClient->disconnect();
            return receivedY;
        }
    }

    Serial.println("Dispositivo non trovato nella scansione.");
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Init

void setup()
{

    Serial.begin(115200);

    I2C_OLED.begin(I2C_SDA, I2C_SCL);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("Display non trovato"));
        while (true)
            ;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Inizializzo BLE...");
    display.display();

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks(), false);
    pBLEScan->setActiveScan(true);
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    pinMode(BUTTON_PIN, INPUT_PULLUP); // Button for pairing mode
}

void loop()
{
    pBLEScan->start(1, false); // scansione per 1 secondo

    unsigned long time_now = millis();
    auto it = devices.begin();
    while (it != devices.end())
    {
        if (time_now - it->second.lastSeen > DEVICE_TIMEOUT)
        {
            it = devices.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Prints devices based on transmitting power
    display.clearDisplay();
    display.setCursor(0, 0);

    int row = 0;

    std::vector<std::pair<String, DeviceInfo>> sortedDevices(devices.begin(), devices.end());

    std::sort(sortedDevices.begin(), sortedDevices.end(),
              [](const std::pair<String, DeviceInfo> &a, const std::pair<String, DeviceInfo> &b)
              {
                  return a.second.rssi > b.second.rssi;
              });

    for (auto it = sortedDevices.begin(); it != sortedDevices.end(); ++it)
    {
        String name = it->first;
        const DeviceInfo &info = it->second;
        String shortName = name.length() > 15 ? name.substring(0, 12) + "..." : name;

        display.printf("%s %.1fm\n", shortName.c_str(), info.distance);
        row++;
        if (row >= 3)
            break; // max 3 righe per display 128x32
    }

    display.display();
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        if (!sortedDevices.empty())
        {
            DiffieHellman d;
            DH_Message msg;
            msg.p = d.getP();
            msg.g = d.getG();
            msg.publicKey = d.getPublicKey();

            String targetName = sortedDevices.front().first;
            uint32_t otherY = connectAndSendDHMessage(targetName, d, msg);
            d.computeSharedKey(otherY);
            Serial.println("Ciave calcolata: " + String(d.getSharedKey()));
        }
        else
        {
            Serial.println("Nessun dispositivo BLE disponibile.");
        }

        delay(1000); // debounce
    }
}
