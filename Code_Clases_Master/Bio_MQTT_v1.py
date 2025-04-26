import paho.mqtt.client as mqtt
import time

# Configuración del broker
BROKER = "broker.emqx.io"  # Puedes usar tu propio broker
PORT = 1883
TOPIC_SUB = "outTopic/message"
TOPIC_PUB = "inTopic/message"

# Callback cuando se conecta
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

# Conectar y mantener el loop
client.connect(BROKER, PORT, 60)
client.loop_start()

# Publicar datos automáticamente cada 1 segundo
contador = 0

try:
    while True:
        mensaje = f"Mensaje automático #{contador}"
        client.publish(TOPIC_PUB, mensaje)
        print(f"📤 Publicado: {mensaje}")
        contador += 1
        time.sleep(1)
except KeyboardInterrupt:
    print("👋 Desconectando...")
    client.loop_stop()
    client.disconnect()
