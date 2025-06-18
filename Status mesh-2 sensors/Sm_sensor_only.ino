#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "redmi";
const char* password = "123456789";

#define IR_SENSOR_PIN  4
const char* host = "192.168.47.18";  // Replace with ESP32 #2's IP address

String prevStatus = "";

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
}

void loop() {
  String newStatus = (digitalRead(IR_SENSOR_PIN) == LOW) ? "OPEN" : "CLOSED";
  if (newStatus != prevStatus) {
    prevStatus = newStatus;
    sendStatus(newStatus);
  }
  delay(1000);
}

void sendStatus(String status) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(host) + "/update?facility=2&status=" + status;
    http.begin(url);
    int httpCode = http.GET();
    Serial.println("Sent to Dashboard: " + status + " | HTTP Code: " + httpCode);
    http.end();
  }
}
