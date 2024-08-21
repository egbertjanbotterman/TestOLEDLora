#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int ledPin = 2; // Use the appropriate GPIO pin for your setup
bool deviceConnected = false;
class Esp32BLE
{
private:
    BLEServer* pServer = NULL;
    BLECharacteristic* pSensorCharacteristic = NULL;
    BLECharacteristic* pLedCharacteristic = NULL;
    bool oldDeviceConnected = false;
    uint32_t value = 0;


    // See the following for generating UUIDs:
    // https://www.uuidgenerator.net/
    #define SERVICE_UUID                "19b10000-e8f2-537e-4f6c-d104768a1242"
    #define SENSOR_CHARACTERISTIC_UUID  "19b10001-e8f2-537e-4f6c-d104768a1242"
    #define LED_CHARACTERISTIC_UUID     "19b10002-e8f2-537e-4f6c-d104768a1242"

    class MyServerCallbacks: public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {
            deviceConnected = true;
        };

        void onDisconnect(BLEServer* pServer) {
            deviceConnected = false;
        }
    };
    class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
        void onWrite(BLECharacteristic* pLedCharacteristic) {
            String *value = new String(pLedCharacteristic->getValue());
            if (value->length() > 0) {
            Serial.print("Characteristic event, written: ");
            Serial.println(static_cast<int>(value[0])); // Print the integer value

            int receivedValue = static_cast<int>(value[0]);
            if (receivedValue == 1) {
                digitalWrite(ledPin, HIGH);
            } else {
                digitalWrite(ledPin, LOW);
            }
            }
        }
    };

public:
    Esp32BLE(String deviceName);
    ~Esp32BLE();

    BLEServer* GetServer() { return pServer; }
};


Esp32BLE::Esp32BLE(String deviceName)
{
    // Create the BLE Device
    BLEDevice::init(deviceName.c_str());

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pSensorCharacteristic = pService->createCharacteristic(
        SENSOR_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ
    );

    pSensorCharacteristic->setValue((uint8_t*)&value, 4);

    // Create a BLE Characteristic
    pLedCharacteristic = pService->createCharacteristic(
        LED_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE
    );

    pLedCharacteristic->setValue((uint8_t*)&value, 4);
    pLedCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

Esp32BLE::~Esp32BLE()
{
}
