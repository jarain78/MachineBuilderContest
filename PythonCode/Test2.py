import asyncio
from bleak import BleakScanner

async def scan_devices():
    print("Escaneando dispositivos BLE...")

    def callback(device, advertisement_data):
        print(f"Dispositivo encontrado: {device.name} [{device.address}]")
        
        # Revisar si hay datos de servicio disponibles
        if advertisement_data.service_data:
            for uuid, data in advertisement_data.service_data.items():
                print(f"Servicio UUID: {uuid}, Datos: {data.hex()}")

    # Escaneo con callback para obtener AdvertisementData
    scanner = BleakScanner(callback)
    await scanner.start()
    await asyncio.sleep(10)  # Escanear durante 10 segundos
    await scanner.stop()

asyncio.run(scan_devices())



import asyncio
from bleak import BleakScanner

async def scan_and_decode():
    def callback(device, advertisement_data):
        if "0000a201-0000-1000-8000-00805f9b34fb" in advertisement_data.service_data:
            data = advertisement_data.service_data["0000a201-0000-1000-8000-00805f9b34fb"]
            print(f"Servicio UUID: 0000a201-0000-1000-8000-00805f9b34fb")
            print(f"Datos en formato hexadecimal: {data.hex()}")

            # Decodificación del dato recibido
            decode_data(data)

    # Función para decodificar los datos
    def decode_data(data):
        # Interpretar como una lista de bytes enteros
        decoded_values = list(data)
        print("Lista de enteros:", decoded_values)

        # Interpretar como texto ASCII si es aplicable
        try:
            decoded_text = data.decode('utf-8')
            print("Texto decodificado:", decoded_text)
        except UnicodeDecodeError:
            print("No es un texto válido.")

        # Interpretar como números (por ejemplo, enteros de 16 bits)
        import struct
        if len(data) >= 2:
            value = struct.unpack('<H', data[:2])[0]  # Little-endian 16-bit integer
            print("Valor numérico (primeros 2 bytes):", value)

    # Escaneo de dispositivos con callback para capturar AdvertisementData
    scanner = BleakScanner(callback)
    await scanner.start()
    await asyncio.sleep(10)  # Escanear durante 10 segundos
    await scanner.stop()

asyncio.run(scan_and_decode())

