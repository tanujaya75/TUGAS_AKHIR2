#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
//#include <WiFi.h>
#include <PubSubClient.h>
#define LED 15 //D8
#define RELAY 0 //D3
#define RELAY2 2 //D4
#define CLK 16 //D0
#define DT 14 //D5
#define SW 12 //D6

// const char* ssid = "Edward";
// const char* pass = "edward181422";
const char* ssid = "Antares";
const char* pass = "12345678";
// const char* ssid = "LAPTOP-JR9OPGBQ 2346";
// const char* pass = "73%P170j";
const char* brokerip1 = "192.168.0.104";
// const char* brokerip2 = "192.168.137.147";
// const char* topic1 = "Topic1";
const char* topic2 = "Topic2";
const char* topic3 = "Topic3";
// const char* topic4 = "Topic4";
char buffer1[50];
char* messagetosend;
String printing;
String POMPA1STATE = "OFF";
String POMPA2STATE = "OFF";
String PMP;

int randomnummber;
int I;
byte Counter = 1;
byte Counter2 = 1;
byte Counter3 = 0;
byte Counter4 = 1;
byte CurrentStateCLK;
byte CurrentStateCLK2;
byte CurrentStateCLK3;
byte LastStateCLK;
byte LastStateCLK2;
byte LastStateCLK3;
byte MenuState = 1;
byte LastCounter = 1;
byte CurrentCounter;
byte Mode = 0;
byte faliure = 0;

bool RELAY1STATE = LOW;
bool RELAY2STATE = LOW;

unsigned long int now;
unsigned long int prevtime;
unsigned long int interval = 1000;
unsigned long int now2;
unsigned long int prevtime2;
unsigned long int interval2 = 100;

LiquidCrystal_I2C lcd(0x27, 16, 2);

WiFiClient Mqttclient2;
WiFiClient Mqttclient3;

PubSubClient client2(Mqttclient2);
PubSubClient client3(Mqttclient3);

DynamicJsonDocument doc(256);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  pinMode(LED, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  client2.setServer(brokerip1, 1883);
  client3.setServer(brokerip1, 1883);
  client2.setCallback(callback1);
  client3.setCallback(callback2);
  LastStateCLK = digitalRead(CLK);
  LastStateCLK2 = digitalRead(CLK);
  UpdateMenu();
  
  
}

