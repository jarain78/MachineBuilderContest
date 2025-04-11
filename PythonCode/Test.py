import asyncio
from bleak import BleakClient


async def connect_and_get_services(address):
    async with BleakClient(address) as client:
        services = await client.get_services()
        for service in services:
            print(f"Servicio: {service.uuid}")
            for char in service.characteristics:
                print(f"  Característica: {char.uuid}, Propiedades: {char.properties}")

asyncio.run(connect_and_get_services("DC:23:4D:F2:FD:8E"))  # Reemplazar con la dirección del dispositivo

input("Presione Enter para continuar...")

import asyncio
from bleak import BleakScanner

async def scan_and_check_service():
    print("Escaneando dispositivos BLE...")
    devices = await BleakScanner.discover()

    for device in devices:
        print(f"Dispositivo encontrado: {device.name} [{device.address}]")

        # Verificar si el campo 'service_data' está disponible en los metadatos
        if 'service_data' in device.metadata:
            service_data = device.metadata['service_data']
            if service_data:
                for uuid, data in service_data.items():
                    print(f"Servicio UUID: {uuid}, Datos: {data.hex()}")
            else:
                print(f"El dispositivo {device.address} no tiene datos de servicio.")
        else:
            print(f"El dispositivo {device.address} no tiene 'service_data' en los metadatos.")

asyncio.run(scan_and_check_service())



import asyncio
from bleak import BleakScanner

async def scan_devices():
    devices = await BleakScanner.discover()
    for device in devices:
        print(f"Device: {device.name}, Address: {device.address}")

asyncio.run(scan_devices())


device_address = "DC:23:4D:F2:FD:8E"  # Reemplace con la dirección MAC de su dispositivo


import asyncio
from bleak import BleakScanner, BleakClient

service_uuid = "0000a201-0000-1000-8000-00805f9b34fb"  # UUID expandido de "0xA201"

async def scan_and_read():
    print("Escaneando dispositivos BLE...")
    devices = await BleakScanner.discover()
    
    for device in devices:
        if device.address == device_address:
            print(f"Dispositivo encontrado: {device.name} [{device.address}]")
            print(device.metadata)
            
            '''if service_uuid in device.metadata["service_data"]:
                service_data = device.metadata["service_data"][service_uuid]
                print(f"Datos del servicio {service_uuid}: {service_data.hex()}")'''

asyncio.run(scan_and_read())


# Datos recibidos en formato bytearray
data = bytearray(b'\xA2\x01\x10\x20')

# Convertir a lista de enteros
int_values = list(data)
print("Lista de enteros:", int_values)

# Interpretar como texto ASCII si es aplicable
try:
    decoded_text = data.decode('utf-8')
    print("Texto decodificado:", decoded_text)
except UnicodeDecodeError:
    print("No es un texto válido.")

# Interpretar como valores numéricos (ejemplo: enteros de 16 bits)
import struct
value = struct.unpack('<H', data[2:4])[0]  # Little-endian 16-bit integer
print("Valor numérico:", value)



'''async def get_device_characteristics(address):
    try:
        async with BleakClient(address) as client:
            print(f"Conectado a {address}")

            # Obtener los servicios y características del dispositivo
            services = await client.get_services()
            for service in services:
                print(f"Servicio: {service.uuid}")
                for characteristic in service.characteristics:
                    print(f"  Característica: {characteristic.uuid}")
                    print(f"    Propiedades: {characteristic.properties}")

            print("Exploración de características completada.")

    except Exception as e:
        print(f"Error al conectar con el dispositivo: {e}")

asyncio.run(get_device_characteristics(device_address))



characteristic_uuid = "00002a05-0000-1000-8000-00805f9b34fb"  # Reemplace con el UUID de la característica

async def read_characteristic():
    async with BleakClient(device_address) as client:
        value = await client.read_gatt_char(characteristic_uuid)
        print(f"Characteristic value: {value}")
        try:
            decoded_string = value.decode('utf-8')
            print(decoded_string)
        except UnicodeDecodeError:
            print("No se pudo decodificar como texto.")


asyncio.run(read_characteristic())


import asyncio
from bleak import BleakClient



async def get_uuids(address):
    async with BleakClient(address) as client:
        services = await client.get_services()
        print("Servicios y características encontradas:")

        service_uuids = []  # Lista para almacenar los UUIDs de los servicios
        characteristic_uuids = []  # Lista para almacenar los UUIDs de las características

        for service in services:
            print(f"Servicio: {service.uuid}")
            service_uuids.append(service.uuid)  # Agregar UUID del servicio a la lista

            for characteristic in service.characteristics:
                print(f"  Característica: {characteristic.uuid}")
                characteristic_uuids.append(characteristic.uuid)  # Agregar UUID de la característica

        return service_uuids, characteristic_uuids

# Ejecutar la función async
services_list, characteristics_list = asyncio.run(get_uuids(device_address))

print("\nLista de UUIDs de servicios:", services_list)
print("Lista de UUIDs de características:", characteristics_list)


async def read_characteristics(address, char_list):
    async with BleakClient(address) as client:
        for char in char_list:
            try:
                value = await client.read_gatt_char(char)
                print(f"Datos de {char}: {value}")
            except Exception as e:
                print(f"No se pudo leer {char}: {e}")

asyncio.run(read_characteristics(device_address, characteristics_list))


async def notification_handler(sender, data):
    print(f"Notificación recibida de {sender}: {data}")

async def subscribe_to_notifications(address, char_uuid):
    async with BleakClient(address) as client:
        await client.start_notify(char_uuid, notification_handler)
        print(f"Suscrito a notificaciones de {char_uuid}")
        await asyncio.sleep(30)  # Mantener conexión por 30 segundos para recibir datos
        await client.stop_notify(char_uuid)
        print(f"Suscripción finalizada de {char_uuid}")

# Suscribirse a la característica de notificación
asyncio.run(subscribe_to_notifications(device_address, '00002a05-0000-1000-8000-00805f9b34fb'))'''

