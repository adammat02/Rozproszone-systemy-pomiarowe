# Struktura i ogólne funkcjonowanie projektu

## 1. Uproszczony schemat przepływu danych 
Główna oś komunikacji w systemie jest jednokierunkowa i opiera się na następującym łańcuchu:

**[ Węzeł ESP32 ]** ➔ **[ Broker MQTT ]** ➔ **[ Ingestor ]** ➔ **[ Baza PostgreSQL ]** ➔ **[ REST API ]** ➔ **[ Streamlit Dashboard ]**

## 2. Architektura warstwowa
Projekt został zrealizowany w oparciu o architekturę rozproszonych mikroserwisów. System składa się z czterech logicznych warstw, co zapewnia jego wysoką skalowalność oraz łatwość w izolowaniu błędów:

1. **Warstwa sprzętowa (Edge / Węzeł IoT):** Zbudowana w oparciu o mikrokontroler ESP32 zaprogramowany w języku C++. Pełni rolę producenta danych (Publisher), generując ładunki (payloads) w formacie JSON.
2. **Warstwa pośrednicząca (Middleware):** Broker wiadomości Mosquitto realizujący protokół MQTT. Odpowiada za przyjmowanie wiadomości od mikrokontrolera i bezstratne rozsyłanie ich do subskrybentów.
3. **Warstwa logiki i składowania (Backend):** Składa się z usługi Ingestora (skrypt w języku Python) oraz relacyjnej bazy danych PostgreSQL. Ingestor pełni rolę konsumenta (Subscriber) – łapie dane z brokera i umieszcza je w bazie, która gwarantuje ich trwałość (persystencję).
4. **Warstwa dostępu i prezentacji (Frontend / API):** Interfejs REST API napisany we frameworku Flask służący do bezpiecznego udostępniania zasobów z bazy oraz aplikacja webowa zrealizowana w bibliotece Streamlit, odpowiedzialna za wizualizację trendów dla użytkownika końcowego.

## 3. Ogólne funkcjonowanie systemu
System działa w pełni asynchronicznie, co oznacza, że poszczególne moduły są od siebie odseparowane. Ingestor w sposób ciągły pracuje w tle (w kontenerze Docker), nasłuchując zdarzeń na brokerze i zapisując historię pomiarów. Dzięki temu ewentualna awaria interfejsu webowego (API lub samego Dashboardu) w żaden sposób nie przerywa pracy węzła pomiarowego ESP32 ani nie powoduje utraty zbieranych danych.