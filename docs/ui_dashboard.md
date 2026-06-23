# Dokumentacja Interfejsu Użytkownika (Streamlit UI)

## 1. Architektura i Koncepcja
Zamiast środowiska LabVIEW, warstwę prezentacji (UI) zrealizowano przy użyciu nowoczesnego frameworka **Streamlit** (język Python). Aplikacja działa jako niezależny klient HTTP, komunikujący się wyłącznie z wystawionym REST API (oddzielenie logiki backendu od interfejsu). Do manipulacji danymi i obsługi czasu wykorzystano bibliotekę `pandas`.

## 2. Funkcjonalności Dashboardu
Zgodnie z założeniami projektowymi, panel został podzielony na 3 główne obszary robocze:

### Obszar 1: Konfiguracja i diagnostyka
* Możliwość dynamicznej zmiany adresu bazowego API (`http://localhost:5001`).
* Przycisk **Test API**, odpytujący endpoint `/health`. Wyświetla komunikaty o sukcesie lub kodach błędów HTTP.

### Obszar 2: Filtrowanie danych
* Dynamiczne pobieranie listy dostępnych urządzeń z endpointu `/devices`.
* Możliwość określenia interesującego sensora oraz nałożenia limitu pobieranych rekordów.

### Obszar 3: Prezentacja wyników
* **Latest measurement:** Pobiera najnowszą próbkę z użyciem endpointu historii (`limit=1`). Wyświetla dane w formie czytelnych kafelków (`st.metric`) oraz przelicza czas `ts_ms` na czytelny format daty.
* **Historia i Trend:** Wykorzystuje endpoint `/measurements/history` z nałożonymi filtrami. Ładuje odpowiedź JSON do ramki danych `pandas`, a następnie generuje interaktywny wykres liniowy oraz tabelę surowych danych.

## 3. Instrukcja Uruchomienia
1. Upewnić się, że kontenery z bazą danych i REST API działają w tle (`docker compose up -d`).
2. Zainstalować wymagane pakiety: `pip install streamlit requests pandas`.
3. Uruchomić aplikację poleceniem z poziomu terminala:
   `streamlit run dashboard.py`
4. Aplikacja otworzy się automatycznie w domyślnej przeglądarce pod adresem `http://localhost:8501`.