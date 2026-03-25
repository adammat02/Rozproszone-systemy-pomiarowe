#include <Arduino.h>
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "secrets.h"
#include "device.h"

WiFiClient espClient;
mqtt_manager mqttClient(espClient);

String deviceId;
String topic;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  deviceId = generateDeviceIdFromEfuse();
  topic = "lab/" + String(MQTT_GROUP) + "/" + deviceId;

  Serial.print("Device ID: ");
  Serial.println(deviceId);

  mqttClient.begin(deviceId, topic);

  connectWiFi();
  mqttClient.connectMQTT();
}

void loop()
{
  if (!isWiFiConnected())
  {
    connectWiFi();
  }

  if (!mqttClient.isConnected())
  {
    mqttClient.connectMQTT();
  }

  mqttClient.loop();
  
  float tempC = temperatureRead();
  mqttClient.publishMeasurement("temperature", tempC, "C");
  
  delay(5000);
}