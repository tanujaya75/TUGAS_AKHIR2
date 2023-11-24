#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_PCF8591.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PCF8574.h>
#include <BH1750.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <LoRa.h>

#define SENSOR_INDEX_SENSOR_EC_1 0
#define SENSOR_INDEX_SENSOR_EC_2 1
#define csPin 18
#define resetPin 23
#define irqPin 26
#define RELAY_1 0
#define RELAY_2 1
#define RELAY_3 2
#define RELAY_4 3
#define OLED_SDA 21
#define OLED_SCL 22 
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// const char* ssid = "LAPTOP-JR9OPGBQ 2346";
// const char* password = "73%P170j";
// const char* ssid = "Edward";
// const char* password = "edward181422";
const char* ssid = "Antares";
const char* password = "12345678";
const char* Host1 = "192.168.0.104";
//const char* Host2 = "192.168.137.147";
const char* Topic1 = "Topic1";
const char* Topic2 = "Topic2";
const char* Topic3 = "Topic3";
char MessageToSend1_Char[100];
char MessageToSend2_Char[100];
char Buffer1[50];
char Buffer2[50];
char Buffer3[50];
char Buffer4[50];
char BufferSensor[10];
char EC_Level[4];
char* Suhu_Char;
char* Kelembapan_Char;
char* Tekanan_Char;
char* Lux_Char;
char* String1Data;
char EC_Sensor;

String MessageToSendJSON;
String SensorValue[15];
String StringData2;
String message;
String outgoing;
String payload1;
String payload2;
String payload3;
String payload4;
String payload5;
String Pompa;
String Kipas;
String Lampu;
String Misting;
String Pompa_JS;
String Lampu_JS;
String Kipas_JS;
String Misting_JS;
String nutrientDataString;
String nutrientDataStringLast = "LOW";

char* SensorEC1Value = new char[SensorValue[SENSOR_INDEX_SENSOR_EC_1].length() + 1];
char* SensorEC2Value = new char[SensorValue[SENSOR_INDEX_SENSOR_EC_2].length() + 1];

TickType_t NOW;
TickType_t PREVTIME;
const TickType_t INTERVAL = pdMS_TO_TICKS(2000);
TickType_t NOW2;
TickType_t PREVTIME2;
const TickType_t INTERVAL2 = pdMS_TO_TICKS(500);
TickType_t NOW3;
TickType_t PREVTIME3;
const TickType_t INTERVAL3 = pdMS_TO_TICKS(500);
TickType_t NOW4;
TickType_t PREVTIME4;
const TickType_t INTERVAL4 = pdMS_TO_TICKS(1000);

int Tekanan;
int String1Length;
int LUX;
int IndexSensor = 0;
int AddressRequest;
int IntECLevel;
int ecInt;       
int interval = 2000;
int PompaState;
int KipasState;
int LampuState;
int MistingState;
int Kelembapan;

float Kelembapan2;

byte countery=1;
byte counterx=1;
byte counterx2=1;
byte OKAY = 0;
byte Suhu;
byte BTN = 0;
byte SB2 = 0;
byte X_Pos;
byte Y_Pos;
byte Button;
byte Mode = 0;

float ec; 

DynamicJsonDocument JS_SEND(1000);
DynamicJsonDocument JS_Receive(1000);
TaskHandle_t Task1; 
TaskHandle_t Task2; 

Adafruit_BME280 BME280;
Adafruit_PCF8591 Joystick = Adafruit_PCF8591();
PCF8574 relay;
BH1750 LightSensor(0x23);
SoftwareSerial ECSerial(34,4);
RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 20, 4);
Adafruit_SSD1306 lcd2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
DateTime now; 

WiFiClient wificlient;

PubSubClient Client1(wificlient);

void SetRelay(byte RelayIndex, int Status);

