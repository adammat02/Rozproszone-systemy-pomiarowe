#include "mqtt_manager.h"
#include "secrets.h"
#include <ArduinoJson.h>

#define SCHEMA_VERSION 2

mqtt_manager::mqtt_manager(WiFiClient& espClient) : mqttClient(espClient) {}

void mqtt_manager::begin(const String& deviceID, const String& deviceTopic)
{
    mainTopic = deviceTopic;
    deviceId = deviceID;
    seq_data_counter = 0;
    seq_status_counter = 0;
}

void mqtt_manager::connectMQTT()
{
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    
    if (mqttClient.connected()) {
        return;
    }

    Serial.print("Laczenie z MQTT...");
    deviceId = "esp32-pola";
    
    // Konfiguracja "Testamentu" (Last Will and Testament)
    String willTopic = mainTopic + "/status";
    
    JsonDocument doc;
    doc["schema_version"] = SCHEMA_VERSION;
    doc["group_id"] = MQTT_GROUP;
    doc["device_id"] = deviceId;
    doc["status"] = "offline";
    doc["ts_ms"] = 0; 
    doc["seq"] = seq_status_counter;
    doc["type"] = "status";
    
    String willPayload;
    serializeJson(doc, willPayload);

    // Połączenie z parametrami LWT
    bool ok = mqttClient.connect(
        deviceId.c_str(),
        NULL,           
        NULL,           
        willTopic.c_str(), 
        0,              
        true,           // retain = true, żeby broker zapamiętał status offline
        willPayload.c_str()
    );

    if (ok) {
        Serial.println("OK");
    } else {
        Serial.print("blad, rc=");
        Serial.println(mqttClient.state());
    }
}

void mqtt_manager::publishMeasurement(const String &sensor, float value, const String &unit, long long ts_ms)
{
    JsonDocument doc;
    doc["schema_version"] = SCHEMA_VERSION;
    doc["group_id"] = MQTT_GROUP;
    doc["device_id"] = deviceId;
    doc["sensor"] = sensor;
    doc["value"] = value;
    doc["unit"] = unit;
    doc["ts_ms"] = ts_ms;
    doc["seq"] = seq_data_counter++;
    
    // ZMIANA: "meas" zamiast "data", żeby pasowało do Twojego Ingestora i kontraktu v2!
    doc["type"] = "meas"; 

    char payload[256];
    serializeJson(doc, payload);

    String topic = mainTopic + "/" + sensor;

    mqttClient.publish(topic.c_str(), payload);
    Serial.print("Publikacja na topic: ");
    Serial.println(topic);
    Serial.println(payload);
}

void mqtt_manager::publishStatus(const String &status, long long ts_ms)
{
    JsonDocument doc;
    doc["schema_version"] = SCHEMA_VERSION;
    doc["group_id"] = MQTT_GROUP;
    doc["device_id"] = deviceId;
    doc["status"] = status;
    doc["ts_ms"] = ts_ms;
    doc["seq"] = seq_status_counter++;
    doc["type"] = "status";

    char payload[256];
    serializeJson(doc, payload);

    String topic = mainTopic + "/status";

    // retain = true, nowi subskrybenci od razu dostaną obecny status
    mqttClient.publish(topic.c_str(), payload, true); 
    Serial.print("Publikacja na topic: ");
    Serial.println(topic);
    Serial.println(payload);
}