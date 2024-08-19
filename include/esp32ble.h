#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const int ledPin = 2; // Use the appropriate GPIO pin for your setup
bool deviceConnected = false;
class esp32ble
{
private:
    BLEServer* pServer = NULL;
    BLECharacteristic* pSensorCharacteristic = NULL;
    BLECharacteristic* pLedCharacteristic = NULL;
    bool oldDeviceConnected = false;
    uint32_t value = 0;


    // See the following for generating UUIDs:
    // https://www.uuidgenerator.net/
    #define SERVICE_UUID        "19b10000-e8f2-537e-4f6c-d104768a1214"
    #define SENSOR_CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
    #define LED_CHARACTERISTIC_UUID "19b10002-e8f2-537e-4f6c-d104768a1214"

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
            String value = "1"; //pLedCharacteristic->getValue();
            if (value.length() > 0) {
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
    esp32ble(/* args */);
    ~esp32ble();



};


esp32ble::esp32ble(/* args */)
{
}

esp32ble::~esp32ble()
{
}
