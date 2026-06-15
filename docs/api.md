# Dokumentacja REST API 

API zostało zbudowane w oparciu o framework Flask i udostępnia dane zapisane w bazie PostgreSQL poprzez zapytania HTTP GET. Aplikacja posiada wbudowaną obsługę błędów (zwracanie kodu 500 w przypadku awarii bazy). Wszystkie odpowiedzi zwracane są w formacie JSON.

## Dostępne Endpointy

### 1. Sprawdzenie statusu API
* **URL:** `/health`
* **Metoda:** `GET`
* **Odpowiedź:** `{"status": "ok"}`

### 2. Pobranie ostatnich pomiarów
* **URL:** `/measurements`
* **Metoda:** `GET`
* **Opis:** Zwraca listę 20 najnowszych pomiarów posortowanych malejąco po ID.

### 3. Pobranie najnowszego pomiaru
* **URL:** `/measurements/latest`
* **Metoda:** `GET`
* **Opis:** Zwraca pojedynczy, najnowszy wpis z bazy. W przypadku braku danych zwraca kod 404.

### 4. Historia pomiarów (z zaawansowanym filtrowaniem)
* **URL:** `/measurements/history`
* **Metoda:** `GET`
* **Parametry URL (opcjonalne):**
  * `device_id` (string) - filtruje po ID urządzenia.
  * `group_id` (string) - filtruje po identyfikatorze grupy.
  * `sensor` (string) - filtruje po typie sensora.
  * `limit` (integer) - ogranicza liczbę wyników (domyślnie 20).
* **Przykład zapytania:** `/measurements/history?device_id=esp32-01&group_id=g03&limit=5`

### 5. Lista dostępnych urządzeń
* **URL:** `/devices`
* **Metoda:** `GET`
* **Opis:** Zwraca unikalną listę identyfikatorów urządzeń (`device_id`), które zarejestrowały pomiary w bazie.

### 6. Lista dostępnych sensorów
* **URL:** `/sensors`
* **Metoda:** `GET`
* **Parametry URL (opcjonalne):**
  * `device_id` (string) - zawęża listę sensorów do konkretnego urządzenia.
* **Opis:** Zwraca unikalne pary `device_id` oraz `sensor` występujące w systemie.