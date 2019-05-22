// Library: https://github.com/256dpi/arduino-mqtt
// Dependencies: WifiClient, secrets for mqtt

#include <MQTTClient.h>

MQTTClient mqttClient;
unsigned long lastMillis = 0;

const int TIMER = 10000;
const char* MQTT_TOPIC_TIMER = "nodemcu/timer";

void connectMqtt() {
  Serial.println();
  Serial.print("Connecting to MQTT broker...\nClient Id:");
  Serial.println(mqttClientId);

  while (!mqttClient.connect(mqttClientId, mqttUsername, mqttPassword)) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("MQTT connected.");
}

void setupMqttClient(char* mqttBrokerHost, WiFiClient wifiClient) {
  mqttClient.begin(mqttBrokerHost, wifiClient);
  connectMqtt();
}

void loopMqttClient() {
  mqttClient.loop();
  delay(50);              // for stability on ESP8266
  
  // publish a message roughly every ten second.
  if (millis() - lastMillis > TIMER) {
    lastMillis = millis();
    mqttClient.publish(MQTT_TOPIC_TIMER, TIMER / 1000);
  }
}
