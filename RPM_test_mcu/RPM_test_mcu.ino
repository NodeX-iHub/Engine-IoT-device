#include "WiFi.h"
#include <FirebaseESP32.h>
#include <ArduinoJson.h>
#include <Thread.h>

#define FIREBASE_HOST "engine-sensors-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "88cb5d5846f9896ee7a30fae489b70e4a25bc548"

int temperature ;
int pressure ;
int RPM;

FirebaseData firebaseData;
FirebaseJson json;
StaticJsonDocument<200> doc;
Thread FBThread = Thread();

void setup()
{
  Serial.begin(115200);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  WiFi.begin("Nokia G20", "mypasscode91");
  Serial.println("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected");
  FBThread.onRun(FBCallback);
  FBThread.setInterval(10000);
}

void FBCallback() {
  json.set("temp", temperature);
  json.set("psi", pressure);
  json.set("RPM", RPM);
  json.toString(Serial, true );
  Firebase.set(firebaseData, "/engine", json);
}

void loop()
{
  DeserializationError error = deserializeJson(doc, Serial);
  if (!error)
  {
    temperature = doc["temp"];
    pressure = doc["pressure"];
    RPM = doc["RPM"];
  }
  if (FBThread.shouldRun()) {
    FBThread.run();
  }
}

