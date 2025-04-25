import requests
import json

# Dirección del servidor Unity
UNITY_SERVER = "http://localhost:8080"

def get_angles():
    """Solicita los ángulos actuales del robot desde Unity."""
    try:
        response = requests.get(f"{UNITY_SERVER}/angles")
        if response.status_code == 200:
            data = response.json()
            angles = data.get('angles', [])
            print("✅ Ángulos actuales del robot:", angles)
            return angles
        else:
            print(f"❌ Error al obtener ángulos: {response.status_code}")
    except Exception as e:
        print(f"❌ Excepción: {e}")

def set_angles(new_angles):
    """Envía nuevos ángulos al robot en Unity."""
    try:
        payload = {
            "angles": new_angles
        }
        headers = {"Content-Type": "application/json"}
        response = requests.post(f"{UNITY_SERVER}/set_angles", data=json.dumps(payload), headers=headers)

        if response.status_code == 200:
            print("✅ Ángulos enviados correctamente.")
        else:
            print(f"❌ Error al enviar ángulos: {response.status_code}")
    except Exception as e:
        print(f"❌ Excepción: {e}")

# ==== Ejemplo de uso ====

if __name__ == "__main__":
    # 1. Leer los ángulos actuales
    current_angles = get_angles()

    # 2. Modificar algunos ángulos y enviarlos de vuelta
    if current_angles:
        new_angles = [0, -15, 0, 5]  # Sumar +10 a cada ángulo
        set_angles(new_angles)

    # 3. Leer de nuevo para confirmar
    get_angles()


import requests
import json

angles = [90.0, -20.0, -15.0, 5.5]
data = {'angles': angles}
res = requests.post(f'{UNITY_SERVER}/angles', json=data)
print(res.json())

res = requests.get(f'{UNITY_SERVER}/angles')
print(res.json())

img1 = requests.get(f'{UNITY_SERVER}/camera1').content
img2 = requests.get(f'{UNITY_SERVER}/camera2').content

with open('camera1.jpg', 'wb') as f:
    f.write(img1)

with open('camera2.jpg', 'wb') as f:
    f.write(img2)
