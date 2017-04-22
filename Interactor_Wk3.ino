/* This programm creates a time log after the user used buttons set time and start and stop the timer. 
The time is counting down on a LCD screen and is made visual with a led ring. 

The LCD keypad shield I used is attatched: 
see also https://www.dfrobot.com/wiki/index.php/Arduino_LCD_KeyPad_Shield_(SKU:_DFR0009)
    Analoog  0  Buttons
    Digitaal 4  DB4
    Digitaal 5  DB5
    Digitaal 6  DB6
    Digitaal 7  DB7
    Digitaal 8  RS (Data of signaal selectie beeldscherm)
    Digitaal 9  Enable
    Digitaal 10 Backlight controle

    So the buttons are analoge for this project are all on analoge 1

For datalogging we used a Datalogger shield with SD and RTC.
see also https://learn.adafruit.com/adafruit-data-logger-shield

The led lights use the pins: ????
*/

// INCLUDES:
// *****************
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h> //for LCD

// DEFINITIONS:
//*****************
// Setup variable for logging to SD
  File myFile;
// Setup variable for real time clock
  RTC_DS1307 rtc;
unsigned long StartTime = 0; // variable to establish the start of a timer in millis
unsigned long CareTime = 0; // variable to calculated the time since start of a timer in millis
unsigned long StopTime; // variable to establish the time when stop button is used
unsigned long EndTime; // variable to establish the time when countdown timer ended
int PrintTimer;

// Setup variable for count down timer
int hours = 0; // start hours
int minutes = 1; //start min
int seconds = 0; //start seconds

int count;
/*************************************************************************************
Mark Bramwell, July 2010
  This program will test the LCD panel and the buttons.When you push the button on the shield，
  the screen will show the corresponding one.
Connection: Plug the LCD Keypad to the UNO(or other controllers)
**************************************************************************************/
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);           // select the pins used on the LCD panel

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int read_LCD_buttons(){               // read the buttons
    adc_key_in = analogRead(0);       // read the value from the sensor 
     if (adc_key_in > 1000) return btnNONE;    
     if (adc_key_in < 50)   return btnRIGHT;  
     if (adc_key_in < 195)  return btnUP; 
     if (adc_key_in < 380)  return btnDOWN; 
     if (adc_key_in < 555)  return btnLEFT; 
     if (adc_key_in < 790)  return btnSELECT;   
    return btnNONE;                // when all others fail, return this.
}

void setup(){
// Setup Serial communication  
  while (!Serial); // for Leonardo/Micro/Zero
  Serial.begin(57600);

// Setup RTC
  //Serial.print("Initializing RTC...");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
  }
   Serial.println("initialization RTC done.");

// Setup SD 
    Serial.print("Initializing SD card...");

  if (!SD.begin()) {
    Serial.println("initialization SD failed!");
    return;
  }
  Serial.println("initialization SD done.");

// Create file for logging
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("datalog.txt", FILE_WRITE);
  Serial.print("Writing to InteractorLog...");
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.print("Start Logging sequence: ");
    dateprint();
    // close the file:
    myFile.close();
    Serial.println("done.");
    // checklog();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening datalog.txt");
  }
  
   lcd.begin(16, 2);               // start the library
   lcd.setCursor(0,0);             // set the LCD cursor position 
   lcd.print("Interactor v3.0");  
   lcd.setCursor(0,1);              
   lcd.print("READY"); 
   delay(3000);
   lcd.setCursor(0,0);              
   lcd.print("Set time          "); 
   lcd.setCursor(0,1);              
   lcd.print("                  "); 
}

