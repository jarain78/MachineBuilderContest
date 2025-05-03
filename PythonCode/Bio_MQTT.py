import paho.mqtt.client as mqtt

# Configuración del broker (puede ser IP local o remoto)
BROKER = "iot.eclipse.org"  # o tu broker
PORT = 1883
TOPIC_SUB = "outTopic/message"
TOPIC_PUB = "inTopic/message"

# Callback cuando nos conectamos al broker
def on_connect(client, userdata, flags, rc):
    print("✅ Conectado al broker con código:", rc)
    client.subscribe(TOPIC_SUB)

# Callback cuando se recibe un mensaje
def on_message(client, userdata, msg):
    print(f"📩 Mensaje recibido en {msg.topic}: {msg.payload.decode()}")

# Crear cliente MQTT
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Conectar al broker y mantener loop
client.connect(BROKER, PORT, 60)
client.loop_start()

# Enviar comandos al dispositivo Particle
import time

try:
    while True:
        mensaje = input("Escribe un mensaje para el dispositivo Particle: ")
        client.publish(TOPIC_PUB, mensaje)
        print("📤 Mensaje enviado.")
        time.sleep(1)
except KeyboardInterrupt:
    print("👋 Desconectando...")
    client.loop_stop()
    client.disconnect()
