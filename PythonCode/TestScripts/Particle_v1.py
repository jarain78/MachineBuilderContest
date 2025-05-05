import subprocess
import json
import signal
import matplotlib.pyplot as plt
from collections import defaultdict
import csv
from pathlib import Path

# Buffer para señales por tipo
signal_data = defaultdict(list)

# === Guardar datos por tipo en CSV ===
def save_signals_to_csv(data_by_type, output_dir="capturas_csv"):
    Path(output_dir).mkdir(parents=True, exist_ok=True)
    for signal, values in data_by_type.items():
        filename = Path(output_dir) / f"{signal}.csv"
        with open(filename, "w", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(["Sample", "Value"])
            for idx, val in enumerate(values):
                writer.writerow([idx, val])
        print(f"💾 Señal '{signal}' guardada en: {filename}")

# === Graficar señales ===
def plot_signals(data_by_type):
    plt.figure(figsize=(12, 6))
    for i, (signal, values) in enumerate(data_by_type.items(), 1):
        plt.subplot(len(data_by_type), 1, i)
        plt.plot(values, label=signal)
        plt.title(signal)
        plt.xlabel("Muestra")
        plt.ylabel("Valor")
        plt.grid(True)
        plt.legend()
    plt.tight_layout()
    plt.show()

# === Ctrl+C para terminar ===
def signal_handler(sig, frame):
    print("\n📊 Finalizando captura...")
    save_signals_to_csv(signal_data)
    plot_signals(signal_data)
    exit(0)

signal.signal(signal.SIGINT, signal_handler)

# === Suscribirse al stream de Particle ===
with subprocess.Popen(['./particle', 'subscribe'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True) as proc:
    print("📡 Suscribiéndose a eventos desde Particle... (Ctrl+C para detener)")
    for line in proc.stdout:
        line = line.strip()
        print("🔹", line)

        try:
            event = json.loads(line)
            if event.get("name") != "full_signal":
                continue

            raw = event["data"]  # ej: <DATA>:id=9;total=54;signal=PPG;payload=...
            if raw.startswith("<DATA>:"):
                parts = raw[7:].split(";")
                meta = {}
                for p in parts:
                    if "=" in p:
                        k, v = p.split("=", 1)
                        meta[k] = v

                signal_type = meta.get("signal", "unknown")
                payload = meta.get("payload", "")
                values = [int(v) for v in payload.split(",") if v.strip().isdigit()]
                signal_data[signal_type].extend(values)

        except json.JSONDecodeError:
            continue
