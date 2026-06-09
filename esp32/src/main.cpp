#include <Arduino.h>
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "time_sync.h"
#include "secrets.h"
#include "device.h"

WiFiClient espClient;
mqtt_manager mqttClient(espClient);

String deviceId;
String topic;

// Zmienne przechowujące czas ostatniej akcji (do nieblokującego sprawdzania)
unsigned long lastWifiAttemptMs = 0;
unsigned long lastMqttAttemptMs = 0;
unsigned long lastMeasurementMs = 0;

// Odstępy czasowe w milisekundach
const unsigned long WIFI_RETRY_MS = 5000;
const unsigned long MQTT_RETRY_MS = 3000;
const unsigned long MEASUREMENT_PERIOD_MS = 5000; // Zamiast delay(5000)

void setup()
{
  Serial.begin(115200);
  delay(1000);

  deviceId = generateDeviceIdFromNvs();
  topic = "lab/" + String(MQTT_GROUP) + "/" + deviceId;

  Serial.print("Device ID: ");
  Serial.println(deviceId);

  mqttClient.begin(deviceId, topic);

  // Pierwsze połączenie przy starcie
  connectWiFi();
  synchronizeTime();
  mqttClient.connectMQTT();
  
  if (mqttClient.isConnected()) {
      mqttClient.publishStatus("online", getTimestampMs());
  }
}

void loop()
{
  unsigned long currentMs = millis();

  // 1. Sprawdzanie i reconnect Wi-Fi (nieblokujące)
  if (!isWiFiConnected())
  {
    if (currentMs - lastWifiAttemptMs >= WIFI_RETRY_MS)
    {
      lastWifiAttemptMs = currentMs;
      Serial.println("WiFi disconnected. Trying reconnect...");
      connectWiFi();
    }
  }
  else
  {
    // 2. Sprawdzanie i reconnect MQTT (tylko gdy jest Wi-Fi)
    if (!mqttClient.isConnected())
    {
      if (currentMs - lastMqttAttemptMs >= MQTT_RETRY_MS)
      {
        lastMqttAttemptMs = currentMs;
        Serial.println("MQTT disconnected. Trying reconnect...");
        mqttClient.connectMQTT();
        
        // Jeśli reconnect się uda, od razu wyślij status online
        if (mqttClient.isConnected()) {
           mqttClient.publishStatus("online", getTimestampMs());
        }
      }
    }
    else
    {
      // 3. Podtrzymanie komunikacji z brokerem
      mqttClient.loop();
    }
  }

  // 4. Publikacja pomiarów co 5 sekund BEZ użycia delay()
  if (currentMs - lastMeasurementMs >= MEASUREMENT_PERIOD_MS)
  {
    lastMeasurementMs = currentMs; // Zresetuj stoper

    long long time = 0;
    if (isTimeSynchronized())
    {
      time = getTimestampMs();
    }

    float tempC = temperatureRead(); // Wbudowany pomiar z rdzenia ESP32
    mqttClient.publishMeasurement("temperature", tempC, "C", time);
  }
}
