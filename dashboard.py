import streamlit as st
import requests
import pandas as pd
from datetime import datetime

# Konfiguracja strony
st.set_page_config(page_title="RSP Dashboard", layout="wide")
st.title("Rozproszone Systemy Pomiarowe - Dashboard")

# --- OBSZAR 1: POŁĄCZENIE I KONFIGURACJA (Pasek boczny) ---
st.sidebar.header("Konfiguracja API")
api_url = st.sidebar.text_input("API URL", "http://localhost:5001")

# Testowanie połączenia z API na endpoint /health
if st.sidebar.button("Test API"):
    try:
        response = requests.get(f"{api_url}/health", timeout=3)
        if response.status_code == 200:
            st.sidebar.success("Status API: OK")
        else:
            st.sidebar.error(f"Status API: Błąd HTTP {response.status_code}")
    except requests.exceptions.RequestException:
        st.sidebar.error("Status API: Brak połączenia z backendem")

st.sidebar.markdown("---")

# --- OBSZAR 2: WYBÓR I STEROWANIE ---
st.sidebar.header("Filtry i konfiguracja")

# Pobranie listy urządzeń z endpointu /devices
device_list = ["esp32-test"] # Wartość domyślna
try:
    devices_res = requests.get(f"{api_url}/devices", timeout=2)
    if devices_res.status_code == 200:
        device_list = devices_res.json()
except:
    pass # Ignorujemy błąd, jeśli API nie działa

selected_device = st.sidebar.selectbox("Device ID", device_list)
selected_sensor = st.sidebar.text_input("Sensor", "temperature")
limit = st.sidebar.number_input("Limit rekordów", min_value=1, max_value=1000, value=50)

# --- OBSZAR 3: PREZENTACJA DANYCH ---
col1, col2 = st.columns([1, 2])

with col1:
    st.subheader("Ostatni pomiar (Latest)")
    if st.button("Pobierz latest"):
        try:
            # Pobieramy 1 najnowszy rekord dla wybranego urządzenia
            res = requests.get(f"{api_url}/measurements/history?device_id={selected_device}&sensor={selected_sensor}&limit=1")
            
            if res.status_code == 200 and len(res.json()) > 0:
                data = res.json()[0]
                
                # Wskaźniki
                st.metric("Device ID", data.get("device_id", "Brak danych"))
                st.metric("Sensor", data.get("sensor", "Brak danych"))
                st.metric("Wartość", f"{data.get('value', 'Brak')} {data.get('unit', '')}")
                
                # Czas
                if "ts_ms" in data:
                    dt = datetime.fromtimestamp(data["ts_ms"] / 1000.0).strftime('%Y-%m-%d %H:%M:%S')
                    st.write(f"**Czas pomiaru:** {dt}")
            else:
                st.warning("Brak danych dla wybranych filtrów.")
        except Exception as e:
            st.error("Błąd pobierania ostatniego pomiaru.")

with col2:
    st.subheader("Historia i Trend")
    if st.button("Pobierz historię"):
        try:
            params = {"device_id": selected_device, "sensor": selected_sensor, "limit": limit}
            res = requests.get(f"{api_url}/measurements/history", params=params)
            
            if res.status_code == 200:
                history_data = res.json()
                if history_data:
                    # Ładowanie do tabeli pandas
                    df = pd.DataFrame(history_data)
                    
                    if "ts_ms" in df.columns:
                        df['Czas'] = pd.to_datetime(df['ts_ms'], unit='ms')
                        df = df.sort_values(by='Czas')
                        df.set_index('Czas', inplace=True)
                    
                    # Wykres i tabela
                    st.line_chart(df['value'])
                    st.subheader("Ostatnie rekordy (Tabela)")
                    st.dataframe(df[['device_id', 'sensor', 'value', 'unit']])
                else:
                    st.info("Brak danych historycznych w bazie.")
            else:
                st.error(f"Błąd HTTP: {res.status_code}")
        except requests.exceptions.RequestException:
            st.error("Błąd połączenia z backendem podczas pobierania historii.")