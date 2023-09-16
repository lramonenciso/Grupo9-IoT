
//Librerias
#include <ESP8266WiFi.h>
#include "src/iotc/common/string_buffer.h"
#include "src/iotc/iotc.h"
#include "DHT.h"

#define DHTPIN 2 //Pin del sensor DHT22

#define DHTTYPE DHT22   // Tipo de sensor DHT 22

//Credenciales de la red wifi
#define WIFI_SSID "CDT"
#define WIFI_PASSWORD "C3d3t32023*"
//Credenciales para la conexión remota con Azure IoT Central

const char* SCOPE_ID = "0ne00AA53BE";
const char* DEVICE_ID = "26szlkf5a7e";
const char* DEVICE_KEY = "PhYNo7EanDstkCaXW5FMj02Gx64kG4ULeB2tkMtyWV8=";

int soilMoisturePin = A0; //Pin del sensor de suelo FC-28
int soilMoisture = 0; //Inicializa en  0

DHT dht(DHTPIN, DHTTYPE);// Inicialización del sensor DHT

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo);
#include "src/connection.h"

void on_event(IOTContext ctx, IOTCallbackInfo* callbackInfo) {
  // Estado de conexión
  if (strcmp(callbackInfo->eventName, "ConnectionStatus") == 0) {
    LOG_VERBOSE("Is connected ? %s (%d)",
                callbackInfo->statusCode == IOTC_CONNECTION_OK ? "YES" : "NO",
                callbackInfo->statusCode);
    isConnected = callbackInfo->statusCode == IOTC_CONNECTION_OK;
    return;
  }
  // Manipulación de datos recibidos
  // El búfer de carga útil no tiene un final nulo.
  // Agrega terminación nula en otro buffer antes de imprimir
  AzureIOT::StringBuffer buffer;
  if (callbackInfo->payloadLength > 0) {
    buffer.initialize(callbackInfo->payload, callbackInfo->payloadLength);
  }

  LOG_VERBOSE("- [%s] evento recibido. Payload => %s\n",
              callbackInfo->eventName, buffer.getLength() ? *buffer : "EMPTY");

  if (strcmp(callbackInfo->eventName, "Command") == 0) {
    LOG_VERBOSE("- Nombre del comando => %s\r\n", callbackInfo->tag);
  }
}

void setup() {
  Serial.begin(9600);
  // Conexión WiFi y al servicio en la nube
  connect_wifi(WIFI_SSID, WIFI_PASSWORD);
  connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY);

  if (context != NULL) {
    lastTick = 0;  /// Configurar el temporizador en el pasado para habilitar la primera telemetría lo antes posible
  }
  dht.begin();// Inicializar el sensor DHT
}

void loop() {
  // Lectura de datos del sensor DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Lectura de datos del sensor humedad del suelo.
  soilMoisture = analogRead(soilMoisturePin);

  //Ajustar los valores de lectura a la humedad del suelo según el sensor.
  float soilHumidity = map(soilMoisture, 0, 1023, 0, 100);


  if (isConnected) {

    unsigned long ms = millis();
    if (ms - lastTick > 10000) {  // envía telemetría cada 10 segundos
      char msg[64] = {0};
      int pos = 0, errorCode = 0;

      lastTick = ms;
      if (loopId++ % 2 == 0) {  // envia telemetria
        pos = snprintf(msg, sizeof(msg) - 1, "{\"Temperatura\": %f}",
                       t);//Temperatura
        errorCode = iotc_send_telemetry(context, msg, pos);

        pos = snprintf(msg, sizeof(msg) - 1, "{\"Humedad\":%f}",
                       h);//Humedad
        errorCode = iotc_send_telemetry(context, msg, pos);

        pos = snprintf(msg, sizeof(msg) - 1, "{\"HumedadSuelo\":%f}",
                       soilHumidity);//Humedad del suelo
        errorCode = iotc_send_telemetry(context, msg, pos);

      } else {   // Enviar propiedad (si es necesario)

      }

      msg[pos] = 0;

      if (errorCode != 0) {
        LOG_ERROR("Fallo al enviar el mensaje con el codigo de error %d", errorCode);
      }
    }

    iotc_do_work(context);  // Realizar trabajo en segundo plano para iotc
  } else {
    iotc_free_context(context);
    context = NULL;
    connect_client(SCOPE_ID, DEVICE_ID, DEVICE_KEY); // Reconectar si no está conectado.
  }

}
