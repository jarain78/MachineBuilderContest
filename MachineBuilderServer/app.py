from flask import Flask, render_template, request, jsonify
from mqtt_client import start_mqtt, received_messages

app = Flask(__name__)
mqtt_client = start_mqtt()

@app.route("/")
def index():
    return render_template("index.html")

@app.route("/send", methods=["POST"])
def send():
    mensaje = request.form.get("mensaje")
    mqtt_client.publish("particle/commands", mensaje)
    return "Mensaje enviado"

@app.route("/data")
def data():
    return jsonify(received_messages[-100:])  # últimos 100 datos

if __name__ == "__main__":
    app.run(debug=True)
