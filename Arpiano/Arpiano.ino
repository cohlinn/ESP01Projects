/***
 * Arduino UNO project.
 * 
 * Objective : 
 * 1. To build entire project without breadboard or connectors.
 * 2. Has interaction (input and output).
 * 3. Use standard electronic parts (no shields).
 * 
 * Features :
 * 1) e-ruler - Measures distance and display on OLED screen (0 - 400 cm)
 * 2) e-music - Converts distance to sound (0 - 40 cm, 40 notes)
 * 3) Midi player - plays music
 * Author: Cathy Ooi (cohlinn)
 */
#include <NewTone.h> //prevent conflict of interrupt pins with OLED

#include <Arduino.h>
#include <U8g2lib.h>  //to allow flexibility to change OLED pins
#include <Wire.h>

// Include custom images
#include "images.h"
const byte pwrPin = 7;  //VCC POWER 5V --> HIGH
const byte gndPin = 6;  //GND --> LOW
const byte SCLPIN = 5;  //SCL
const byte SDAPIN = 4;  //SDA

// Initialize the OLED display using Wire library
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCLPIN, /* data=*/ SDAPIN, /* reset=*/ U8X8_PIN_NONE);   // OLED .98' 128x32 screen

#include <NewPing.h>
#define SONAR_VCC 8
#define TRIGGER_PIN  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     10  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define SONAR_GND 11
#define DIST_MAX_DISTANCE 400 // 89 pitches. Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define MUSIC_MAX_DISTANCE 40 // 89 pitches. Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

#define SPEAKER 13
#include "pitch.h"

//Potentiometer switch
#define POT_VCC A5
#define POT_GND A1
#define POT_IN A3 //Analog Input pin

void setup()
{
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
  u8g2.setFont(u8g2_font_helvR10_tf); // choose a suitable font

  pinMode(POT_VCC, OUTPUT);
  pinMode(POT_GND, OUTPUT);
  digitalWrite(POT_VCC, HIGH);
  digitalWrite(POT_GND, LOW);
}


char buff[10];
int dist;
int buzzval;
int potval;
int opt = 0;
void loop()
{
  u8g2.clearBuffer();          // clear the internal memory
  potval = analogRead(POT_IN);
  opt = getOpt();
  noNewTone(SPEAKER);
  
  switch(opt)
  {
    case 1: default: eRuler(); break;
    case 2: eMusic(); break;
    case 3: playMusic("Star Wars", SONG_SW, 18); break;
    case 4: playMusic("Star Trek: TNG", SONG_TNG, 43); break;
    case 5: playMusic("Doctor Who", SONG_DRW, 16); break;
  }
  
}

//Convert Pot value into options
int getOpt ()
{
  if(potval < 100) return 1;
  else if(potval < 200) return 2;
  else if(potval < 300) return 3;
  else if(potval < 400) return 4;
  else if(potval < 500) return 5;
  else return 0;
}

void eRuler()
{
  NewPing sonar(TRIGGER_PIN, ECHO_PIN, DIST_MAX_DISTANCE); // NewPing setup of pins and maximum distance.
  dist = sonar.ping_cm();
  u8g2.drawStr(0,13,"e-Ruler");  // write something to the internal memory
  u8g2.drawStr(60,13,itoa(potval,buff,10));  // write something to the internal memory
  u8g2.drawStr(100,13,itoa(opt,buff,10));  // write something to the internal memory
  u8g2.drawStr(0,27,itoa(sonar.ping_cm(),buff,10));  // write something to the internal memory
  u8g2.drawStr(60,27,"cm");  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(100);
}

void eMusic()
{
  u8g2.drawStr(0,13,"Arpiano");  // write something to the internal memory
  u8g2.drawStr(60,13,itoa(potval,buff,10));  // write something to the internal memory
  u8g2.drawStr(100,13,itoa(opt,buff,10));  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display

  NewPing sonar(TRIGGER_PIN, ECHO_PIN, MUSIC_MAX_DISTANCE); // NewPing setup of pins and maximum distance.
  while(getOpt() == 2)
  {
    dist = sonar.ping_cm();
    play(dist);
    potval = analogRead(POT_IN);
    delay(50);
  }
}

void playMusic(char song[], int number, int sumNotes)
{
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.drawStr(0,13,"Arpiano");  // write something to the internal memory
  u8g2.drawStr(60,13,itoa(potval,buff,10));  // write something to the internal memory
  u8g2.drawStr(100,13,itoa(opt,buff,10));  // write something to the internal memory
  u8g2.drawStr(0,27,song);  // write something to the internal memory
  u8g2.sendBuffer();          // transfer internal memory to the display

  int *notes;
  int *dur;
  switch(number)
  {
    case SONG_SW: 
      notes = SWnotes;
      dur   = SWdur;
      break;
    case SONG_TNG: 
      notes = TNGnotes;
      dur   = TNGdur;
      break;
    case SONG_DRW: 
      notes = DRWnotes;
      dur   = DRWdur;
      break;
    default : return;
  }
  int newopt = 0;
  for(int i=0; i<sumNotes; i++)
  {
    potval = analogRead(POT_IN);
    u8g2.drawStr(60,13,itoa(potval,buff,10));  // write something to the internal memory
    u8g2.sendBuffer();          // transfer internal memory to the display
    newopt = getOpt();
    if(newopt!=opt) 
    { 
      noNewTone(SPEAKER);
      break;
    }
    
    NewTone(SPEAKER, notes[i]);
    delay( dur[i]);
    noNewTone(SPEAKER);
  }
  noNewTone(SPEAKER);
}

int OFFSET=0;
void play(int ppos)
{
  if(ppos!=0)
  NewTone(SPEAKER, PITCH[(ppos*1)+OFFSET]);
  else
  noNewTone(SPEAKER);
}