void Wifi_Connect() 
{
  Serial.println("Menghubungkan WiFi Dengan: ");
  Serial.println(ssid);

  lcd2.clearDisplay();
  lcd2.setTextColor(WHITE);
  lcd2.setTextSize(0.5);
  lcd2.setCursor(0,0);
  lcd2.print("Menghubungkan WiFi Dengan: ");
  lcd2.setCursor(0,20);
  lcd2.print(ssid);
  lcd2.display();

  vTaskDelay(pdMS_TO_TICKS(1000));
 
  WiFi.begin(ssid, password);

  if (WiFi.status() != WL_CONNECTED) 
  {
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(100));
  }

  digitalWrite(LED_BUILTIN, HIGH);

  if (WiFi.status() == WL_CONNECTED) 
  {
    lcd2.clearDisplay();
    lcd2.setTextColor(WHITE);
    lcd2.setTextSize(0.5);
    lcd2.setCursor(0,0);
    lcd2.print("Terhubung: ");
    lcd2.setCursor(0,20);
    lcd2.print(WiFi.localIP());
    lcd2.display();
    Serial.println("Terhubung");
    Serial.println(WiFi.localIP());
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void callback1(char* topic, byte* payload, unsigned int length) 
{
  Serial.print("Message Received: ");
  Serial.println(topic);
  Serial.print("");

  String comming_message = "";

  for (int i = 0; i < length; i++) {
    comming_message += (char)payload[i];
  }
  Serial.println();
  Serial.println(comming_message);

   if ((strcmp(topic,"Topic2") == 0))
   {
    JS_Receive.clear();
    DeserializationError error = deserializeJson(JS_Receive, comming_message);

    if (error) {
      Serial.print("Failed to parse JSON: ");
      Serial.println(error.c_str());
      return;
    }

    Serial.println("Received on Topic2:");
    Pompa_JS = String(JS_Receive["Pompa"].as<String>());
    Lampu_JS = String(JS_Receive["Lampu"].as<String>());
    Kipas_JS = String(JS_Receive["Kipas"].as<String>());
    Misting_JS = String(JS_Receive["Misting"].as<String>());
    if(Pompa_JS.equals("HIGH"))
    {
      SetRelay(0, LampuState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, KipasState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2,1);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, MistingState);
      vTaskDelay(pdMS_TO_TICKS(10));
      PompaState = 1;
    }
    if(Lampu_JS.equals("HIGH"))
    {
      SetRelay(0,1);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, KipasState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, PompaState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, MistingState);
      vTaskDelay(pdMS_TO_TICKS(10));
      LampuState = 1;
    }
    if(Kipas_JS.equals("HIGH"))
    {
      SetRelay(0, LampuState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1,1);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, PompaState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, MistingState);
      KipasState = 1;
    }
    if(Misting_JS.equals("HIGH"))
    {
      SetRelay(0, LampuState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, KipasState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, PompaState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3,1);
      vTaskDelay(pdMS_TO_TICKS(10));
      MistingState = 1;
    }
    if(Pompa_JS.equals("LOW"))
    {
      SetRelay(0, LampuState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, KipasState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2,0);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, MistingState);
      PompaState = 0;
    }
    if(Lampu_JS.equals("LOW"))
    {
      SetRelay(0,0);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, KipasState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, PompaState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, MistingState);
      LampuState = 0;
    }
    if(Kipas_JS.equals("LOW"))
    {
      SetRelay(0, LampuState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1,0);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, PompaState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, MistingState);
      KipasState = 0;
    }
    if(Misting_JS.equals("LOW"))
    {
      SetRelay(3, 0);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(0, LampuState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, KipasState);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, PompaState);
      MistingState = 0;
    }
    if(Pompa_JS.equals("HIGH") && Lampu_JS.equals("HIGH") && Kipas_JS.equals("HIGH") && Misting_JS.equals("HIGH"))
    {
      SetRelay(0, 1);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, 1);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, 1);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, 1);
      vTaskDelay(pdMS_TO_TICKS(10));
      PompaState = 1;
      LampuState = 1;
      KipasState = 1;
      MistingState = 1;
    }
    if(Pompa_JS.equals("LOW") && Lampu_JS.equals("LOW") && Kipas_JS.equals("LOW") && Misting_JS.equals("LOW"))
    {
      SetRelay(0, 0);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(1, 0);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(2, 0);
      vTaskDelay(pdMS_TO_TICKS(10));
      SetRelay(3, 0);
      vTaskDelay(pdMS_TO_TICKS(10));
      PompaState = 0;
      LampuState = 0;
      KipasState = 0;
      MistingState = 0;
    }
    Serial.println(Pompa_JS);
    Serial.println(Lampu_JS);
    Serial.println(Kipas_JS);
    Serial.println(Misting_JS);
   }
}

