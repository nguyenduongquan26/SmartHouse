#include <Arduino.h>
#include <EEPROM.h>
int address = 0;
byte value;

#include "BasicStepperDriver.h"
#define MOTOR_STEPS 200
#define RPM 120
#define MICROSTEPS 1
#define DIR 8
#define STEP 9
#define ENABLE 13
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);
#include <SoftwareSerial.h>
#include <Wire.h> 
SoftwareSerial DebugSerial(2,3);
#define BLYNK_PRINT DebugSerial
#include <BlynkSimpleStream.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,16,2);

char auth[]= "1ab349adede74dc383a4c47c3c3c0c7d";
#define sensor A0
#define PinAnalog1 A1
#define PinAnalog2 A2
#define LED 6
#define BUTTON_OPEN 4
#define BUTTON_CLOSE 5
int val1,val2,val3,val4;
int Steped=0,Step,MODE=0,BLYNK_MODE=0;
int BLYNK_BUTTON1=0,BLYNK_BUTTON2=0;
BLYNK_WRITE(V1){
  int virtualpin1 = param.asInt();
    if ( virtualpin1 == 1 ) {
      BLYNK_MODE = 1;
      Blynk.virtualWrite(V7,"Blynk Control");
      }
    else { if ( virtualpin1 == 0 ){
      BLYNK_MODE = 0;
      Blynk.virtualWrite(V7,"Button Control");
      }
    }
  }


BLYNK_WRITE(V2){
  int virtualpin2 = param.asInt();
    if ( virtualpin2 == 1 && BLYNK_MODE == 1 ) {
    BLYNK_BUTTON1 = 1;
    Blynk.virtualWrite(V5,"Man cua mo");
    }
   else {
        if ( virtualpin2 == 0 && BLYNK_MODE == 1 ) {
    BLYNK_BUTTON1 = 0;
    Blynk.virtualWrite(V5," ");
    }   
   }
  }
BLYNK_WRITE(V3){
  int virtualpin3 = param.asInt();
  if ( virtualpin3 == 1 && BLYNK_MODE == 1 ){
    BLYNK_BUTTON2 = 1;
    Blynk.virtualWrite(V5,"Man cua dong");
    }
  else {
      if ( virtualpin3 == 0 && BLYNK_MODE == 1 ){
    BLYNK_BUTTON2 = 0;
    Blynk.virtualWrite(V5," ");
    }
    }
  }

BLYNK_WRITE(V4){
  int virtualpin3 = param.asInt();
  if (BLYNK_MODE == 1){
    analogWrite(LED,virtualpin3);
    }
  
  }





void setup()
{
DebugSerial.begin(9600);
Serial.begin(9600);
Blynk.begin(Serial, auth);
stepper.begin(RPM, MICROSTEPS);
value = EEPROM.read(address);
stepper.move(-value);
value = 0;
EEPROM.write(address,value);
lcd.init();  
lcd.backlight();
pinMode(LED,OUTPUT);
pinMode(sensor,INPUT);
pinMode(PinAnalog1,INPUT);
pinMode(BUTTON_OPEN,INPUT_PULLUP);
pinMode(BUTTON_CLOSE,INPUT_PULLUP);
pinMode(PinAnalog2,INPUT);
}




