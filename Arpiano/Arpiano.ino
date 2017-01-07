#include <TimerFreeTone.h>


#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Include custom images
#include "images.h"
// Initialize the OLED display using brzo_i2c
// D4 -> SDA
// D5 -> SCL

const byte pwrPin = 7;  // digital pin 5 wired to '+' on BlinkM
const byte gndPin = 6;  // digital pin 4 wired to '-' on BlinkM
const byte SCLPIN = 5;  // digital pin 7 wired to 'c' on BlinkM
const byte SDAPIN = 4;  // digital pin 6 wired to 'd' on BlinkM

// Initialize the OLED display using Wire library
//SSD1306  display(0x3c, D3, D5);
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCLPIN, /* data=*/ SDAPIN, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

#include <NewPing.h>

#define SONAR_VCC 8
#define TRIGGER_PIN  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define SONAR_GND 11
#define MAX_DISTANCE 40 // 89 pitches. Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

#define SPEAKER 13
//#define BUZZ_NEG 

#include "pitch.h"

void setup()
{
  Serial.begin( 9600 );
  
  pinMode(pwrPin, OUTPUT);
  pinMode(gndPin, OUTPUT);
  digitalWrite(pwrPin, HIGH);
  digitalWrite(gndPin, LOW);

  // Initialising the UI will init the display too.
   u8g2.begin();

  pinMode(SONAR_VCC, OUTPUT);
  pinMode(SONAR_GND, OUTPUT);
  digitalWrite(SONAR_VCC, HIGH);
  digitalWrite(SONAR_GND, LOW);

  pinMode(SPEAKER, OUTPUT);
//  pinMode(BUZZ_NEG, OUTPUT);
//  digitalWrite(BUZZ_NEG, LOW);
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setFont(u8g2_font_helvR10_tf); // choose a suitable font
  u8g2.drawStr(0,13,"Arpiano");  // write something to the internal memory
}


char buff[10];
int dist;
int buzzval;
void loop()
{
  dist = sonar.ping_cm();
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.drawStr(0,27,itoa(sonar.ping_cm(),buff,10));  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
 // buzzval = (dist*1024)/50;
  //play(buzzval);
  play(dist);
  
 // delay(100);
}

int VOLUME=1023; //max 1023

void play(int ppos)
{
  if(ppos!=0)
  TimerFreeTone(SPEAKER, PITCH[ppos*2+20],500);
}