void loop(){
   lcd.setCursor(0,0);             // move to the begining of the first line
   lcd_key = read_LCD_buttons();   // read the buttons
   switch (lcd_key){               // depending on which button was pushed, we perform an action
       case btnRIGHT:{             //  push button "RIGHT" and show the word on the screen
          lcd.print("START          ");
             StartTime = millis();  // input interactor: variable StartTime get value
             myFile = SD.open("datalog.txt", FILE_WRITE);          
           // logging the time Start was initiated by the user to the SD file
           if (myFile) { 
              Serial.print("Writing Start to InteractorLog...");

              myFile.print("Time start: "); // logging the starting time of the timer
              dateprint() ;
              
              myFile.print("Time counter setting: "); // logging the settings of the timer
              logCounterDown();

              stepDown(); // counting down the time
              trigger();  // actions when timer is zero           
              myFile.close();
 
              Serial.println("done.");
            
            } else {
              // if the file didn't open, print an error:
              Serial.println("error START opening datalog.txt");
              }
            break;
   }
       case btnLEFT:{
        stopTimer();
             break;
       }    
       case btnUP:{
             lcd.setCursor(0,0);
             lcd.print("Setting time");  //  push button "UP" and show the value of StartTime on the screen
             lcd.print("                ");
             minutes++;
             counterDown();
             delay(500);
             break;
       }
       case btnDOWN:{
             lcd.setCursor(0,0);
             lcd.print("Setting time");  //  push button "DOWN" and show the word on the screen
             lcd.print("                ");
             minutes--;
             counterDown();
             delay(500);
             break;
       }
       case btnSELECT:{
             lcd.print("CHECKLOG        ");  //  push button "SELECT" and show the word on the screen
             checklog();
             break;
       }
       case btnNONE:{
             break;
       }
   }
}

void dateprint() {
//Create current time label and print it to the SD card and LCD
    DateTime now = rtc.now();
    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(" ");
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.println();
}

void stopTimer(){
             lcd.print("STOP             "); //  push button "LEFT" and show the word on the screen
             seconds = 0;
 
             myFile.close(); // in case of interuption START... data is saved
             myFile = SD.open("datalog.txt", FILE_WRITE);
           
           // logging the ellapsed time after Start was initiated by the user, to the SD file
           if (myFile) {
              Serial.print("Writing Stop to InteractorLog...");

              myFile.print("Time stop: ");
              dateprint();
              myFile.println("  ");
              
            // close the file:
              myFile.close();
               delay(2000);
               lcd.setCursor(0,0);              
               lcd.print("Set time          "); 
               lcd.setCursor(0,1);              
               lcd.print("                  "); 
              Serial.println("done.");
              } else {
              // if the file didn't open, print an error:
              Serial.println("error STOP opening datalog.txt");
              }
             delay(500);
  }

void checklog (){
     // re-open the file for reading and check during programming:
        myFile = SD.open("datalog.txt");
        if (myFile) {
          Serial.println("datalog.txt:");
          // read from the file until there's nothing else in it:
          while (myFile.available()) {
            Serial.write(myFile.read());
          }
          // close the file:
          myFile.close();
        }
}

void counterUp (){ 
      // create variable CareTime to store elapsed time after START is pressed.
      if (StartTime > 0) {CareTime = (millis() - StartTime);}       
      
      if (CareTime > 1000) {
        PrintTimer = CareTime/1000;
        lcd.setCursor(0,9);
        lcd.print(PrintTimer);  //visualize Caretime in seconds on LCD
        lcd.print("     ");  // clearing previous parts of LCD
      } 
}

void logCounterDown() {
 (minutes < 10) ? myFile.print("0") : NULL;
 myFile.print(minutes);
 myFile.print(":");
 (seconds < 10) ? myFile.print("0") : NULL;
 myFile.print(seconds);
 myFile.println();
}
 
void stepDown() {
  while (hours > 0 || minutes > 0 || seconds >= 0) {
    counterDown();
    delay(1000); 
 if (seconds >= 0) {
 seconds -= 1;
 } else {
 if (minutes > 0) {
 seconds = 59;
 minutes -= 1;
 } 
 }
 }
}

void counterDown() {
  lcd.setCursor(0,2);
  // (hours < 10) ? lcd.print("0") : NULL;
  // lcd.print(hours);
  // lcd.print(":");
  (minutes < 10) ? lcd.print("0") : NULL;
  lcd.print(minutes);
  lcd.print(":");
  (seconds < 10) ? lcd.print("0") : NULL;
  lcd.print(seconds);
  lcd.print("                 ");
  }
 
void trigger() {
 //lcd.clear(); // clears the screen and buffer

//  myFile = SD.open("datalog.txt", FILE_WRITE);
//// logging the time CountDown ended to the SD file
//   if (myFile) { 
      myFile.print("Time countdown ended: ");
      dateprint() ;
//      myFile.close();
//   }
 seconds = 0;
 lcd.setCursor(0,0); // set timer position on lcd for end.
 lcd.print("TIMER END           ");
 loop();
}