void loop()
{
  Blynk.run();
  stepper.enable();
  val1 = getAVRvalue(PinAnalog1);
  val2 = getAVRvalue(PinAnalog2);
  val3 = getAVRvalue(sensor);
  val4 = getAVR(PinAnalog1);
  Blynk.virtualWrite(V6,val3);
  if ( BLYNK_MODE == 0){
  if (val2 < 50 ){
    MODE = 0;
  }
  if (val2 >= 50 ){
    MODE = 1;
  }
  
  }
  else if ( BLYNK_MODE == 1){
    MODE = 0;
  }
  
  value = EEPROM.read(address);
if (MODE == 1){
 automode();
}

else{   if ( MODE == 0){
  manualmode();
  }
}

/*
  Serial.print(val1);
  Serial.print("-------------");
  Serial.print(val2);
  Serial.print("-------------");
  Serial.print(val3);
  Serial.print("-------------");
  Serial.print(val4);
  Serial.print("-------------");
  Serial.print("Value in eeprom = ");
  Serial.println(value);
  */
  showLCD(val2);
}
void automode(){
   if (val3 < 20){
    analogWrite(LED,255);
    Blynk.virtualWrite(V5,"Man cua mo");
    int temp1 = 200 - value;
    int i;
    while ( i < temp1){
      stepper.move(1);
       EEPROM.write(address,temp1+value);
      delay(15);
      i++;
      }
    
   
    }
  else{ 
    analogWrite(LED,80);
    Blynk.virtualWrite(V5,"Man cua dong");
    int temp2 = - value ;
    int j;
    while ( j < abs(temp2)){
      stepper.move(-1);
      EEPROM.write(address,temp2+value);
      delay(15);
      j++;
      }
    
    
    }
  }
  
void manualmode(){
if ( BLYNK_MODE == 1){
  if( BLYNK_BUTTON1 == 1){
    if ( Steped <= 190){
    Steped = EEPROM.read(address);
    stepper.move(10);
    Steped +=10;
//    Serial.println(Steped);
    EEPROM.write(address,Steped);
    delay(5);
    }
  }
  else { 
    if( BLYNK_BUTTON2 == 1){
          if ( Steped > 0 ){
      Steped = EEPROM.read(address);
      stepper.move(-10);
      Steped -=10;
 //     Serial.println(Steped);
      EEPROM.write(address,Steped);
      delay(5);
    }
  }
}
}
else { if ( BLYNK_MODE == 0){
    if (debounce(BUTTON_OPEN) == 0){
    if ( Steped <= 190){
    Steped = EEPROM.read(address);
    stepper.move(10);
    Steped +=10;
//    Serial.println(Steped);
    EEPROM.write(address,Steped);
    delay(5);
    }
    }
  else{  
    
    if (debounce(BUTTON_CLOSE) == 0){
      if ( Steped > 0 ){
      Steped = EEPROM.read(address);
      stepper.move(-10);
      Steped -=10;
 //     Serial.println(Steped);
      EEPROM.write(address,Steped);
      delay(5);
      
  }
  }
  }
}
  if (!BLYNK_MODE == 1){
    analogWrite(LED,val4);
  }
  }
}
void showLCD(int val){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Anh Sang =");
  lcd.setCursor(12,0);
  lcd.print(val3);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Den=");
  lcd.setCursor(4,1);
  lcd.print(val1);
  lcd.print("%");
  lcd.setCursor(8,1);
  lcd.print("Mode:");
  if (val > 50){
    lcd.print("A");
    }
  else {
    lcd.print("M");
  }
  }
int getAVR(int a)
{
  int i = 0;
  int anaValue = 0;
  for (i = 0; i < 10; i++)  //
  {
    anaValue += analogRead(a); 
    delay(10);   
  }
  
  anaValue = anaValue / 10;
  anaValue = map(anaValue, 1023, 0, 0, 255);
  return anaValue;
}

int getAVRvalue(int anaPin)
{
  int anaValue = 0;
  for (int i = 0; i < 10; i++) // Đọc giá trị cảm biến 10 lần và lấy giá trị trung bình
  {
    anaValue = anaValue + analogRead(anaPin);
    delay(10);
  }

  anaValue = anaValue / 10;
  anaValue = map(anaValue, 1023, 0, 0, 100); //Tối:0  ==> Sáng 100%

  return anaValue;
}


boolean debounce(int pinIN){
  boolean state;
  boolean previousState;
  const int debounceDelay = 10;

  previousState = digitalRead(pinIN);
  for (int counter = 0; counter < debounceDelay; counter++)
  {
    delay(1);
    state = digitalRead(pinIN);
    if (state != previousState)
    {
      counter = 0;
      previousState = state;
    }
  }
  return state;
}  

