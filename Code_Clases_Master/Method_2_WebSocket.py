import asyncio
import websockets

async def communicate_with_unity():
    uri = "ws://127.0.0.1:8080"  # 👈 Replace this with your Unity WebSocket address and port
    async with websockets.connect(uri) as websocket:
        print("✅ Connected to Unity WebSocket server!")

        # Send a message to Unity
        message = "Hello from Python!"
        await websocket.send(message)
        print(f"📤 Sent: {message}")

        # Wait for response
        response = await websocket.recv()
        print(f"📩 Received from Unity: {response}")

# Run the client
asyncio.get_event_loop().run_until_complete(communicate_with_unity())
