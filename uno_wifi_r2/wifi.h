// Dependencies: secrets for wifi connection

#include <SPI.h>
#include <WiFiNINA.h>

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient wifiClient;
int status = WL_IDLE_STATUS;
const char* server = "www.google.com";    // name address for Google (using DNS)

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void setupWifiClient(char *ssid, char *pass) {
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }
  
  while (status != WL_CONNECTED) {
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
}

void testWifiConnection() {
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (wifiClient.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    wifiClient.println("GET /search?q=arduino HTTP/1.1");
    wifiClient.println("Host: www.google.com");
    wifiClient.println("Connection: close");
    wifiClient.println();
  }
}

void loopWifiClient() {
  // if the server's disconnected, stop the client:
  if (!wifiClient.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    wifiClient.stop();

    // do nothing forevermore:
    while (true);
  }

  if(WiFi.status() != WL_CONNECTED) {
    Serial.print("wifi status:");
    Serial.println(WiFi.status());
  }
}
