#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class mqtt_manager
{
  PubSubClient mqttClient;
  String deviceId;
  String mainTopic;

public:
  mqtt_manager(WiFiClient &espClient);
  void begin(const String& deviceID, const String &deviceTopic);
  void connectMQTT();
  void publishMeasurement(const String &sensor, float value, const String &unit);
  bool isConnected() { return mqttClient.connected(); }
  bool loop() { return mqttClient.loop(); }
};

#endif // MQTT_MANAGER_H