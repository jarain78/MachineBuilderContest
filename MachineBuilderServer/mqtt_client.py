import paho.mqtt.client as mqtt
import threading

# Buffer para mensajes recibidos
received_messages = []

def on_connect(client, userdata, flags, rc):
    print("✅ Conectado MQTT:", rc)
    client.subscribe("particle/data")

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    print(f"📩 MQTT recibido: {payload}")
    received_messages.append(payload)

def start_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("broker.emqx.io", 1883, 60)
    thread = threading.Thread(target=client.loop_forever)
    thread.daemon = True
    thread.start()
    return client
