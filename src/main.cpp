#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include <WiFi.h>

#define ENG 16

AsyncWebServer server(80);

const char* ssid = "ESP32";
const char* password = "unodostres";

bool isActiveEngine = false;
bool isConnected = false;

void blink(int speed)
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(speed);
  digitalWrite(LED_BUILTIN, LOW);
  delay(speed);
}

float handleSensor(int PIN)
{
  float value = analogRead(PIN);

  return value;
}

void handleEngineHTTP()
{
  server.on("/engine/on", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    Serial.println(F("[ENGINE] [ON]"));
    digitalWrite(ENG, LOW);
    request->send(200);
  });

  server.on("/engine/off", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    Serial.println(F("[ENGINE] [OFF]"));
    digitalWrite(ENG, LOW);
    request->send(200);
  });
}

void handleSensorHTTP()
{
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    int PIN = request->getParam("pin")->value().toInt();
    Serial.print(F("[SENSOR] [READ] "));
    Serial.println(PIN);

    request->send(200, "text/plain", String(handleSensor(PIN)));
  });
}

void initWiFi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void WiFiEventHandler(WiFiEvent_t event)
{
  switch (event) 
  {
    case SYSTEM_EVENT_STA_GOT_IP:
      isConnected = true;
      Serial.print(F("[INFO] [ADDRESS] "));
      Serial.println(WiFi.localIP());
      handleEngineHTTP();
      handleSensorHTTP();
      server.begin();
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      isConnected = false;
      initWiFi();
      break;

    default:
      break;
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.onEvent(WiFiEventHandler);

  Serial.println(F("Sensor Reader"));
  Serial.println();

  pinMode(ENG, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  initWiFi();
}

void loop()
{
  if (isConnected)
  {
    digitalWrite(LED_BUILTIN, HIGH);
  }

  else
  {
    blink(800);
  }
}
