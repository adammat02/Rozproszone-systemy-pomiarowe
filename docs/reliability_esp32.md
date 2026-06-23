# Niezawodność węzła pomiarowego ESP32 (Reliability)

## 1. Architektura Reconnect
Urządzenie bazuje na nieblokującej architekturze asynchronicznej (odpytywanie zegara systemowego `millis()`). W pętli głównej zaimplementowano dwa niezależne mechanizmy wznawiania połączenia:
* **Wi-Fi Reconnect:** Co 5 sekund weryfikowany jest status `WiFi.status()`.
* **MQTT Reconnect:** Co 3 sekundy weryfikowany jest status `client.connected()`.
Rozdzielenie tych logik ułatwia diagnostykę problemów sieciowych od problemów z samym brokerem.

## 2. Topic Statusowy i Last Will (LWT)
System odseparowuje komunikaty pomiarowe od statusowych. 
* **Topic statusowy:** `lab/grupa/esp32-pola/status`
* **Status online:** Publikowany automatycznie z poziomu funkcji reconnect natychmiast po poprawnym odzyskaniu połączenia z brokerem.
* **Status offline (Last Will):** Zdefiniowano mechanizm LWT. W momencie rejestracji w brokerze, ESP32 deponuje wiadomość statusową awaryjną (`offline`). Jeśli urządzenie zostanie nagle odłączone od zasilania, broker automatycznie roześle ten komunikat do aplikacji Ingestora/UI.

## 3. Scenariusze Testowe
Przeprowadzono następujące testy awarii:
1. **Zanik sieci Wi-Fi:** Po wyłączeniu punktu dostępowego (hotspotu), ESP32 zaprzestało publikacji i płynnie przeszło w stan `reconnecting` raportując problem na porcie szeregowym. Po ponownym uruchomieniu AP, urządzenie wznowiło pracę.
2. **Utrata brokera MQTT:** Po zatrzymaniu kontenera z mosquitto (`docker stop`), ESP32 utrzymało warstwę Wi-Fi, próbując co 3 sekundy połączyć się z brokerem. Po resecie kontenera, opublikowano natychmiast status `online` i wznowiono streaming danych.
3. **Test LWT (Last Will):** W trakcie pracy nagle odłączono ESP32 z portu USB. W programie MQTT Explorer natychmiast zaobserwowano pojawienie się komunikatu o statusie `offline` opublikowanego przez brokera w imieniu urządzenia.