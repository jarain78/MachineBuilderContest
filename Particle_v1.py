import os

particle_dir = os.path.expanduser("~/.particle")

if os.path.exists(particle_dir):
    print(os.listdir(particle_dir))
else:
    print("⚠️ La carpeta ~/.particle no existe. ¿Has iniciado sesión con Particle CLI?")

import os

# Expande la ruta ~ a /home/usuario o C:\Users\Usuario
particle_dir = os.path.expanduser("~/.particle")

# Crea la carpeta si no existe
os.makedirs(particle_dir, exist_ok=True)

print(f"✅ Carpeta creada en: {particle_dir}")


CONFIG_PATH = os.getenv("CONFIG_PATH", "~/.particle/particle.config.json")
print(CONFIG_PATH)


from particle_linux import ParticleLinuxSDK

sdk = ParticleLinuxSDK()