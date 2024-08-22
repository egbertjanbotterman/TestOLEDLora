#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class MyServerCallbacks: public BLEServerCallbacks {

public:
    bool deviceConnected = false;

    void onConnect(BLEServer* pServer) {
        Serial.println("Device connected");
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        Serial.println("Device disconnected");
        deviceConnected = false;
    }
};
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pLedCharacteristic) {
        String value = pLedCharacteristic->getValue().c_str();
        if (value.length() > 0) {
            digitalWrite(2, HIGH);
            Serial.println("LED on");
        } else {
            digitalWrite(2, LOW);
            Serial.println("LED off");
        }
    }
};

class Esp32BLE
{
private:
    BLEServer* pServer = NULL;
    MyServerCallbacks* pServerCallbacks = NULL;
    BLECharacteristic* pSensorCharacteristic = NULL;
    BLECharacteristic* pLedCharacteristic = NULL;
    bool oldDeviceConnected = false;
    bool IsDeviceConnected() { return pServerCallbacks->deviceConnected; }

    // See the following for generating UUIDs:
    // https://www.uuidgenerator.net/
    #define SERVICE_UUID                "19b10000-e8f2-537e-4f6c-d104768a1242"
    #define SENSOR_CHARACTERISTIC_UUID  "19b10001-e8f2-537e-4f6c-d104768a1242"
    #define LED_CHARACTERISTIC_UUID     "19b10002-e8f2-537e-4f6c-d104768a1242"
public:

    Esp32BLE(String deviceName);
    ~Esp32BLE();

    void ProcessLoop();
    void SendData(String data);
    BLEServer* GetServer() { return pServer; }
};
