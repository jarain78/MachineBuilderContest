import asyncio
from bleak import BleakClient

device_address = "DC:23:4D:F2:FD:8E"  # Reemplace con la dirección MAC de su dispositivo
characteristic_uuid = "00002a05-0000-1000-8000-00805f9b34fb"

async def read_characteristic():
    async with BleakClient(device_address) as client:
        value = await client.read_gatt_char(characteristic_uuid)
        print(f"Valor leído de {characteristic_uuid}: {value}")

asyncio.run(read_characteristic())



'''characteristic_uuid = "00002b11-0000-1000-8000-00805f9b34fb"
data_to_write = bytearray([0x01, 0x02, 0x03])  # Datos a enviar

async def write_characteristic():
    async with BleakClient(device_address) as client:
        await client.write_gatt_char(characteristic_uuid, data_to_write)
        print(f"Datos escritos en {characteristic_uuid}: {data_to_write}")

asyncio.run(write_characteristic())'''


characteristic_uuid = "00002b10-0000-1000-8000-00805f9b34fb"

def notification_handler(sender, data):
    print(f"Notificación recibida de {sender}: {data}")

async def subscribe_notifications():
    async with BleakClient(device_address) as client:
        await client.start_notify(characteristic_uuid, notification_handler)
        print(f"Suscrito a notificaciones de {characteristic_uuid}")
        
        # Mantener la conexión durante un tiempo para recibir notificaciones
        await asyncio.sleep(30)

        await client.stop_notify(characteristic_uuid)
        print(f"Suscripción finalizada a {characteristic_uuid}")

asyncio.run(subscribe_notifications())

async def subscribe_indications():
    async with BleakClient(device_address) as client:
        await client.start_notify(characteristic_uuid, notification_handler)
        print(f"Suscrito a indicaciones de {characteristic_uuid}")
        
        await asyncio.sleep(30)  # Mantener la conexión
        await client.stop_notify(characteristic_uuid)

asyncio.run(subscribe_indications())
