///////////////////Includes//////////////////

#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "pins.h"
#include "display.h"
#include "functions.h"
#include "measure.h"
#include "pitches.h"
#include "settings.h"
#include "ESP32_BLE.h"

// https://www.uuidgenerator.net/

#define SERVICE_UUID "d4f94552-2959-496e-bc0f-d3e4f4e2da2d"
#define CHAR1_UUID "fd57b195-58af-4479-8907-e5f4fdc9e8ed"
#define CHAR2_UUID "10483f42-46f9-4789-92b4-2406b40a629d"
#define CHAR3_UUID "d4b0046c-6717-4514-a36b-0e9c21f14990"
#define CHAR4_UUID "f3826d07-101e-4e53-9b7b-c43741c31c9e"
#define CHAR5_UUID "99f5f192-b42d-439f-a9d2-993d0efa765b"
#define CHAR6_UUID "96d9f8ed-3091-47c1-a0d5-b24ed1bd09d3"
#define CHAR7_UUID "dbd25b9d-2ec4-481d-8a3b-b2ba13b2292b"
#define CHAR8_UUID "4bf2958b-59f6-4f70-b0e5-4992d2a24b5b"

BLEServer* pServer = NULL;

BLECharacteristic* pCharacteristic = NULL;
BLECharacteristic* pCharacteristic_2 = NULL;
BLECharacteristic* pCharacteristic_3 = NULL;
BLECharacteristic* pCharacteristic_4 = NULL;
BLECharacteristic* pCharacteristic_5 = NULL;
BLECharacteristic* pCharacteristic_6 = NULL;
BLECharacteristic* pCharacteristic_7 = NULL;
BLECharacteristic* pCharacteristic_8 = NULL;

BLE2902* pBLE2902;

/*
  CHAR Value:

  -CHAR1 - DischargeState
  -CHAR2 - BATVoltage
  -CHAR3 - AvgCellVoltage
  -CHAR4 - DischargeVoltage
  -CHAR5 - CellType
  -CHAR6 - DischargePercent
  -CHAR7 - VERSION
  -CHAR8 - Discharge delta
*/

/////////////////////////////////////////////

//////////////////Variables//////////////////

unsigned int BLEStat = 0;  //0 - OFF, 1 - ON, 2 - connected

bool deviceConnected = false;
bool oldDeviceConnected = false;

/////////////////////////////////////////////

////////////////////Code/////////////////////

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEStat = 2;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      BLEStat = 1;
    }
};

void BLELoop() {

  if (BLEStat >= 1) {

    // notify changed value
    if (deviceConnected) {

      pCharacteristic->setValue(String(DischargeState).c_str());
      pCharacteristic->notify();

      pCharacteristic_2->setValue(String(String(BATVoltage, 3)).c_str());
      pCharacteristic_2->notify();

      pCharacteristic_3->setValue(String(String(AvgCellVoltage, 3)).c_str());
      pCharacteristic_3->notify();

      pCharacteristic_4->setValue(String(DischargeVoltage).c_str());
      pCharacteristic_4->notify();

      pCharacteristic_5->setValue(String(CellType).c_str());
      pCharacteristic_5->notify();

      pCharacteristic_6->setValue(String(DischargePercent).c_str());
      pCharacteristic_6->notify();

      pCharacteristic_7->setValue(VERSION);
      pCharacteristic_7->notify();

      pCharacteristic_8->setValue(String(DischargeDelta).c_str());
      pCharacteristic_8->notify();

    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
      delay(500);                   // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising();  // restart advertising
      Serial.println("Start advertising");
      oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
    }
  }
}

void StartBLE() {

  String BLEDeviceName = String("BDC_" + String(GetSerialNumber()));

  BLEDevice::init(BLEDeviceName.c_str());

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService* pService = pServer->createService(BLEUUID(SERVICE_UUID), 30, 0);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHAR1_UUID,
                      BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_2 = pService->createCharacteristic(
                        CHAR2_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_3 = pService->createCharacteristic(
                        CHAR3_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_4 = pService->createCharacteristic(
                        CHAR4_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_5 = pService->createCharacteristic(
                        CHAR5_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_6 = pService->createCharacteristic(
                        CHAR6_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_7 = pService->createCharacteristic(
                        CHAR7_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic_8 = pService->createCharacteristic(
                        CHAR8_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY);

  pBLE2902 = new BLE2902();
  pBLE2902->setNotifications(true);

  // Add all Descriptors here
  pCharacteristic->addDescriptor(pBLE2902);

  pCharacteristic_2->addDescriptor(pBLE2902);

  pCharacteristic_3->addDescriptor(pBLE2902);

  pCharacteristic_4->addDescriptor(pBLE2902);

  pCharacteristic_5->addDescriptor(pBLE2902);

  pCharacteristic_6->addDescriptor(pBLE2902);

  pCharacteristic_7->addDescriptor(pBLE2902);

  pCharacteristic_8->addDescriptor(pBLE2902);

  // Start the service
  pService->start();

  BLEStat = 1;

  // Start advertising
  BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void StopBLE() {
  BLEStat = 0;
  BLEDevice::stopAdvertising();
  BLEDevice::deinit();
  BLEStat = 0;
}

/////////////////////////////////////////////
