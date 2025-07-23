#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "Diffie_Hellman.hpp"

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_WRITE "abcd1234-1234-1234-1234-abcdefabcdef"
#define CHARACTERISTIC_NOTIFY "abcd5678-1234-1234-1234-abcdefabcdef"

BLECharacteristic *pNotifyCharacteristic = nullptr;

typedef struct __attribute__((packed))
{
  uint32_t p;
  uint32_t g;
  uint32_t publicKey;
} DH_Message;

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() != sizeof(DH_Message))
    {
      Serial.println("Messaggio DH non valido.");
      return;
    }

    // Ricevi DH_Message
    DH_Message msg;
    memcpy(&msg, value.data(), sizeof(DH_Message));

    Serial.println("Ricevuto -> p: " + String(msg.p) + ", g: " + String(msg.g) + ", Y: " + String(msg.publicKey));

    // Configura DH con p/g ricevuti
    DiffieHellman dh = DiffieHellman(msg.p, msg.g);
    uint32_t sharedKey = dh.computeSharedKey(msg.publicKey);

    Serial.println("Chiave condivisa calcolata:" + String(sharedKey));

    // Invia la nostra chiave pubblica (Y) come notifica
    uint32_t ourPublicKey = dh.getPublicKey();
    pNotifyCharacteristic->setValue((uint8_t *)&ourPublicKey, sizeof(ourPublicKey));
    pNotifyCharacteristic->notify();

    Serial.println("Inviata chiave pubblica");
  }
};

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("Avvio BLE DH Responder...");

  BLEDevice::init("ESP32_Responder");

  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pWriteCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_WRITE,
      BLECharacteristic::PROPERTY_WRITE);

  pNotifyCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_NOTIFY,
      BLECharacteristic::PROPERTY_NOTIFY);

  pWriteCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("Dispositivo pronto e in advertising.");
}

void loop()
{
  delay(1000);
}