void MQTT1_Reconnect() 
{
  if ((WiFi.status() == WL_CONNECTED) && (!Client1.connected())) {
    Serial.println("Reconnecting TO MQTT Broker: ");
    Serial.println(Host1);

    lcd2.clearDisplay();
    lcd2.setTextColor(WHITE);
    lcd2.setTextSize(0.5);
    lcd2.setCursor(0,0);
    lcd2.print("Reconnecting To: ");
    lcd2.setCursor(0,20);
    lcd2.print(Host1);
    lcd2.display();
    
    if (Client1.connect("1")) 
    {
      lcd2.clearDisplay();
      lcd2.setTextColor(WHITE);
      lcd2.setTextSize(0.5);
      lcd2.setCursor(0,0);
      lcd2.print("Connected To: ");
      lcd2.setCursor(0,20);
      lcd2.print(Host1);
      lcd2.display();
    } 
    else 
    {
      Serial.println("Failed To Connect Broker1, rc=");
      Serial.print(Client1.state());
      Serial.println("");
      Serial.println(" Try Again In 5 Seconds");

      lcd2.clearDisplay();
      lcd2.setTextColor(WHITE);
      lcd2.setTextSize(0.5);
      lcd2.setCursor(0,0);
      lcd2.print("Try Again In 5 Seconds");
      lcd2.setCursor(0,20);
      lcd2.print("Reconnecting To: ");
      lcd2.setCursor(0,20);
      lcd2.print(Host1);
      lcd2.display();

      digitalWrite(LED_BUILTIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(100));
      digitalWrite(LED_BUILTIN, LOW);
      vTaskDelay(pdMS_TO_TICKS(100));

      vTaskDelay(pdMS_TO_TICKS(5000));
    }
  }
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void GetECSensor(int address)
{
  //01 03 00 00 00 02 C4 0B
  //02 03 00 00 00 02 C4 38
  if (address == 1)
  {
    ECSerial.write(0x01);
    ECSerial.write(0x03);
    ECSerial.write((byte)0x00);
    ECSerial.write((byte)0x00);
    ECSerial.write((byte)0x00);
    ECSerial.write(0x02);
    ECSerial.write(0xC4);
    ECSerial.write(0x0B);
  }

  if (address == 2)
  {
    ECSerial.write(0x02);
    ECSerial.write(0x03);
    ECSerial.write((byte)0x00);
    ECSerial.write((byte)0x00);
    ECSerial.write((byte)0x00);
    ECSerial.write(0x02);
    ECSerial.write(0xC4);
    ECSerial.write(0x38);
  }
}

void GetECSensorData()
{
  while (ECSerial.available())
  {
    EC_Sensor = ECSerial.read();
    BufferSensor[IndexSensor] = EC_Sensor;
    IndexSensor++;

    //01 03 04 37 32 00 33
    if (IndexSensor > 6)
    {
      if (BufferSensor[0] == AddressRequest && BufferSensor[1] == 0x03 && BufferSensor[2] == 0x04)
      {
        StringData2 = String(BufferSensor[5], HEX) + String(BufferSensor[6], HEX);
        StringData2.toCharArray(EC_Level, StringData2.length() + 1);

        int IntECLevel = strtol(EC_Level, NULL, 16);
        float ec = IntECLevel*0.64; //*0.64;
        Serial.print("EC ORI");
        Serial.println(ec);
        int ecInt = round(ec);
        
        if (AddressRequest == 1) 
        {
          SensorValue[SENSOR_INDEX_SENSOR_EC_1] = (String)ecInt;
        }  
        else if (AddressRequest == 2) 
        {
          SensorValue[SENSOR_INDEX_SENSOR_EC_2] = (String)ecInt;
        }     
      }
      IndexSensor = 0;
    }
  }
}

void GetBMEData()
{
  Suhu = round(BME280.readTemperature());
  Kelembapan2 = BME280.readHumidity();
  Kelembapan = int(round(Kelembapan2));
  Tekanan = round(BME280.readPressure() / 100.0F);
  payload1 = String(Suhu);
  payload2 = String(Kelembapan);
  payload3 = String(Tekanan);
}

void GetLuxData()
{
  LUX = LightSensor.readLightLevel();
  payload4 = String(LUX);
}

void CheckJoystick()
{
  X_Pos = Joystick.analogRead(1);
  Y_Pos = Joystick.analogRead(2);
  Button = Joystick.analogRead(3);

  if (X_Pos < 100 && Y_Pos < 150 & Y_Pos > 100 && counterx == 2) 
  {
    countery++;
  }
  if (X_Pos > 150 && Y_Pos< 150 & Y_Pos > 100 && counterx == 2) 
  {
    countery--;
  }
  if (Y_Pos < 100 && X_Pos < 150 & X_Pos > 100) 
  {
    counterx--;
  }
  if (Y_Pos > 150 && X_Pos < 150 & X_Pos > 100) 
  {
    counterx++;
  }

  if (Button < 10) 
  {
    BTN = 1;
  }
  else if(Button > 10)
  {
    BTN = 0;
  }

  counterx = constrain(counterx, 1, 2);
  countery = constrain(countery, 1, 4);

  Serial.print("X:");
  Serial.println(counterx);
  Serial.print("Y:");
  Serial.println(countery);

}

void CheckJoystick2()
{
  X_Pos = Joystick.analogRead(1);
  Y_Pos = Joystick.analogRead(2);
  Button = Joystick.analogRead(3);

  if (Y_Pos < 100 && X_Pos < 150 & X_Pos > 100 && SB2 == 1) 
  {
    counterx2--;
  }
  if (Y_Pos > 150 && X_Pos < 150 & X_Pos > 100 && SB2 == 1) 
  {
    counterx2++;
  }

  counterx2 = constrain(counterx2, 1, 2);

  if (Button < 10) 
  {
    BTN = 1;
  }
  else if(Button > 10)
  {
    BTN = 0;
  }

  Serial.print("X2:");
  Serial.println(counterx2);
}

void Menu1()
{
 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(String(now.day(), DEC) + "/" + String(now.month(), DEC) + "/" + String(now.year(), DEC) + " | " + String(now.hour(), DEC) + ":" + String(now.minute(), DEC) + ":" + String(now.second(), DEC));
  
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.setCursor(2,1);
  lcd.print(Suhu);
  lcd.setCursor(6,1);
  lcd.print("*C");

  lcd.setCursor(0,2);
  lcd.print("H:");
  lcd.setCursor(2,2);
  lcd.print(Kelembapan);
  lcd.setCursor(7,2);
  lcd.print("%");

  lcd.setCursor(0,3);
  lcd.print("L:");
  lcd.setCursor(2,3);
  lcd.print(LUX);
  lcd.setCursor(7,3);
  lcd.print("L");

  lcd.setCursor(9,1);
  lcd.print("P");
  lcd.setCursor(12,1);
  lcd.print(":");
  lcd.setCursor(13,1);
  lcd.print(Tekanan);
  lcd.setCursor(17,1);
  lcd.print("hPa");

  lcd.setCursor(9,2);
  lcd.print("EC1:");
  lcd.setCursor(13,2);
  lcd.print(SensorEC1Value);
  lcd.setCursor(17,2);
  lcd.print("PPM");

  lcd.setCursor(9,3);
  lcd.print("EC2:");
  lcd.setCursor(13,3);
  lcd.print(SensorEC2Value);
  lcd.setCursor(17,3);
  lcd.print("PPM");

  lcd.setCursor(8,1);
  lcd.print("|");
  lcd.setCursor(8,2);
  lcd.print("|");
  lcd.setCursor(8,3);
  lcd.print("|");
}

void Menu2()
{
  switch (countery)
  {
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(">Pompa Air");
      lcd.setCursor(10,0);
      lcd.print(Pompa);
      lcd.setCursor(0,1);
      lcd.print(" Kipas");
      lcd.setCursor(10,1);
      lcd.print(Kipas);
      lcd.setCursor(0,2);
      lcd.print(" Lampu");
      lcd.setCursor(10,2);
      lcd.print(Lampu);
      lcd.setCursor(0,3);
      lcd.print(" Misting");
      lcd.setCursor(10,3);
      lcd.print(Misting);
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" Pompa Air");
      lcd.setCursor(10,0);
      lcd.print(Pompa);
      lcd.setCursor(0,1);
      lcd.print(">Kipas");
      lcd.setCursor(10,1);
      lcd.print(Kipas);
      lcd.setCursor(0,2);
      lcd.print(" Lampu");
      lcd.setCursor(10,2);
      lcd.print(Lampu);
      lcd.setCursor(0,3);
      lcd.print(" Misting");
      lcd.setCursor(10,3);
      lcd.print(Misting);
      break;
    case 3:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" Pompa Air");
      lcd.setCursor(10,0);
      lcd.print(Pompa);
      lcd.setCursor(0,1);
      lcd.print(" Kipas");
      lcd.setCursor(10,1);
      lcd.print(Kipas);
      lcd.setCursor(0,2);
      lcd.print(">Lampu");
      lcd.setCursor(10,2);
      lcd.print(Lampu);
      lcd.setCursor(0,3);
      lcd.print(" Misting");
      lcd.setCursor(10,3);
      lcd.print(Misting);
      break;
    case 4:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" Pompa Air");
      lcd.setCursor(10,0);
      lcd.print(Pompa);
      lcd.setCursor(0,1);
      lcd.print(" Kipas");
      lcd.setCursor(10,1);
      lcd.print(Kipas);
      lcd.setCursor(0,2);
      lcd.print(" Lampu");
      lcd.setCursor(10,2);
      lcd.print(Lampu);
      lcd.setCursor(0,3);
      lcd.print(">Misting");
      lcd.setCursor(10,3);
      lcd.print(Misting);
      break;

  }
}

