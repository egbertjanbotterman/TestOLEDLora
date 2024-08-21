#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

bool deviceConnected = false;
const int ledPin = 2; // Use the appropriate GPIO pin for your setup
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
            String value = pLedCharacteristic->getValue().c_str();
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
    Esp32BLE(String deviceName);
    ~Esp32BLE();

    void ProcessLoop();
    void SendData(String data);
    BLEServer* GetServer() { return pServer; }
};


Esp32BLE::Esp32BLE(String deviceName)
{
 // Create the BLE Device
  BLEDevice::init("ESP32_LoRa_EJB");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pSensorCharacteristic = pService->createCharacteristic(
                      SENSOR_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Create the ON button Characteristic
  pLedCharacteristic = pService->createCharacteristic(
                      LED_CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  // Register the callback for the ON button characteristic
  pLedCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pSensorCharacteristic->addDescriptor(new BLE2902());
  pLedCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();}

Esp32BLE::~Esp32BLE()
{
}

void Esp32BLE::SendData(String data)
{
    pSensorCharacteristic->setValue(data.c_str());
    pSensorCharacteristic->notify();
}

void Esp32BLE::ProcessLoop()
{
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
