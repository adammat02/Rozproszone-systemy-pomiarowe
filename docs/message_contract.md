# Kontrakt danych - Projekt Rozproszone Systemy Pomiarowe

## 1. Opis struktury topiców MQTT
Wiadomości są publikowane według schematu pozwalającego na rozróżnienie wielu czujników na jednym urządzeniu:
`lab/<group_id>/<device_id>/<sensor_type>`

**Przykłady:**
* `lab/g03/esp32-01/temperature`
* `lab/g03/esp32-01/humidity`

## 2. Opis wiadomości JSON 
Poniżej znajduje się przykład poprawnej wiadomości pomiarowej:

```json
{
  "schema_version": 1,
  "group_id": "g02",
  "device_id": "esp32-01",
  "sensor": "temperature",
  "value": 24.5,
  "unit": "C",
  "ts_ms": 1742030400000,
  "seq": 1
}
```

## 3. Lista pól wymaganych
* device_id, typ: string
* sensor, typ: string
* value, typ: number
* ts_ms, typ: integer, >0


## 4. Lista pól opcjonalnych 
* schema_version, typ: integer
* group_id, typ: string
* unit, typ: string
* seq, typ: string, $\ge 0$


## 5. Przykłady wiadomości błędnych
Przedstawiono przykłady błędnej wiadomości pomiarowej.

1.  W tej wiadomości brakuje pola ts_ms, które jest kluczowe w określeniu dokładnego czasu pomiaru. 

```json

{
  "device_id": "esp32-01",
  "sensor": "temperature",
  "value": 24.5
}
```
2. W tej wiadomości wartość 24.5 zapisana jest jako string, a nie jako number, co uniemoliwia poprawny odczyt.
```json

{
  "device_id": "esp32-01",
  "sensor": "temperature",
  "value": "24.5",
  "ts_ms": 1742030400000
}
```
3.  W tej wiadomości brak pola device_id uniemożliwia przypisanie pomiaru do źródła.
```json
{
  "sensor": "temperature",
  "value": 24.5,
  "ts_ms": 1742030400000
}
```


