import socket

HOST = '127.0.0.1'  # Cambia si Unity corre en otra IP
PORT = 5000         # El mismo puerto del servidor Unity

client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((HOST, PORT))
print("✅ Conectado al servidor Unity")

try:
    while True:
        message = input("📝 Escribe un mensaje: ")
        client.sendall(message.encode('utf-8'))

        data = client.recv(1024)
        print(f"📩 Respuesta de Unity: {data.decode('utf-8')}")
except KeyboardInterrupt:
    print("👋 Cliente desconectado.")
    client.close()
