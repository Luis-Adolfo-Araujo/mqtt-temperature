import paho.mqtt.client as mqtt
import time
import random
import json

broker = "localhost"  # ou o IP do seu Raspberry Pi
port = 1883
topic = "sensor/temperatura"

client = mqtt.Client()
client.connect(broker, port)

while True:
    temperatura = round(random.uniform(18.0, 28.0), 2)

    payload = json.dumps({
        "temperatura": temperatura,
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
    })

    client.publish(topic, payload)
    print(f"Publicado: {payload}")
    time.sleep(5)  # publica a cada 5 segundos
