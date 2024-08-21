#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include "Esp32BLE.h"

// The ESP32 bluetooth BLE device
Esp32BLE* ble;

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define OLED_RESET if the OLED reset pin is connected, else set to -1
#define OLED_RESET    -1

// Initialize the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void InitDisplay() {
  // Initialize the display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Clear the buffer
  display.clearDisplay();

  // Draw a test pattern
  for(int16_t i=0; i<SCREEN_HEIGHT; i+=4) {
    display.drawLine(0, i, SCREEN_WIDTH-1, i, WHITE);
  }

  // Display the buffer
  display.display();

  delay(500);
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println("Display initialized");
  // Display the buffer
  display.display();
}

String incoming = "";
int lastRSSI = 0;
bool newPacket = false;

// Interrupt handler, so no serial.print(), or BLE traffic in here!
void onReceiveIRQ(int packetSize) {

  incoming = "";
  for (int i = 0; i < packetSize; i++) {
    incoming += (char)LoRa.read();
  }
  lastRSSI = LoRa.packetRssi();
  newPacket = true;
}


// SX1278 pins
#define RST 15    // GPIO15 -- SX1278 RST
#define DIO0 4    // GPIO4  -- SX1278 DIO0

// Initialize LoRa (sx1278) receiver
void InitLoRa() {
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(SS, RST, DIO0);

  //Start LoRa
  if(!LoRa.begin(434800000)) {
    Serial.println("Starting LoRa failed!");
    display.println("Starting LoRa failed!");
    display.display();
    while(1);
  } 
  else 
  { 
    // LoRa.implicitHeaderMode();
    LoRa.setSyncWord(0xC0);
    LoRa.setSignalBandwidth(625E2);
    LoRa.setSpreadingFactor(12);

    // LoRa.setGain(6);
    
    LoRa.onReceive(onReceiveIRQ);
    LoRa.receive();

    display.println("LoRa initialized");
  }
  display.display();
}

void setup() {
  // Start the Serial communication
  Serial.begin(115200);

  // Initialize the display
  InitDisplay();

  // Initialize LoRa (sx1278) receiver
  InitLoRa();

  // Initialize the BLE device
  ble = new Esp32BLE("ESP32_LoRa_EJB");
}


int packets = 0;

void ProcesLoRaPacket() {

  // Is there a new packet?
  if (!newPacket) 
    return;

// Immediately reset the flag    
  newPacket = false;

// Display the received packet
  display.clearDisplay();
  display.setCursor(0,0);
  display.printf("Received %d\nRSSI: %d\n", ++packets, lastRSSI);
  display.println(incoming);
  display.display();
  Serial.printf("Received %d, RSSI: %d :'%s'\n", packets, lastRSSI, incoming.c_str());

  char outgoing[100];
  sprintf(outgoing, "%d, RSSI: %d\n'%s'\n", packets, lastRSSI, incoming.c_str());

  // Send the received packet over BLE  
  ble->SendData(outgoing);
}

void loop() {
  // put your main code here, to run repeatedly:
  // LoRaLoop();
  ProcesLoRaPacket();
  ble->ProcessLoop();
}

 