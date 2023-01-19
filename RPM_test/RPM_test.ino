#include <LiquidCrystal_I2C.h>
#include <ArduinoJson.h>
#include <Thread.h>
#include "max6675.h"

#define RPM_sensor 2
#define pressure_Sensor A0
#define SO 12
#define CS 10
#define sck 13
unsigned int count;
unsigned long previousTime = 0;
unsigned int times = 0, RPM = 0;
bool displayFlag = false;
bool displayFlag1 = false;

MAX6675 module(sck, CS, SO);
StaticJsonDocument<200> doc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Thread serialThread = Thread();


byte degree[8] =
{
  0b00011,
  0b00011,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

void Count()
{
  lcd.setCursor(0, 3);
  lcd.print("Crankshaft RPM: ");
  bool flag = false;
  Serial.println(digitalRead(RPM_sensor));
  if (digitalRead(RPM_sensor))
  {
    count++;
    Serial.println(count);
    while (digitalRead(RPM_sensor));
  }
  if (millis() - previousTime >= 10000) {
    previousTime = millis();
    flag = true;

  }

  if (flag) {
    count *= 6;
    doc["RPM"] = count;
    lcd.clear();
    lcd.setCursor(0, 3);
    lcd.print("Crankshaft RPM: " + String (count));
    count = 0;
  }

}

void pressure()
{
  int sensorVal = analogRead(pressure_Sensor);
  float voltage = (sensorVal * 5.0) / 1024.0;
  float pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
  float pressure_bar = pressure_pascal / 10e5;
  float PSI = pressure_bar * 14.5038;
  doc["pressure"] = PSI;
  lcd.setCursor(0, 2);
  lcd.print("Pressure: " + String(PSI) + "psi");
  //delay(100);
}


void temp()
{
  int temperature = module.readCelsius();
  doc["temp"] = temperature;
  lcd.setCursor(0, 1);
  lcd.print("EGT: " + String(temperature));
  lcd.write(1);
  lcd.print("C");
  delay(300);

  if (temperature >= 100) {
    displayFlag = true;
  }
  if (displayFlag && temperature < 100) {
    lcd.clear();
    displayFlag = false;
  }
}

void serialize()
{
  serializeJsonPretty(doc, Serial);
}


void setup()
{
  Serial.begin(115200);
  //Wire.begin(2, 0);
  pinMode(RPM_sensor, INPUT);
  serialThread.onRun(serialize);
  serialThread.setInterval(10000);
  lcd.init();      // initialize the lcd
  lcd.backlight(); // Turns On BackLight
  lcd.createChar(1, degree);
  lcd.setCursor(0, 2);
  lcd.print("Engine Display");
  delay(3000);
  lcd.clear();
}

void loop()
{
  if (serialThread.shouldRun())
  {
    serialThread.run();
  }
  lcd.setCursor(3, 0);
  lcd.print ("Engine Display");
  temp();
  pressure();
  Count();
}


