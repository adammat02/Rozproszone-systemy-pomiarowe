# Dokumentacja Serwisu Ingestor

## 1. Architektura i Konfiguracja bazy danych
Ingestor został zaimplementowany w języku Python. Logika połączenia z bazą danych PostgreSQL została wydzielona do osobnego modułu `db.py`, wykorzystującego bibliotekę `psycopg2`.

Schemat bazy danych zakłada użycie dwóch osobnych tabel (zgodnie z kontraktem v2):
* `measurements`: Tabela przechowująca dane pomiarowe. Wymagane pola to `device_id`, `sensor`, `value`, `ts_ms`.
* `statuses`: Tabela przechowująca informacje o stanie urządzeń. Wymagane pola to `device_id`, `status`, `ts_ms`.

## 2. Działanie Ingestora
Serwis łączy się z brokerem MQTT i subskrybuje wzorzec topiców: `lab/+/+/+`.
Logika odbioru opiera się na funkcjach zwrotnych (callbacks):
* `on_connect`: nawiązuje subskrypcję po poprawnym połączeniu.
* `on_message`: dekoduje wiadomość JSON i przeprowadza walidację.

### 2.1 Proces Walidacji i Filtrowania (Kontrakt v2)
Ingestor weryfikuje pole `type` w celu skierowania danych do odpowiedniej tabeli:
1. Wiadomość z `type: "meas"` przechodzi przez funkcję walidacyjną `is_measurment_valid()` (sprawdzenie obecności `device_id`, `sensor`, `value`, `ts_ms`).
2. Wiadomość z `type: "status"` jest walidowana przez `is_status_valid()` (sprawdzenie `device_id`, `status`, `ts_ms`).

Wiadomości niezgodne z kontraktem są odrzucane z zapisem do logów (komunikat `Invalid payload`).

## 3. Instrukcja Uruchomienia i Weryfikacji

1. **Uruchomienie:** Środowisko uruchamiane jest poleceniem:
   `docker compose up -d --build`
2. **Podgląd Logów:** Logi Ingestora można monitorować na żywo za pomocą polecenia:
   `docker compose logs -f ingestor`
3. **Testowanie MQTT:** Narzędziem MQTT Explorer publikujemy przykładową wiadomość.
4. **Weryfikacja Zapisu:** Używając klienta bazy danych (np. psql lub wtyczki w VS Code), należy wejść do bazy i wykonać zapytanie np. `SELECT * FROM measurements ORDER BY id DESC;` aby potwierdzić zapis.