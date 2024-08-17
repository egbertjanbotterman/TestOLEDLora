#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include "WiFi.h"

// WiFi credentials.
const char* WIFI_SSID = "FritzseBadmuts";
const char* WIFI_PASS = "There is no sp00n!";

// OLED display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Define OLED_RESET if the OLED reset pin is connected, else set to -1
#define OLED_RESET    -1

// Initialize the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Server
WiFiServer server(80);

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
    // LoRa.implicitHeaderMode();
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

  server.begin();
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
 
void LoRaLoop() {
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

void WiFiLoop()
{
  String header;

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() ) {  // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  LoRaLoop();
  WiFiLoop();
}


void PrintIt(const char* message) {
  display.println(message);
  display.display();
  Serial.println(message);
}
 