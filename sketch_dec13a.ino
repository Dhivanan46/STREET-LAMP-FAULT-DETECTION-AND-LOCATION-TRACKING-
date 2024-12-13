#include <TinyGPS++.h> // Include the TinyGPS++ library for GPS functionality
#include <WiFi.h>       // Include the WiFi library for connecting to Wi-Fi

// Wi-Fi credentials
const char* ssid = "your_wifi_ssid";       // Replace with your Wi-Fi SSID
const char* password = "your_wifi_password"; // Replace with your Wi-Fi password

// Pin definitions for LDRs and relay
const int ldrPin1 = 13;  // LDR 1 connected to pin 13
const int ldrPin2 = 12;  // LDR 2 connected to pin 12
const int ldrPin3 = 14;  // LDR 3 connected to pin 14
const int ldrPin4 = 27;  // LDR 4 connected to pin 27
const int relayPin1 = 26; // Relay connected to pin 26

// Blink app endpoint and API key
const char* blinkEndpoint = "http://blinkapp.com/update_location"; // Endpoint for Blink app
const char* apiKey = "your_blink_api_key";                         // Replace with your Blink API key

// Create instances of TinyGPSPlus and WiFiClient
TinyGPSPlus gps;
WiFiClient client;

void setup() {
  Serial.begin(115200); // Initialize serial communication at 115200 baud
  connectToWiFi();      // Connect to Wi-Fi

  // Set up pin modes
  pinMode(ldrPin1, INPUT);   // LDR 1 as input
  pinMode(ldrPin2, INPUT);   // LDR 2 as input
  pinMode(ldrPin3, INPUT);   // LDR 3 as input
  pinMode(ldrPin4, INPUT);   // LDR 4 as input
  pinMode(relayPin1, OUTPUT); // Relay as output
}

void loop() {
  // Read LDR values
  int ldrValue1 = analogRead(ldrPin1);
  int ldrValue2 = analogRead(ldrPin2);
  int ldrValue3 = analogRead(ldrPin3);
  int ldrValue4 = analogRead(ldrPin4);

  // Print LDR values to the serial monitor
  Serial.println("LDR Values:");
  Serial.print("LDR 1: "); Serial.println(ldrValue1);
  Serial.print("LDR 2: "); Serial.println(ldrValue2);
  Serial.print("LDR 3: "); Serial.println(ldrValue3);
  Serial.print("LDR 4: "); Serial.println(ldrValue4);

  // Check if LDR 1 detects sufficient light (daytime)
  if (ldrValue1 > 3000) {
    digitalWrite(relayPin1, HIGH); // Turn on the relay (lamps on)
    Serial.println("All lamps are on");

    // Check individual LDRs for faults and send location to Blink if necessary
    if (ldrValue2 > 2500) {
      if (gps.encode(Serial.read())) {
        if (gps.location.isValid()) {
          sendDataToBlink(gps.location.lat(), gps.location.lng());
          Serial.println("Fault: Street Lamp 1 is not working.");
        }
      }
    }

    if (ldrValue3 > 2500) {
      if (gps.encode(Serial.read())) {
        if (gps.location.isValid()) {
          sendDataToBlink(gps.location.lat(), gps.location.lng());
          Serial.println("Fault: Street Lamp 2 is not working.");
        }
      }
    }

    if (ldrValue4 > 200) {
      if (gps.encode(Serial.read())) {
        if (gps.location.isValid()) {
          sendDataToBlink(gps.location.lat(), gps.location.lng());
          Serial.println("Fault: Street Lamp 3 is not working.");
        }
      }
    }
  } else {
    // Nighttime, turn off street lamps
    digitalWrite(relayPin1, LOW);
  }

  delay(5000); // Wait for 5 seconds before the next iteration
}

// Function to connect to Wi-Fi
void connectToWiFi() {
  Serial.println("Connecting to WiFi");
  WiFi.begin(ssid, password); // Begin Wi-Fi connection
  while (WiFi.status() != WL_CONNECTED) { // Wait until connected
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// Function to send data to the Blink app
void sendDataToBlink(float latitude, float longitude) {
  if (client.connect(blinkEndpoint, 80)) { // Connect to the Blink endpoint
    // Prepare data to send
    String data = "lat=" + String(latitude, 6) + "&lng=" + String(longitude, 6) + "&apiKey=" + apiKey;

    // Send HTTP POST request
    client.println("POST " + String(blinkEndpoint) + " HTTP/1.1");
    client.println("Host: blinkapp.com");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(data.length()));
    client.println();
    client.println(data);
    client.stop(); 
    Serial.println("Location sent to Blink");
  } else {
    Serial.println("Failed to connect to Blink"); 
  }
}
