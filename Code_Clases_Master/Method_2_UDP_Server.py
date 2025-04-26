import socket

# Configurar IP y Puerto
SERVER_IP = "0.0.0.0"  # Escucha en todas las interfaces
SERVER_PORT = 5005

# Crear socket UDP
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((SERVER_IP, SERVER_PORT))

print(f"🟢 Servidor UDP escuchando en {SERVER_IP}:{SERVER_PORT}")

try:
    while True:
        data, addr = server_socket.recvfrom(1024)
        message = data.decode('utf-8')
        print(f"📩 Recibido de {addr}: {message}")

        # Opcional: responder al cliente
        response = "✅ Mensaje recibido en Python Server"
        server_socket.sendto(response.encode('utf-8'), addr)
except KeyboardInterrupt:
    print("🛑 Servidor detenido.")
finally:
    server_socket.close()
