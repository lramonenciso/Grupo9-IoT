#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// Configuración de la red WiFi
const char *ssid = "CDT";
const char *password = "C3d3t32023*";

// Pin del sensor DHT22
#define DHTPIN 2
DHT dht(DHTPIN, DHT22);

// Pin analógico al que está conectado el sensor de suelo FC-28
const int SOILPIN = A0;

ESP8266WebServer server(80);

void setup() {
  // Inicializa el sensor DHT22
  dht.begin();

  // Inicializa la comunicación serial
  Serial.begin(115200);

  // Conéctate a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }
  Serial.println("Conexión exitosa");
  Serial.println(WiFi.localIP());
  // Rutas para la página web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/gdd", HTTP_GET, handleGDD);

  // Inicia el servidor web
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  // Leer la temperatura y la humedad del sensor DHT22
  float tempDHT = dht.readTemperature();
  float humDHT = dht.readHumidity();

  // Leer la humedad del suelo del sensor FC-28
  int valorSuelo = analogRead(SOILPIN);
  // Convierte el valor en un porcentaje de humedad (ajusta según tus calibraciones)
  float humedadSuelo = map(valorSuelo, 0, 1023, 0, 100);

  String html = "<html><body>";
  html += "<h1>Datos del sensor DHT22</h1>";
  html += "<p>Temperatura: " + String(tempDHT) + " &#8451;</p>";
  html += "<p>Humedad: " + String(humDHT) + " %</p>";
  html += "<h1>Datos del sensor de suelo FC-28</h1>";
  html += "<p>Humedad del suelo: " + String(humedadSuelo) + " %</p>";
  html += "<a href='/gdd'>Calcular GDD</a>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleGDD() {
  // Leer la temperatura y la humedad del sensor DHT22
  float tempDHT = dht.readTemperature();
  float humDHT = dht.readHumidity();

  // Calcular el GDD
  float gdd = (tempDHT + 15.0) / 2.0;

  String html = "<html><body>";
  html += "<h1>Grados Días de Desarrollo (GDD)</h1>";
  html += "<p>Temperatura del DHT22: " + String(tempDHT) + " &#8451;</p>";
  html += "<p>Humedad del DHT22: " + String(humDHT) + " %</p>";
  html += "<p>GDD calculado: " + String(gdd) + " &#8451;</p>";
  html += "<a href='/'>Volver</a>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}
