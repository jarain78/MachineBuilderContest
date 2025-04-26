import socket
import threading

# Configuración
HOST = '127.0.0.1'  # Escucha en todas las interfaces
PORT = 5000       # Debe ser el mismo puerto que configures en Unity

# Función para manejar a cada cliente
def handle_client(conn, addr):
    print(f"🔵 Nueva conexión desde {addr}")

    while True:
        try:
            data = conn.recv(1024)
            if not data:
                break  # Se desconectó
            message = data.decode('utf-8')
            print(f"📩 Recibido de {addr}: {message}")

            # Opcional: enviar una respuesta
            response = f"Servidor recibió: {message}"
            conn.sendall(response.encode('utf-8'))
            print(f"📤 Enviado a {addr}: {response}")

        except ConnectionResetError:
            break  # El cliente se desconectó bruscamente

    conn.close()
    print(f"🔴 Conexión cerrada con {addr}")

# Crear socket
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((HOST, PORT))
server.listen()

print(f"🟢 Servidor escuchando en {HOST}:{PORT}")

# Aceptar conexiones
while True:
    conn, addr = server.accept()
    thread = threading.Thread(target=handle_client, args=(conn, addr))
    thread.start()
    print(f"🧵 Número de clientes conectados: {threading.active_count() - 1}")
