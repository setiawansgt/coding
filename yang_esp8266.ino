//=================Library=====================
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp8266.h>
#include <Adafruit_MCP4725.h>
#include <Wire.h>

//===============Blynk=========================
#define BLYNK_TEMPLATE_ID "TMPL6SeCdjsT5"
#define BLYNK_DEVICE_NAME "Motor Speed Controller"
#define BLYNK_AUTH_TOKEN "edAbKzYgAjI_2O_KAt8PndWyCITZuooQ"
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Galaxy A523403";
char pass[] = "fxit5608";

#define HallSen A0
#define Relay1 D6

LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_MCP4725 DAC;

//==========Variabel Waktu=====================
unsigned long previousMillisCheckConnection = 0;
const long intervalCheckConnection = 10000;
unsigned long previousMillisSendDataBlynk = 0;
const long intervalSendDataBlynk = 1000;
unsigned long waktuAwal;

bool Hall;
int count = 0, Hz = 0, Rpm = 0 ;
int Slider;
bool Power;
int Kecepatan;

BLYNK_WRITE(V1){
  Slider = param.asInt();
}

BLYNK_WRITE(V2){
  Power = param.asInt();
  Serial.print("Power : ");
  Serial.println(Power);
}

void setup() {
  Serial.begin(9600);
  DAC.begin(0x60);
  pinMode(HallSen,INPUT);
  pinMode(Relay1,OUTPUT);
// LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Selamat Datang!");
  delay(1000);
  lcd.clear();
//  Blynk
  WiFi.begin(ssid, pass); // untuk menggantikan program Blynk.begin(auth, ssid, pass, server, port);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Blynk.config(auth);
  lcd.clear();
}

void loop() {
unsigned long currentMillis = millis();
Kecepatan = map(Slider, 0,1500, 0,4095);
DAC.setVoltage(Kecepatan, false);
Serial.print("Slider : ");
Serial.println(Slider);
Serial.print("Kecepatan: ");
Serial.println(Kecepatan); 

Hall = digitalRead(HallSen);
Serial.println(Hall);

if (currentMillis - previousMillisCheckConnection >= intervalCheckConnection) {
    previousMillisCheckConnection = currentMillis;
    CheckConnection();
  }
if (currentMillis - previousMillisSendDataBlynk >= intervalSendDataBlynk) {
    previousMillisSendDataBlynk = currentMillis;
    Serial.print("Actual RPM : ");
    Blynk.virtualWrite(V0, Rpm);
  }
if(Blynk.connected()){
    Blynk.run();    
  }
if(Power == 1){
  digitalWrite(Relay1,LOW);
}
else{
  digitalWrite(Relay1,HIGH);
}

millis();
if(millis() - waktuAwal > 1000){
    Hz = count;
    count = 0;
    Rpm = (Hz*120/4)*2; // konversi Hz ke RPM
    waktuAwal = millis();
  }
if(digitalRead(HallSen) == 0){
    count++;
    //digitalWrite(led,HIGH);
    while(digitalRead(HallSen) == 0){}
  }
else{
    //digitalWrite(led,LOW);
  }

lcd.setCursor(0,0);
lcd.print("Actual RPM :    ");
lcd.setCursor(0,1);
lcd.print(Rpm);
lcd.print("   ");
}

void CheckConnection(){    // cek koneksi blynk setiap 11 detik
  if(!Blynk.connected()){
    Serial.println("Not connected to Blynk server");
    Blynk.connect();  // mencoba ulang konek ke server
  }
  else{
    Serial.println("Connected to Blynk server");
  }
}