import math
import time
import threading
from flask import Flask, render_template_string, jsonify

app = Flask(__name__)

state = {
    "hour": "00:00",
    "amb_temp": 22.0,
    "compost_temp": 58.0,
    "teg_power": 3.89,
    "solar_power": 0.0,
    "load_power": 35.0,
    "soc": 32.0,
    "bus_voltage": 11.92,
    "grid_active": False,
    "history": []
}

def raspberry_pi_loop():
    soc = 32.0
    cap_wh = 1200.0
    grid = False
    hour = 0

    while True:
        amb = 24.0 + 10.0 * math.sin(math.pi * (hour - 8) / 12.0) if 6 <= hour <= 20 else 20.0 + 2.0 * math.cos(math.pi * hour / 12.0)
        comp = 58.0 + 3.0 * math.sin(math.pi * hour / 12.0)
        dt = comp - amb
        teg = round(0.018 * (dt ** 1.5), 2)
        irr = 920.0 * math.sin(math.pi * (hour - 6) / 12.0) if 6 <= hour <= 18 else 0.0
        sol = round((irr / 1000.0) * 200.0 * 0.88, 2)
        load = 95.0 if 10 <= hour <= 16 else (75.0 if 18 <= hour <= 22 else 35.0)

        net_wh = (teg + sol - load) if not grid else (teg + sol + 60.0 - load)
        soc = max(15.0, min(100.0, round(soc + (net_wh / cap_wh) * 100.0, 1)))

        if soc <= 25.0:
            grid = True
        elif soc >= 45.0:
            grid = False

        v_bus = round(11.5 + (soc / 100.0) * 1.3, 2)
        timestamp = f"{hour:02d}:00"

        state.update({
            "hour": timestamp,
            "amb_temp": round(amb, 1),
            "compost_temp": round(comp, 1),
            "teg_power": teg,
            "solar_power": sol,
            "load_power": load,
            "soc": soc,
            "bus_voltage": v_bus,
            "grid_active": grid
        })

        state["history"].append({
            "hour": timestamp,
            "teg": teg,
            "sol": sol,
            "load": load,
            "soc": soc
        })
        if len(state["history"]) > 24:
            state["history"].pop(0)

        hour = (hour + 1) % 24
        time.sleep(1.5)

threading.Thread(target=raspberry_pi_loop, daemon=True).start()

@app.route('/api/telemetry')
def telemetry():
    return jsonify(state)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
