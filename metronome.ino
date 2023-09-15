#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <stdio.h>

const int buzzerPin = 5;
const int buttonPin1 = 4;
const int buttonPin2 = 6;

int buttonState1 = HIGH;
int lastButtonState1 = HIGH;
int count1 = 0;

int buttonState2 = LOW;
int lastButtonState2 = LOW;
int count2 = 3;

static int pinA = 2;
static int pinB = 3;
volatile byte aFlag = 0;
volatile byte bFlag = 0;
volatile int encoderPos = 60;
volatile byte oldEncPos = 0;
volatile byte reading = 0;

LiquidCrystal lcd( 7, 8, 9, 10, 11, 12);
byte customChar[9] = {
  B00100,
  B00110,
  B00101,
  B00100,
  B00100,
  B11100,
  B11100,
  B01000
};



void setup() {
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  attachInterrupt(0,PinA,RISING);
  attachInterrupt(1,PinB,RISING);
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.createChar(0, customChar);
}



void PinA(){
  cli();
  reading = PIND & 0xC;
  if(reading == B00001100 && aFlag) {
    encoderPos --;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00000100) bFlag = 1;
  sei();
}



void PinB(){
  cli();
  reading = PIND & 0xC;
  if (reading == B00001100 && bFlag) {
    encoderPos ++;
    bFlag = 0;
    aFlag = 0;
  }
  else if (reading == B00001000) aFlag = 1;
  sei();
}


void pressed() {
  buttonState1 = digitalRead(buttonPin1);
  if (buttonState1 != lastButtonState1) {
    if (buttonState1 == LOW) {
      count1++;
      Serial.print("Button 1 Pressed. Count: ");
      Serial.println(count1);
    }
    delay(50);
  }
  lastButtonState1 = buttonState1;
}


void loop() {
  if(oldEncPos != encoderPos) {
    Serial.println(encoderPos);
    oldEncPos = encoderPos;
  }
  
  if (encoderPos < 20) {
    encoderPos = 20;
  }

  if (encoderPos > 240) {
    encoderPos = 240;
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.println("BPM: ");
  lcd.println(encoderPos);
  lcd.setCursor(0,1);
  lcd.println("MJERA: ");
  lcd.setCursor(7,1);
  lcd.println(count2+1);
  ZvukMjera(count2);
}


void ZvukMjera(int buttonPressed){
  int i = 0;
  int cursor = 8;
  pressed();
  if (count1 % 2 != 0) {
    i = 0;
    for (int i = 0; i <= buttonPressed; i++) {
      if (i == 0) {
       lcd.setCursor(cursor,1);
       lcd.write(byte(0));
       cursor++;
       tone(buzzerPin, 3000);
       delay(6000 / encoderPos);
       noTone(buzzerPin);
       delay(54000 / encoderPos);
      } else {
          lcd.setCursor(cursor,1);
          lcd.write(byte(0));
          cursor++;
          tone(buzzerPin, 2000);
          delay(6000 / encoderPos);
          noTone(buzzerPin);
          delay(54000 / encoderPos);
      }
      Serial.println("i: ");
      Serial.println(i);
      pressed();
      if (count1 %2 == 0) {
        return;
      }
      buttonState2 = digitalRead(buttonPin2);

      if (buttonState2 != lastButtonState2) {
        if (buttonState2 == LOW) {
          count2++;
          Serial.print("Button 2 Pressed. Count: ");
          Serial.println(count2);
        }
        delay(50);
      }
      // Update lastButtonState2 for button 2
      lastButtonState2 = buttonState2;
      if (count2 > 7) {
        count2 = 0;
      return;
      }
    }
  }
}