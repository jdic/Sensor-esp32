#include <ESPAsyncWebServer.h>
#include <Arduino.h>
#include <WiFi.h>

#define ENA 15
#define IN1 5
#define IN2 4

AsyncWebServer server(80);

const char* ssid = "HUAWEI-0617AJ";
const char* password = "rgdrcVwrBcLf9tuMpF";

bool isActiveEngine = false;
bool isConnected = false;
int mappedSpeed = 0;

void blink(int speed)
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(speed);
  digitalWrite(LED_BUILTIN, LOW);
  delay(speed);
}

void handleSpeed(int speed)
{
  mappedSpeed = map(speed, 0, 100, 0, 255);
}

void handleEngineHTTP()
{
  server.on("/engine/on", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    Serial.println(F("[ENGINE] [ON]"));
    isActiveEngine = true;
    request->send(200);
  });

  server.on("/engine/off", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    Serial.println(F("[ENGINE] [OFF]"));
    isActiveEngine = false;
    request->send(200);
  });

  server.on("/engine/speed", HTTP_POST, [](AsyncWebServerRequest *request)
  {
    if (request->hasParam("speed", true))
    {
      int speed = request->getParam("speed", true)->value().toInt();
      Serial.print(F("[ENGINE] [SPEED] "));
      Serial.println(speed);
      handleSpeed(speed);
    }

    request->send(200);
  });
}

void handleSensorHTTP()
{
  
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

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
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
