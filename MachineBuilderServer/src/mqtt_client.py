import paho.mqtt.client as mqtt
import threading
from src.SignalReassembler import SignalReassembler

reassembler = SignalReassembler()

# Buffer para mensajes recibidos
received_messages = []
reassembled_signals = []


def on_connect(client, userdata, flags, rc):
    print("✅ Conectado MQTT:", rc)
    client.subscribe("particle/data")

def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    print(f"📩 MQTT recibido: {payload}")
    received_messages.append(payload)
    # Intentar reensamblar
    full_signal = reassembler.add_fragment(payload)
    if full_signal:
        print("✅ Señal reconstruida:", full_signal)
        reassembled_signals.append(full_signal)


def start_mqtt():
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect("broker.emqx.io", 1883, 60)
    thread = threading.Thread(target=client.loop_forever)
    thread.daemon = True
    thread.start()
    return client
