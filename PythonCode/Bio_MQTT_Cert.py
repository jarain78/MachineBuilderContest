import paho.mqtt.client as mqtt
import time

# Configuración del broker
BROKER = "broker.emqx.io"
PORT = 8883  # Puerto seguro TLS
TOPIC_SUB = "particle/data"
TOPIC_PUB = "particle/commands"

# Archivos de certificado (ajustá los paths a donde los tengas guardados)
CA_CERT = "PythonCode/certs/broker.emqx.io-ca.crt"
#CLIENT_CERT = "certs/client.crt"      # opcional
#CLIENT_KEY = "certs/client.key"       # opcional

# Callbacks
def on_connect(client, userdata, flags, rc):
    print("✅ Conectado al broker con código:", rc)
    client.subscribe(TOPIC_SUB)

def on_message(client, userdata, msg):
    print(f"📩 Mensaje recibido en {msg.topic}: {msg.payload.decode()}")

# Crear cliente
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Configurar TLS
client.tls_set(ca_certs=CA_CERT) #, certfile=CLIENT_CERT, keyfile=CLIENT_KEY)

# Conectar al broker
client.connect(BROKER, PORT, 60)
client.loop_start()

# Envío de mensajes
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
