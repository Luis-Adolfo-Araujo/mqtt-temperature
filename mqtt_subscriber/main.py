# mqtt_to_zabbix.py
import paho.mqtt.client as mqtt
import json
import os

def on_message(client, userdata, msg):
    data = json.loads(msg.payload.decode())
    temperature = data.get("temperatura")
    print(f"sending to zabbix: {data}")

    # Envia para o Zabbix via zabbix_sender
    os.system(f'zabbix_sender -z 127.0.0.1 -s "RaspberryPi" -k sensor.temp -o {temperature}')

client = mqtt.Client()
client.connect("localhost", 1883)
client.subscribe("sensor/temperatura")
client.on_message = on_message
client.loop_forever()