void Submenu()
{
  switch (countery)
  {
    case 1:
      switch (counterx2)
      {
        case 1:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Pompa Air ?");
          lcd.setCursor(4,2);
          lcd.print(">ON");
          lcd.setCursor(10,2);
          lcd.print(" OFF");
          break;
        case 2:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Pompa Air ?");
          lcd.setCursor(4,2);
          lcd.print(" ON");
          lcd.setCursor(10,2);
          lcd.print(">OFF");  
          break;
      }
      break;

    case 2:
      switch (counterx2)
      {
        case 1:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Kipas ?");
          lcd.setCursor(4,2);
          lcd.print(">ON");
          lcd.setCursor(10,2);
          lcd.print(" OFF");
          break;
        case 2:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Kipas ?");
          lcd.setCursor(4,2);
          lcd.print(" ON");
          lcd.setCursor(10,2);
          lcd.print(">OFF");  
          break;
      }
      break;

    case 3:
      switch (counterx2)
      {
        case 1:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Lampu ?");
          lcd.setCursor(4,2);
          lcd.print(">ON");
          lcd.setCursor(10,2);
          lcd.print(" OFF");
          break;
        case 2:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Lampu ?");
          lcd.setCursor(4,2);
          lcd.print(" ON");
          lcd.setCursor(10,2);
          lcd.print(">OFF");  
          break;
      }
    break;

    case 4:
      switch (counterx2)
      {
        case 1:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Misting ?");
          lcd.setCursor(4,2);
          lcd.print(">ON");
          lcd.setCursor(10,2);
          lcd.print(" OFF");
          break;
        case 2:
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Misting ?");
          lcd.setCursor(4,2);
          lcd.print(" ON");
          lcd.setCursor(10,2);
          lcd.print(">OFF");  
          break;
      }
    break;
    
  }
}

