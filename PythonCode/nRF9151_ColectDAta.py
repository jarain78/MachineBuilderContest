import serial
import matplotlib.pyplot as plt
import time
import datetime
import os

# ⚙️ CONFIGURACIÓN
PORT = "/dev/ttyACM0"   # Cambia esto por tu puerto COM, ej: "COM3" en Windows
BAUDRATE = 115200
TIMEOUT = 1.0           # segundos
OUTPUT_DIR = "adc_data"
os.makedirs(OUTPUT_DIR, exist_ok=True)

# 📥 FUNCIÓN PARA ESCUCHAR Y GUARDAR
def leer_adc_desde_serial():
    samples = []
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")

    print(f"⏳ Esperando datos desde {PORT}...")

    with serial.Serial(PORT, BAUDRATE) as ser:
        capture = False

        while True:
            line = ser.readline().decode(errors="ignore").strip()                    
            print(line)
            
            if not line:
                continue

            if "Enviando captura" in line:
                print("📡 Inicio de captura detectado...")
                samples.clear()
                capture = True

            elif "Captura finalizada" in line:
                print(f"✅ Captura finalizada. Total: {len(samples)} muestras.")
                break

            elif capture:
                try:
                    print(line)
                    value = int(line)
                    samples.append(value)
                except ValueError:
                    print(f"⚠️ Ignorado: {line}")

        print(samples)

        # Guardar en CSV
        filename = os.path.join(OUTPUT_DIR, f"adc_capture_{timestamp}.csv")
        with open(filename, "w") as f:
            for sample in samples:
                f.write(f"{sample}\n")
        print(f"📁 Datos guardados en {filename}")
    return samples

# 📊 FUNCIÓN PARA GRAFICAR
def graficar_datos(samples):
    plt.figure(figsize=(12, 4))
    plt.plot(samples, color='royalblue', linewidth=1)
    plt.title("Señal ADC capturada")
    plt.xlabel("Muestra")
    plt.ylabel("Valor ADC")
    plt.grid(True)
    plt.tight_layout()
    plt.show()

# 🚀 MAIN
if __name__ == "__main__":
    while True:
        datos = leer_adc_desde_serial()
        if datos:
            graficar_datos(datos)

        print("⏱ Esperando nueva captura en el dispositivo...\n")
        time.sleep(3)
