from flask import Flask, jsonify
from db import get_connection

app = Flask(__name__)

# Endpoint kontrolny 
@app.route("/health", methods=["GET"])
def health():
    return jsonify({"status": "ok"})

# Endpoint pobierający dane z bazy danych
@app.route("/measurements", methods=["GET"])
def get_measurements():
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
            "id": row[0],
            "group_id": row[1],
            "device_id": row[2],
            "sensor": row[3],
            "value": row[4],
            "unit": row[5],
            "ts_ms": row[6],
            "seq": row[7],
            "topic": row[8]
        })

    return jsonify(result)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5001)
