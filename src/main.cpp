#include <ESPAsyncWebServer.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Arduino.h>
#include <WiFi.h>

#define ONE_WIRE_BUS 2
#define ENA 15
#define IN1 16
#define IN2 17

AsyncWebServer server(80);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "HUAWEI-0617AJ";
const char* password = "rgdrcVwrBcLf9tuMpF";

bool isConnected = false;
int mappedSpeed = 0;

void blink(int speed)
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(speed);
  digitalWrite(LED_BUILTIN, LOW);
  delay(speed);
}

float handleSensor(int PIN)
{
  sensors.requestTemperatures();
  return sensors.getTempFByIndex(0);
}

void handleEngine(bool state, int speed)
{
  if (state)
  {
    digitalWrite(ENA, HIGH);
    analogWrite(ENA, speed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }

  else
  {
    digitalWrite(ENA, LOW);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
}

void handleEngineHTTP() {
  server.on("/engine/on", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    Serial.println(F("[ENGINE] [ON]"));
    handleEngine(true, mappedSpeed);
    AsyncWebServerResponse *response = request->beginResponse(200);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/engine/off", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    Serial.println(F("[ENGINE] [OFF]"));
    handleEngine(false, 0);
    AsyncWebServerResponse *response = request->beginResponse(200);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.on("/engine/speed", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    if (request->hasParam("speed", true))
    {
      int speed = request->getParam("speed", true)->value().toInt();
      Serial.print(F("[ENGINE] [SPEED] "));
      Serial.println(speed);
      mappedSpeed = map(speed, 0, 100, 0, 255);
      AsyncWebServerResponse *response = request->beginResponse(200);
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    }

    else
    {
      request->send(400, "text/plain", "Missing speed parameter");
    }
  });
}

void handleSensorHTTP()
{
  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if (request->hasParam("pin")) {
      int PIN = request->getParam("pin")->value().toInt();
      Serial.print(F("[SENSOR] [READ] "));
      Serial.println(PIN);
      String sensorValue = String(handleSensor(PIN));
      AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", sensorValue);
      response->addHeader("Access-Control-Allow-Origin", "*");
      request->send(response);
    }

    else
    {
      request->send(400, "text/plain", "Missing pin parameter");
    }
  });
}

void addCORSHeaders(AsyncWebServerRequest *request)
{
  if (request->method() == HTTP_OPTIONS)
  {
    AsyncWebServerResponse *response = request->beginResponse(204);
    response->addHeader("Access-Control-Allow-Origin", "*");
    response->addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    response->addHeader("Access-Control-Allow-Headers", "Content-Type");
    request->send(response);
  }
}

void initWiFi()
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void WiFiEventHandler(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      isConnected = true;
      Serial.print(F("[INFO] [ADDRESS] "));
      Serial.println(WiFi.localIP());
      handleEngineHTTP();
      handleSensorHTTP();
      server.onNotFound([](AsyncWebServerRequest *request)
      {
        addCORSHeaders(request);
        request->send(404, "text/plain", "Not found");
      });
      server.on("/", HTTP_OPTIONS, addCORSHeaders);
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

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

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
