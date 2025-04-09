import requests
import json
import time
import os
import matplotlib.pyplot as plt


#https://api.particle.io/v1/devices/?access_token=583a73b881049cb24601d606b2485fe9686825bb

'''{
  "token_type": "bearer",
  "access_token": "c4f1d6ce27419f7afa9ad60982192f0b5a215cb7",
  "expires_in": 28800,
  "refresh_token": "8593b8318f2bd1fea5ef9759017e31336e08b8c4"
}'''


# === CONFIGURACIÓN ===
access_token = "91c95fc84de9e2db60992ed265a935d2a7071605"
device_id = "bio-device-2936"  # ID del dispositivo Particle
event_prefix = "adc_block_"  # Los eventos tienen nombres como adc_block_1, adc_block_2...
done_event = "adc_done"
output_csv = "reconstruccion_adc.csv"

# === ALMACENAMIENTO TEMPORAL ===
blocks = {}
done_received = False

# === API URL ===
url = f"https://api.particle.io/v1/devices/?access_token={access_token}" #"https://api.particle.io/v1/devices/{device_id}/events"

print("⏳ Esperando eventos...")

with requests.get(url, stream=True, params={"access_token": access_token}) as response:
    if response.status_code != 200:
        print("❌ Error al conectar con Particle:", response.status_code)
        exit(1)

    for line in response.iter_lines():
        if line:
            try:
                decoded = json.loads(line.decode('utf-8'))
                event = decoded["event"]
                data_raw = decoded["data"]

                if event.startswith(event_prefix):
                    payload = json.loads(data_raw)
                    block_id = payload["id"]
                    block_data = payload["data"]
                    blocks[block_id] = block_data
                    print(f"📥 Recibido bloque {block_id}")

                elif event == done_event:
                    done_received = True
                    print("✅ Evento final recibido. Reconstruyendo CSV...")
                    break

            except Exception as e:
                print("⚠️ Error al procesar línea:", e)

# === RECONSTRUCCIÓN CSV + GRAFICADO ===
if done_received and blocks:
    sorted_blocks = sorted(blocks.items(), key=lambda x: x[0])
    all_samples = []

    for _, block in sorted_blocks:
        samples = block.split(",")
        all_samples.extend([int(s) for s in samples])

    # Guardar en CSV
    with open(output_csv, "w") as f:
        f.write("adc_value\n")
        for s in all_samples:
            f.write(f"{s}\n")

    print(f"✅ Archivo CSV creado: {output_csv}")

    # === GRAFICAR ===
    plt.figure(figsize=(12, 5))
    plt.plot(all_samples, color='blue')
    plt.title("Señal capturada del ADC")
    plt.xlabel("Muestra")
    plt.ylabel("Valor ADC")
    plt.grid(True)
    plt.tight_layout()
    plt.show()

else:
    print("⚠️ No se recibieron bloques completos o evento final.")


 
 
