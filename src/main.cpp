#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>

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

  delay(3000);
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

  if(!LoRa.begin(435E6)) {
    Serial.println("Starting LoRa failed!");
    display.println("Starting LoRa failed!");
    display.display();
    while(1);
  }

}

void setup() {
  // Start the Serial communication
  Serial.begin(115200);

  InitDisplay();
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
