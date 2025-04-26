import socket
import time

# Configura la IP y el puerto de Unity
UNITY_IP = "127.0.0.1"  # Cambia si Unity está en otra máquina
UNITY_PORT = 5005

# Crear socket UDP
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    while True:
        message = "👋 Hola Unity desde Python!"
        client_socket.sendto(message.encode('utf-8'), (UNITY_IP, UNITY_PORT))
        print(f"📤 Enviado: {message}")

        # Esperar respuesta
        try:
            client_socket.settimeout(2)
            data, server = client_socket.recvfrom(1024)
            print(f"📩 Respuesta de Unity: {data.decode('utf-8')}")
        except socket.timeout:
            print("⏳ No hay respuesta del servidor (timeout)")

        time.sleep(1)  # Esperar 1 segundo antes de enviar otro
except KeyboardInterrupt:
    print("👋 Cliente detenido.")
finally:
    client_socket.close()
