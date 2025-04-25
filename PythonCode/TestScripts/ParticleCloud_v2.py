import requests
import os
import json

# Ruta al archivo de configuración
config_path = os.path.expanduser("/home/jarain78/VisualCode/MachineBuilderContest/PythonCode/TestScripts/particle.config.json")

# Verificar que el archivo existe
if not os.path.exists(config_path):
    print("❌ El archivo de configuración no existe:", config_path)
    exit(1)

# Leer el archivo JSON
with open(config_path, "r") as f:
    config = json.load(f)

# Acceder a los datos
access_token = config.get("access_token")
username = config.get("username")
device_id = config.get("deviceID")
api_base_url = config.get("api_base_url")

# Mostrar resultados
print("✅ Configuración cargada:")
print(f"🔐 Access Token: {access_token}")
print(f"👤 Usuario: {username}")
print(f"📟 Device ID: {device_id}")
print(f"🌐 API Base URL: {api_base_url}")

headers = {"Authorization": f"Bearer {access_token}"} if access_token else {}

def get_device_info():
    """Fetch details of the device listed in the config file."""
    if not device_id:
        raise Exception("Device ID not found in config file.")
    url = f"{api_base_url}/devices/{device_id}"
    response = requests.get(url, headers=headers)
    return response.json()


print(get_device_info())


import requests
import threading
import json

class ParticleClient:
    def __init__(self, access_token, base_url="https://api.particle.io/v1", emulation_mode=False):
        self.base_url = base_url
        self.access_token = access_token
        self.headers = {
            "Authorization": f"Bearer {self.access_token}",
            "Accept": "application/json"
        }
        self.emulation_mode = emulation_mode

    def subscribe_event(self, event_name, callback):
        """Subscribe to a Particle event stream using a background thread."""

        if self.emulation_mode:
            print(f"⚠️ Modo emulación: se ignora la suscripción al evento '{event_name}'")
            return None

        url = f"{self.base_url}/devices/events/{event_name}"
        print(f"🔄 Suscribiéndose a: {url}")

        def event_listener():
            try:
                with requests.get(url, headers=self.headers, stream=True) as response:
                    if response.status_code != 200:
                        print(f"❌ Error al suscribirse: {response.status_code} - {response.text}")
                        return

                    for line in response.iter_lines():
                        if line:
                            try:
                                event_data = json.loads(line.decode("utf-8"))
                                callback(event_data)
                            except json.JSONDecodeError:
                                print(f"⚠️ No se pudo decodificar: {line}")
            except Exception as e:
                print(f"❌ Error en el hilo de suscripción: {e}")

        thread = threading.Thread(target=event_listener, daemon=True)
        thread.start()
        return thread


def handle_event(event):
    print("📩 Evento recibido:", event)

#client = ParticleClient(access_token=access_token, base_url=api_base_url, emulation_mode=False)
#client.subscribe_event("adc_block_", handle_event)