void receiveLoRa() 
{
  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    String incoming = "";
    while (LoRa.available()) 
    {
      char c = LoRa.read();
      if (c >= 32 && c <= 126) 
      {
        incoming += c;
      }
    }
    
    int delimiterIndex = incoming.indexOf('|');
    if (delimiterIndex != -1) 
    {
      String waterpumpDataString = incoming.substring(0, delimiterIndex);
      String remainingData = incoming.substring(delimiterIndex + 1);
      int secondDelimiterIndex = remainingData.indexOf('|');

      if (secondDelimiterIndex != -1) 
      {
        nutrientDataString = remainingData.substring(0, secondDelimiterIndex);
        remainingData = remainingData.substring(secondDelimiterIndex + 1);
        int thirdDelimiterIndex = remainingData.indexOf('|');

        if (thirdDelimiterIndex != -1) 
        {
          String fanDataString = remainingData.substring(0, thirdDelimiterIndex);
          remainingData = remainingData.substring(thirdDelimiterIndex + 1);
          int fourthDelimiterIndex = remainingData.indexOf('|');

          if (fourthDelimiterIndex != -1) 
          {
            String lightDataString = remainingData.substring(0, fourthDelimiterIndex);
            String mistingDataString = remainingData.substring(fourthDelimiterIndex + 1);
            MistingState = mistingDataString.toInt();
            
            if(waterpumpDataString.toInt() == 1) 
            {
              PompaState = waterpumpDataString.toInt();
              SetRelay(0,LampuState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(1,KipasState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(2,1);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(3,MistingState);
              vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            if(fanDataString.toInt() == 1)
            {
              KipasState = fanDataString.toInt();
              SetRelay(0,LampuState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(1,1);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(2,PompaState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(3,MistingState);
              vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            if(lightDataString.toInt() == 1)
            {
              LampuState = lightDataString.toInt();
              SetRelay(0,1);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(1,KipasState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(2,PompaState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(3,MistingState);
              vTaskDelay(pdMS_TO_TICKS(10));
            }

            if(waterpumpDataString.toInt() == 0)
            {
              PompaState = waterpumpDataString.toInt();
              SetRelay(0,LampuState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(1,KipasState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(2,0);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(3,MistingState);
              vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            if(fanDataString.toInt() == 0)
            {
              KipasState = fanDataString.toInt();
              SetRelay(0,LampuState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(1,0);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(2,PompaState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(3,MistingState);
              vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            if(lightDataString.toInt() == 0)
            {
              LampuState = lightDataString.toInt();
              SetRelay(0,0);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(1,KipasState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(2,PompaState);
              vTaskDelay(pdMS_TO_TICKS(10));
              SetRelay(3,MistingState);
              vTaskDelay(pdMS_TO_TICKS(10));
            }
            
            Serial.print("Waterpump Data: ");
            Serial.println(PompaState);
            Serial.print("Nutrientpump Data: ");
            Serial.println(nutrientDataString);
            Serial.print("Fan Data: ");
            Serial.println(KipasState);
            Serial.print("Light Data: ");
            Serial.println(LampuState);
            Serial.print("Misting Data: ");
            Serial.println(MistingState);
          }
        }
      }
    }
  }
}

void sendMessage(String outgoing) 
{
  LoRa.beginPacket();                   
  LoRa.print("The Data - ");
  LoRa.print(message);
  LoRa.endPacket();       
  Serial.print("Lora Message: ");     
  Serial.println(message);                                   
}

void SetRelay(byte RelayIndex, int Status)
{
  bool StatusBool;
  if(Status == 0)
  {
    StatusBool = LOW;
  }
  if(Status == 1)
  {
    StatusBool = HIGH;
  }

  relay.write(RelayIndex, !StatusBool); 
  if(RelayIndex == 0 && Status == 1)
  {
    Lampu = "ON";
  }
  else if(RelayIndex == 0 && Status == 0)
  {
    Lampu = "OFF";
  }
  else if(RelayIndex == 1 && Status == 1)
  {
    Kipas = "ON";
  }
  else if(RelayIndex == 1 && Status == 0)
  {
    Kipas = "OFF";
  }
  else if(RelayIndex == 2 && Status == 1)
  {
    Pompa = "ON";
  }
  else if(RelayIndex == 2 && Status == 0)
  {
    Pompa = "OFF";
  }
  else if(RelayIndex == 3 && Status == 1)
  {
    Misting = "ON";
  }
  else if(RelayIndex == 3 && Status == 0)
  {
    Misting = "OFF";
  }
}

void task1(void *pvParameters) 
{
  while(true)
  {
    if (WiFi.status() != WL_CONNECTED) 
    {
      Serial.println("WiFi Reconnecting !");
      message = payload1 + "," + payload2 + "," + payload3 + "," + payload4 + "," + payload5;
      sendMessage(message);
      Wifi_Connect();
    } 
    else if (WiFi.status() == WL_CONNECTED && !Client1.connected()) 
    {
      Serial.println("Reconnecting MQTT Broker1 !");
      message = payload1 + "," + payload2 + "," + payload3 + "," + payload4 + "," + payload5;
      sendMessage(message);
      MQTT1_Reconnect();
    } 
    else if (WiFi.status() == WL_CONNECTED && Client1.connected()) 
    {
      
      NOW = xTaskGetTickCount();
      if ((NOW - PREVTIME) >= INTERVAL) {
        JS_SEND.clear();
        JS_SEND["Suhu"] = Suhu_Char;
        JS_SEND["Kelembapan"] = Kelembapan_Char;
        JS_SEND["Tekanan"] = Tekanan_Char;
        JS_SEND["Lux"] = Lux_Char;
        JS_SEND["EC"] = SensorEC2Value;

        serializeJson(JS_SEND, MessageToSendJSON);
        String1Length = MessageToSendJSON.length();
        String1Data = new char[String1Length * 2];
        MessageToSendJSON.toCharArray(String1Data, (String1Length * 2));
        if (String1Data != "" || String1Data != NULL)
        {
        Client1.publish(Topic1, String1Data);
        }
        Client1.subscribe(Topic2);

        MessageToSendJSON = "";
        Buffer1[0] = '\0';
        Buffer2[0] = '\0';
        Buffer3[0] = '\0';
        Buffer4[0] = '\0';
        Suhu_Char = "";
        Kelembapan_Char = "";
        Tekanan_Char = "";
        Lux_Char = "";
        String1Data = "";

        message = payload1 + "," + payload2 + "," + payload3 + "," + payload4 + "," + payload5;
        sendMessage(message);
      
        PREVTIME = xTaskGetTickCount();
      }
      receiveLoRa();
      if (!nutrientDataString.equals("") || !nutrientDataString.equals(NULL) && nutrientDataString != nutrientDataStringLast)
      {
        Client1.publish(Topic3, nutrientDataString.c_str());
        nutrientDataStringLast = nutrientDataString;
      }
      
      Client1.loop();
    }
  }
}

void task2(void *pvParameters)
{
  while(true)
  {
    //Menu1();
    NOW4 = xTaskGetTickCount();
    if ((NOW4 - PREVTIME4) >= INTERVAL4) 
    {
      GetBMEData();
      GetLuxData();
    
      GetECSensor(AddressRequest);
      vTaskDelay(pdMS_TO_TICKS(50));
      
      if(AddressRequest == 1)
      {
        GetECSensorData();
        AddressRequest = 2;
      }

      GetECSensor(AddressRequest);
      vTaskDelay(pdMS_TO_TICKS(50));

      if(AddressRequest == 2)
      {
        GetECSensorData();
        AddressRequest = 1;
      }
      vTaskDelay(pdMS_TO_TICKS(50));

      Suhu_Char = itoa(Suhu, Buffer1, 10);
      Kelembapan_Char = itoa(Kelembapan, Buffer2, 10);
      Tekanan_Char = itoa(Tekanan, Buffer3, 10);
      Lux_Char = itoa(LUX, Buffer4, 10);
      SensorValue[SENSOR_INDEX_SENSOR_EC_1].toCharArray(SensorEC1Value, SensorValue[SENSOR_INDEX_SENSOR_EC_1].length() + 1);
      SensorValue[SENSOR_INDEX_SENSOR_EC_2].toCharArray(SensorEC2Value, SensorValue[SENSOR_INDEX_SENSOR_EC_2].length() + 1);

      payload5 = String(SensorEC1Value);
      
      Serial.print("Temperature = ");
      Serial.print(Suhu);
      Serial.println(" *C");

      Serial.print("Humidity = ");
      Serial.print(Kelembapan);
      Serial.println(" %");

      Serial.print("Pressure = ");
      Serial.print(Tekanan);  // hPa
      Serial.println(" hPa");

      Serial.print("Light Intensity = ");
      Serial.print(LUX);  // hPa
      Serial.println(" lux");

      Serial.print("EC Sensor1: ");
      Serial.print(SensorEC1Value);
      Serial.println("PPM");

      Serial.print("EC Sensor2: ");
      Serial.print(SensorEC2Value);
      Serial.println("PPM");

      now = rtc.now();

      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(' ');
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();

      NOW2 = xTaskGetTickCount();
      if(SB2 == 0)
      {
        if((NOW2 - PREVTIME2) >= INTERVAL2)
        {
          CheckJoystick();
          if(counterx == 1)
          {
            Menu1();
          }
          if(counterx == 2)
          {
            Menu2();
          }
          if (BTN == 1 && counterx == 2 && SB2 == 0)
          {
            SB2 = 1;
            Submenu();
            PREVTIME3 = xTaskGetTickCount();
          }
          PREVTIME2 = NOW2;
        }
      }
      if(SB2 == 1)
      {
        CheckJoystick2();
        NOW3 = xTaskGetTickCount();
        if((NOW3 - PREVTIME3)>=INTERVAL3)
        {
          Submenu();
          if(SB2 == 1 && counterx2 == 1 && BTN == 1)
          {
            OKAY++;
            lcd.setCursor(19, 0);
            lcd.print(OKAY);
            if(OKAY >= 5)
            {
              OKAY = 0;
              if(countery == 1)
              {
                Serial.println(countery);
                SetRelay(0, LampuState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1, KipasState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2,1);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(3, MistingState);
                vTaskDelay(pdMS_TO_TICKS(10));
                PompaState = 1;
              }
              else if(countery == 2 )
              {
                Serial.println(countery);
                SetRelay(0, LampuState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1,1);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2, PompaState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(3, MistingState);
                KipasState = 1;
              }
              else if(countery == 3 )
              {
                SetRelay(0,1);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1, KipasState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2, PompaState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(3, MistingState);
                vTaskDelay(pdMS_TO_TICKS(10));
                LampuState = 1;
              }
              else if(countery == 4)
              {
                SetRelay(0, LampuState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1, KipasState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2, PompaState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(3,1);
                vTaskDelay(pdMS_TO_TICKS(10));
                MistingState = 1;
              }
              vTaskDelay(pdMS_TO_TICKS(50));
              SB2 = 0;
            }
          }
          if(SB2 == 1 && counterx2 == 2 && BTN == 1)
          {
            OKAY++;
            lcd.setCursor(19, 0);
            lcd.print(OKAY);
            if(OKAY >= 5)
            {
              OKAY = 0;
              if(countery == 1 )
              {
                SetRelay(0, LampuState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1, KipasState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2,0);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(3, MistingState);
                PompaState = 0;
              }
              else if(countery == 2 )
              {
                SetRelay(0, LampuState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1,0);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2, PompaState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(3, MistingState);
                KipasState = 0;
              }
              else if(countery == 3 )
              {
                SetRelay(0,0);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1, KipasState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2, PompaState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(3, MistingState);
                LampuState = 0;
              }
              else if(countery == 4)
              {
                SetRelay(3, int(0));
                SetRelay(0, LampuState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(1, KipasState);
                vTaskDelay(pdMS_TO_TICKS(10));
                SetRelay(2, PompaState);
                vTaskDelay(pdMS_TO_TICKS(10));
                MistingState = 0;
              }
              vTaskDelay(pdMS_TO_TICKS(50));
              SB2 = 0;
            }
          }
          PREVTIME3 = NOW3;
        }
      }
      PREVTIME4 = NOW4;
    }
  }  
}

void setup() 
{
  Serial.begin(115200);
  ECSerial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  LoRa.setPins(csPin, resetPin, irqPin);
  Wire.begin(OLED_SDA, OLED_SCL);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,1);
  lcd.print("Green Firmware V0.1");
  lcd.setCursor(1,2);
  lcd.print("Made In Surabaya");
  delay(1000);

  while (!BME280.begin(0x76)) 
  {
    Serial.println("Sensor BME280 Tidak diTemukan");
  }

  while (!LightSensor.begin()) 
  {
    Serial.println("Sensor BH1750 Tidak diTemukan");
  }

  if (! rtc.begin()) {
    Serial.println("RTC Tidak Ditemukan!!");
    while (1);
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  while (!LoRa.begin(915E6)) 
  {             
    Serial.println("LoRa init failed. Check your connections.");                      
  }
  while (!Joystick.begin()) 
  {
    Serial.println("Joystick Tidak diTemukan !");
  }
  while(!lcd2.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) 
  { 
    Serial.println(F("LCD 2 Tidak diTemukan"));
  }

  relay.begin(0x20);
  Joystick.enableDAC(true);  

  SetRelay(0,LOW);
  SetRelay(1,LOW);
  SetRelay(2,LOW);
  SetRelay(3,LOW);

  PompaState = 0;
  KipasState = 0;
  LampuState = 0;
  MistingState = 0;

  AddressRequest = 1;
 
  Client1.setServer(Host1, 1883);
  Client1.setCallback(callback1);

  vTaskDelay(pdMS_TO_TICKS(1000));

  xTaskCreatePinnedToCore(task1, "Task 1", 3000, NULL, 1, &Task1, CONFIG_ARDUINO_RUNNING_CORE);
  xTaskCreatePinnedToCore(task2, "Task 2", 3000, NULL, 2, &Task2, 0);
}

void loop() 
{
  vTaskDelay(pdMS_TO_TICKS(10));
}
