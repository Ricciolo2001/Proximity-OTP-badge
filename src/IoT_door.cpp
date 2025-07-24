#include <Arduino.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include "Diffie_Hellman.hpp"
#include "Autentication_Protocol.hpp"
#include "AES_comunication.hpp"
#include "SecureOTPGenerator.hpp"

#define SERVICE_UUID "12345678-1234-1234-1234-123456789abc"
#define CHARACTERISTIC_WRITE "abcd1234-1234-1234-1234-abcdefabcdef"
#define CHARACTERISTIC_NOTIFY "abcd5678-1234-1234-1234-abcdefabcdef"

BLECharacteristic *pNotifyCharacteristic = nullptr;

DiffieHellman dh;

std::vector<std::string> tokenize(const std::string &input, char delimiter = '|')
{
  std::vector<std::string> tokens;
  std::stringstream ss(input);
  std::string item;

  while (std::getline(ss, item, delimiter))
  {
    tokens.push_back(item);
  }

  return tokens;
}

class MyCallbacks : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    std::string value = pCharacteristic->getValue();
    if (value.length() != sizeof(DH_Message))
    {
      Serial.println("Messaggio ricevuto");

      auto tokens = tokenize(value);
      auto username = tokens.front().data();

      SecureOTPGenerator s = SecureOTPGenerator(69, username);
      // NB in un caso ideale vorremmo che il server durante la prima fase di handshake invii al client il counter dell'otp che stiamo usando
      // In caso contrario potrebbero sussistere problemi di sincronizzazione se una richiesta del client non va a buon fine
      // TODO Per il momento il seed è un valore predefinito, in futuro sarà necessario creare un file di configurazione per il server per tener traccia di tutti gli utenti e dei loro otp

      uint32_t otp = s.generateOTP();

      uint8_t iv[4];
      memcpy(iv, &otp, sizeof(otp));

      auto sharedKey = dh.getSharedKey();

      uint8_t key[8];
      memcpy(key, &sharedKey, sizeof(sharedKey));

      CfbCipher cfbCipher = CfbCipher(key, iv);

      auto lastToken = tokens.back(); // Ultimo elemento
      const uint8_t *buffer = reinterpret_cast<const uint8_t *>(lastToken.data());
      size_t bufferLen = lastToken.size();

      cfbCipher.decrypt(const_cast<uint8_t *>(buffer), bufferLen);

      tokens = tokenize(value);
      if (strcmp(tokens.front().data(), username) == 0)
      {
        Serial.println("Utente Autenticato con successo");
        int32_t value = static_cast<int32_t>(std::stoul(tokens.back()));
        value++;

        pNotifyCharacteristic->setValue((uint8_t *)&value, sizeof(value));
        pNotifyCharacteristic->notify();
      }
      else
        Serial.println("Errore nome utente cambiato");
    }
    else
    {
      Serial.println("Richiesta di autenticazione ricevuta");

      // Ricevi DH_Message
      DH_Message msg;
      memcpy(&msg, value.data(), sizeof(DH_Message));

      Serial.println("Ricevuto -> p: " + String(msg.p) + ", g: " + String(msg.g) + ", Y: " + String(msg.publicKey));

      // Configura DH con p/g ricevuti
      dh = DiffieHellman(msg.p, msg.g, msg.publicKey);

      Serial.println("Chiave condivisa calcolata:" + String(dh.getSharedKey()));

      // Invia la nostra chiave pubblica (Y) come notifica
      uint32_t ourPublicKey = dh.getPublicKey();
      pNotifyCharacteristic->setValue((uint8_t *)&ourPublicKey, sizeof(ourPublicKey));
      pNotifyCharacteristic->notify();

      Serial.println("Inviata chiave pubblica");
    }
  }
};

class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer) override
  {
    Serial.println("Client connesso.");
  }

  void onDisconnect(BLEServer *pServer) override
  {
    Serial.println("Client disconnesso. Riavvio advertising...");
    delay(500); // breve attesa prima di ripartire
    BLEDevice::startAdvertising();
  }
};

void setup()
{
  Serial.begin(115200);
  Serial.println("Avvio BLE DH Responder...");

  BLEDevice::init("ESP32_Responder");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // <-- AGGIUNGI QUESTA RIGA

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
