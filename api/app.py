from flask import Flask, jsonify
from db import get_connection

app = Flask(__name__)

# 1. Endpoint /health 
@app.route("/health", methods=["GET"])
def health():
    return jsonify({"status": "ok"})

# 2. Endpoint /measurements
@app.route("/measurements", methods=["GET"])
def get_measurements():
    try:
        conn = get_connection()
        cur = conn.cursor()
        cur.execute("""
            SELECT id, group_id, device_id, sensor, value, unit, ts_ms, seq, topic
            FROM measurements
            ORDER BY id DESC
            LIMIT 20
        """)
        rows = cur.fetchall()
        cur.close()
        conn.close()

        result = []
        for row in rows:
            result.append({
                "id": row[0], "group_id": row[1], "device_id": row[2],
                "sensor": row[3], "value": row[4], "unit": row[5],
                "ts_ms": row[6], "seq": row[7], "topic": row[8]
            })
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

# 3. Endpoint /measurements/latest 
@app.route("/measurements/latest", methods=["GET"])
def get_latest_measurement():
    try:
        conn = get_connection()
        cur = conn.cursor()
        cur.execute("""
            SELECT id, group_id, device_id, sensor, value, unit, ts_ms, seq, topic
            FROM measurements
            ORDER BY id DESC
            LIMIT 1
        """)
        row = cur.fetchone()
        cur.close()
        conn.close()

        if row is None:
            return jsonify({"message": "Brak danych"}), 404

        return jsonify({
            "id": row[0], "group_id": row[1], "device_id": row[2],
            "sensor": row[3], "value": row[4], "unit": row[5],
            "ts_ms": row[6], "seq": row[7], "topic": row[8]
        })
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    # Uruchamiamy na porcie 5001 tak jak w instrukcji
    app.run(debug=True, host='0.0.0.0', port=5001)