void setupwifi(){
  delay(10);
  Serial.println();
  Serial.print("Connecting WiFi To: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) 
  {
    printing = "Menghubungkan WiFi Dengan: ";
    for(I=0; I<=printing.length(); I++)
    {
      
      digitalWrite(LED, HIGH);
      delay(100);
      digitalWrite(LED, LOW);
      delay(100);
    }
  }

  Serial.println();
  Serial.print("Connected, IP Address:");
  Serial.println(WiFi.localIP());
  delay(1000);

}

void Reconnect1() {
  while (WiFi.status() == WL_CONNECTED && !client2.connected()) {
    Serial.print("Reconnecting MQTT Broker1...");
    if (client2.connect("2")) 
    {
      delay(1000);
    } 
    else 
    {
      Serial.print("Failed To Connect Broker1, rc=");
      Serial.print(client2.state());
      Serial.println(" Try Again In 5 Seconds");
      delay(5000);
    }
  }
}


void Reconnect2() {
  while (WiFi.status() == WL_CONNECTED && !client3.connected()) {
    Serial.print("Reconnecting MQTT Broker2...");
    if (client3.connect("3")) 
    {
      Serial.println("Mqtt Broker2 Connected");
      delay(1000);
    } 
    else 
    {
      Serial.print("Failed To Connect Broker2, rc=");
      Serial.print(client3.state());
      Serial.println(" Try Again In 5 Seconds");
      delay(5000);
    }
  }
}

void callback1(char* topic, byte* payload, unsigned int length){

  Serial.print("Message Received: ");
  Serial.println(topic);
  Serial.print("");

  String comming_message="";

  for (int i = 0; i < length; i++) 
  {
    comming_message += (char)payload[i];
  }
  Serial.println();

  if ((strcmp(topic,"Topic3") == 0)) 
  {
    CONTROL_RELAY1(comming_message);
  }



}


void callback2(char* topic2, byte* payload2, unsigned int length2){

  Serial.print("Message Received: ");
  Serial.println(topic2);
  Serial.print("");

  String comming_message2="";

  for (int j = 0; j < length2; j++) 
  {
    comming_message2 += (char)payload2[j];
  }
  Serial.println();

  if ((strcmp(topic2,"Topic2") == 0)) 
  {
    
    doc.clear();
    DeserializationError error = deserializeJson(doc, comming_message2);
    if (error) 
    {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    String relay2_bool = doc["PompaNutrisi"];
    CONTROL_RELAY2(relay2_bool);
    Serial.println(comming_message2);

  }
}

void CONTROL_RELAY1(String relay1){
  if(relay1.equals("HIGH"))
  {
    RELAY1STATE = HIGH;
    POMPA1STATE = "ON ";
    digitalWrite(RELAY, HIGH);
  }
  else{
    RELAY1STATE = LOW;
    POMPA1STATE = "OFF";
    digitalWrite(RELAY, LOW);
  }
  if(MenuState == 1)
  {
    lcd.setCursor(8, 0);
    lcd.print(POMPA1STATE);
  }
}

void CONTROL_RELAY2(String relay2){
 
  if(relay2.equals("HIGH"))
  {
    RELAY2STATE = HIGH;
    POMPA2STATE = "ON ";
    digitalWrite(RELAY2, HIGH);
  }
  else
  {
    RELAY2STATE = LOW;
    POMPA2STATE = "OFF";
    digitalWrite(RELAY2, LOW);
  }
  if(MenuState == 1)
  {
    lcd.setCursor(8, 1);
    lcd.print(POMPA2STATE);
  }
}

void UpdateMenu() {
  switch (Counter) 
  {
    case 1:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(">Pompa1:");
      lcd.setCursor(0, 1);
      lcd.print(" Pompa2:");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Pompa1:");
      lcd.setCursor(0, 1);
      lcd.print(">Pompa2:");
      break;
  }
  if(MenuState == 1)
  {
    lcd.setCursor(8, 0);
    lcd.print(POMPA1STATE);
    lcd.setCursor(8, 1);
    lcd.print(POMPA2STATE);
  }
}

void SubMenu1(){
  switch (Counter2) 
  {
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Manual Pompa1");
      lcd.setCursor(4,1);
      lcd.print(">ON");
      lcd.setCursor(8,1);
      lcd.print(" OFF");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Manual Pompa1");
      lcd.setCursor(4,1);
      lcd.print(" ON");
      lcd.setCursor(8,1);
      lcd.print(">OFF");
      break;
  }
}

void SubMenu2(){
  switch (Counter2) 
  {
    case 1:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Manual Pompa2");
      lcd.setCursor(4,1);
      lcd.print(">ON");
      lcd.setCursor(8,1);
      lcd.print(" OFF");
      break;
    case 2:
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Manual Pompa2");
      lcd.setCursor(4,1);
      lcd.print(" ON");
      lcd.setCursor(8,1);
      lcd.print(">OFF");
      break;
  }
}

void ExecuteAction() {
  switch (Counter) 
  {
    case 1:
      SubMenu1();
      break;
    case 2:
      SubMenu2();
      break;
  }
}


void loop() {
  // put your main code here, to run repeatedly:

      if(WiFi.status() != WL_CONNECTED) 
      {
        Serial.println("Disconnected !");
        setupwifi();
      }
      else if(WiFi.status() == WL_CONNECTED && !client2.connected()) 
      {
        Reconnect1(); 
        Reconnect2(); 
      }
      else if(WiFi.status() == WL_CONNECTED && client2.connected()) 
      {
      
      client3.subscribe(topic2);
      client2.subscribe(topic3);
        
      CurrentStateCLK = digitalRead(CLK);
      if(digitalRead(SW) == LOW && MenuState == 1 && Counter3 == 0){
        Serial.println("Button Pressed");
        ExecuteAction();
        MenuState = 2;
      }
      else if (digitalRead(SW) == HIGH){
        Counter3 = 0;
        if(CurrentStateCLK != LastStateCLK){
          if((digitalRead(DT) != CurrentStateCLK) && MenuState == 1)
          {
            Counter++;
            if(Counter > 2)
            {
              Counter = 1;
              UpdateMenu();
              if(digitalRead(SW) == LOW)
              {
                ExecuteAction();
                MenuState = 2;
              }
            }
            else
            {
              UpdateMenu();
            }
          }
          else if((digitalRead(DT) == CurrentStateCLK) && MenuState == 1)
          {
            Counter--;
            UpdateMenu();
            if(Counter < 1){
              Counter = 2;
              UpdateMenu();
              if(digitalRead(SW) == LOW)
              {
                ExecuteAction();
                MenuState = 2;
              }
            }
            else
            {
              UpdateMenu();
            }
          }
          LastStateCLK = CurrentStateCLK;
        }
      }

      if ((digitalRead(SW) == false) && (MenuState == 2))
      {
        now2 = millis();
        if ((now2 - prevtime2) >= interval2)
        {
          Counter3++;
          lcd.setCursor(14,0);
          lcd.print(Counter3);
          if (Counter3 >= 5)
          {
            if(Counter == 1 && Counter2 == 1){
              CONTROL_RELAY1("HIGH");
            }
            else if(Counter == 1 && Counter2 == 2){
              CONTROL_RELAY1("LOW");
            }
            if(Counter == 2 && Counter2 == 1){
              CONTROL_RELAY2("HIGH");
            }
            else if(Counter == 2 && Counter2 == 2){
              CONTROL_RELAY2("LOW");
            }
            MenuState = 1;
            Counter = 1;
            UpdateMenu();
            Serial.println("Button Pressed");
          }

          prevtime2 = millis();
        }
      }
      
      // client2.loop();
      client2.loop();
      client3.loop();
      }
    

    if (MenuState == 2)
    {
      CurrentStateCLK2 = digitalRead(CLK);
      if(CurrentStateCLK2 != LastStateCLK2)
      {
        if((digitalRead(DT) != CurrentStateCLK2) && MenuState == 2)
        {
          Counter2++;
          if(Counter2 > 2)
          {
            Counter2 = 1;
            if(Counter == 1)
            {
              SubMenu1();
            }
            else if(Counter == 2)
            {
              SubMenu2();
            }
          }
          else
          {
            if(Counter == 1)
            {
              SubMenu1();
            }
            else if(Counter == 2)
            {
              SubMenu2();
            }
          }
        }
        else if((digitalRead(DT) == CurrentStateCLK2) && MenuState == 2)
        {
          Counter2--;
          if(Counter2 < 1)
          {
            Counter2 = 2;
            if(Counter == 1)
            {
              SubMenu1();
            }
            else if(Counter == 2)
            {
              SubMenu2();
            }
          }
          else
          {
            if(Counter == 1)
            {
              SubMenu1();
            }
            else if(Counter == 2)
            {
              SubMenu2();
            }
          }
        }
        LastStateCLK2 = CurrentStateCLK2;
      }
    }
  
}
