#include <NTPClient.h>
#include <WiFiUdp.h>

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000); // -10800 = UTC-3
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// Credenciais da sua rede Wi-Fi
const char* ssid = "CLARO_2GFA669D";
const char* password = "38FA669D";

// Endereço IP do seu broker MQTT
const char* mqtt_server = "192.168.0.122";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Conectando-se a ");
  Serial.println(ssid);

  Serial.println("Aguardando conexão WiFi...");
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFalha ao conectar no WiFi!");
  } else {
    Serial.println("\nConectado com sucesso!");
  }
  IPAddress local_ip(192, 168, 0, 150);  // IP fixo para o D1 Mini
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(local_ip, gateway, subnet);
  WiFi.begin(ssid, password);

  // Espera conectar
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop até reconectar
  while (!client.connected()) {
    Serial.print("Tentando conectar ao MQTT...");
    if (client.connect("D1MiniClient")) {
      Serial.println("Conectado!");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  timeClient.begin();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leitura do sensor LM35 (ligado no A0)
  int analogValue = analogRead(A0);
  float voltage = analogValue * (3.3 / 1023.0);
  float temperatureC = voltage * 100.0;

  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println(" °C");

  // Obter timestamp (como string simples)
  timeClient.update();
  String timestamp = timeClient.getFormattedTime(); // Formato: HH:MM:SS

  // Montar JSON
  StaticJsonDocument<200> doc;
  doc["temperatura"] = temperatureC;
  doc["timestamp"] = timestamp;

  char payload[256];
  serializeJson(doc, payload);

  // Publicar via MQTT
  client.publish("sensor/temperatura", payload);
  Serial.print("Publicado: ");
  Serial.println(payload);

  delay(5000); // a cada 5 segundos
}