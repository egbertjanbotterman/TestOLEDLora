#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include "WiFi.h"

// WiFi credentials.
const char* WIFI_SSID = "EgbertsIOTExperiments";
const char* WIFI_PASS = "You missed a starting gun!";

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

void InitLoRa() {
  // put your setup code here, to run once:
  Serial.print("MOSI: ");
  Serial.println(MOSI);
  Serial.print("MISO: ");
  Serial.println(MISO);
  Serial.print("SCK: ");
  Serial.println(SCK);
  Serial.print("SS: ");
  Serial.println(SS);  

  if(!LoRa.begin(4348E5)) {
    Serial.println("Starting LoRa failed!");
    display.println("Starting LoRa failed!");
    display.display();
    while(1);
  } 
  else 
  { 
    display.println("LoRa listening started!"); 
  }
  display.display();
}

void InitWiFi() {
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.println("Connecting to WiFi..");
    display.display();
  }
  display.println("Connected to the WiFi network");
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.display();
}

void setup() {
  // Start the Serial communication
  Serial.begin(115200);

  // Initialize the display
  InitDisplay();

  // Initialize WiFi 
  InitWiFi();

  // Initialize LoRa (sx1278) receiver
  InitLoRa();
}
 

void loop() {
   // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    display.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      display.print((char)LoRa.read());
    }

    // print RSSI of packet
    display.print("' with RSSI ");
    display.println(LoRa.packetRssi());
    display.display();
  }
}
