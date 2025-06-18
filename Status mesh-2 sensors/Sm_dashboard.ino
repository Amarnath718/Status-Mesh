#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

// ====== USER CONFIGURATION ======
const char* ssid = "redmi";
const char* password = "123456789";

#define IR_SENSOR_PIN  4
#define FACILITY_NAME "Facility 1"

WebServer server(80);

String status = "Not Occupied";
String lastSeen = "Never";

String status2 = "Not Occupied";
String lastSeen2 = "Never";

void setup() {
  Serial.begin(115200);
  pinMode(IR_SENSOR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  configTime(19800, 0, "pool.ntp.org");  // GMT+5:30 (IST)

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  server.handleClient();
  updateStatus();
}

void updateStatus() {
  int sensorValue = digitalRead(IR_SENSOR_PIN);
  String newStatus = (sensorValue == LOW) ? "OPEN" : "CLOSED";
  if (newStatus != status) {
    status = newStatus;
    lastSeen = getTimeString();
    Serial.println("Status changed to: " + status);
  }
}

String getTimeString() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "Unknown";
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

void handleUpdate() {
  if (server.hasArg("facility") && server.hasArg("status")) {
    String facility = server.arg("facility");
    String newStatus = server.arg("status");
    String timeStr = getTimeString();

    if (facility == "2") {
      status2 = newStatus;
      lastSeen2 = timeStr;
      Serial.println("Received from Facility 2: " + newStatus);
    }

    server.send(200, "text/plain", "OK");
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void handleRoot() {
  updateStatus();
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <meta charset='UTF-8'>
      <meta name='viewport' content='width=device-width, initial-scale=1.0'>
      <title>Status Mesh</title>
      <style>
        body {
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
          background-color: #1e1e2f;
          color: #fff;
          margin: 0;
          padding: 20px;
        }
        h1 {
          text-align: center;
          margin-bottom: 30px;
          color: #ffffff;
          font-size: 2.5rem;
        }
        .container {
          display: flex;
          flex-wrap: wrap;
          justify-content: center;
          gap: 20px;
        }
        .card {
          background: #2c2c3e;
          border: 1px solid #444;
          border-radius: 16px;
          box-shadow: 0 4px 20px rgba(0,0,0,0.3);
          padding: 20px;
          width: 280px;
          transition: transform 0.2s ease-in-out;
        }
        .card:hover {
          transform: scale(1.02);
        }
        .title {
          font-size: 20px;
          font-weight: bold;
          color: #ffffff;
        }
        .status {
          margin-top: 10px;
          font-size: 18px;
          color: #00e676;
        }
        .status.occupied {
          color: #ff5252;
        }
        .footer {
          margin-top: 10px;
          font-size: 14px;
          color: #bbbbbb;
        }
      </style>
    </head>
    <body>
      <h1>Status Mesh</h1>
      <div class='container'>
  )rawliteral";

  for (int i = 1; i <= 10; i++) {
    html += "<div class='card'>";
    if (String("Facility ") + i == FACILITY_NAME) {
      html += "<div class='title'>" + String(FACILITY_NAME) + "</div>";
      html += "<div class='status " + String((status == "CLOSED") ? "occupied" : "") + "'>Current Status: " + status + "</div>";
      html += "<div class='footer'>Last Seen: " + lastSeen + "</div>";
    } else if (i == 2) {
      html += "<div class='title'>Facility 2</div>";
      html += "<div class='status " + String((status2 == "CLOSED") ? "occupied" : "") + "'>Current Status: " + status2 + "</div>";
      html += "<div class='footer'>Last Seen: " + lastSeen2 + "</div>";
    } else {
      html += "<div class='title'>Facility " + String(i) + "</div>";
      html += "<div class='status'>No information</div>";
      html += "<div class='footer'>Last Seen: --</div>";
    }
    html += "</div>";
  }

  html += R"rawliteral(
      </div>
      <script>
        setTimeout(() => { location.reload(); }, 5000);
      </script>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}
