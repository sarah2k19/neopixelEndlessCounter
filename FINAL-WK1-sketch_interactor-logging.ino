// INCLUDES:
// *****************
// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
#include <SPI.h>
#include <SD.h>

// DEFINITIONS:
//*****************
// Setup variable for logging to SD
  File myFile;
// Setup variable for real time clock
  RTC_DS1307 rtc;
// Setup the buttons START -- STOP -- TIME-up -- TIME-down
  const int ButtonStart = 2;
  const int ButtonStop = 3;
  const int ButtonUp = 7;
  const int ButtonDown = 8;
 // char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  
// GLOBAL VARIABLES:
//*****************
  int ButtonStartState = 0;
  int ButtonStopState = 0;
  int ButtonUpState = 0;
  int ButtonDownState = 0;
  
  int ButtonStartCounter;
  int lastButtonStartState = 0;
  int lastButtonStopState = 0;
  int lastButtonUpState = 0;
  int lastButtonDownState = 0;

  unsigned long lastDebounceTime = 0;
  unsigned long debounceDelay = 5; 
  int pushtime = 1000;

// CODES:
//*****************

void setup () {
// Setup the buttons START -- STOP -- TIME-up -- TIME-down
  pinMode(ButtonStart, INPUT);
  pinMode(ButtonStop, INPUT);
  pinMode(ButtonUp, INPUT);
  pinMode(ButtonDown, INPUT);
  
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
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
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
}

void loop () {

  
// Reset the button state for every loop
/*  int ButtonStartState = 0;
  int ButtonStopState = 0;
  int ButtonUpState = 0;
  int ButtonDownState = 0; */
//DEBOUNCING CODE FOR BUTTONS:
// Create variable Reading.. Button..State which read the button pin value
// read the state of the switch into a local variable
  int ReadingStart = digitalRead(ButtonStart);
  int ReadingStop = digitalRead(ButtonStop);
  int ReadingUp = digitalRead(ButtonUp);
  int ReadingDown = digitalRead(ButtonDown);
//START BUTTON
  // compare the ButtonState to its previous state
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:   
    if (ReadingStart != lastButtonStartState){
      // reset the debouncing timer
      lastDebounceTime = millis();
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (ReadingStart != ButtonStartState){
      ButtonStartState = ReadingStart;
    // only toggle the log action if the new button state is HIGH
        if (ButtonStartState == HIGH) {
          myFile = SD.open("datalog.txt", FILE_WRITE);
           if (myFile) {
              Serial.print("Writing Start to InteractorLog...");
              myFile.print("Time start: ");
              dateprint();
              // close the file:
              myFile.close();
              Serial.println("done.");
              } else {
              // if the file didn't open, print an error:
              Serial.println("error START opening datalog.txt");
              }
          } 
//            else {
//            // if the file didn't open, print an error:
//            Serial.println("error START opening and reading datalog.txt");
//            }
           delay(pushtime);
           digitalWrite(ButtonStartState, LOW);
        }
      }
    
    lastButtonStartState = ReadingStart;

// STOP BUTTON
    if (ReadingStop != lastButtonStopState){  // If the switch changed, due to noise or pressing:   
      lastDebounceTime = millis();  // reset the debouncing timer
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    
    if (ReadingStop != ButtonStopState){  // if the button state has changed:
      ButtonStopState = ReadingStop;
    
        if (ButtonStopState == HIGH) { // only toggle the log action if the new button state is HIGH
          myFile = SD.open("datalog.txt", FILE_WRITE);
           if (myFile) {
              Serial.print("Writing Stop to InteractorLog...");
              myFile.print("Time stop: ");
              dateprint();
              // close the file:
              myFile.close();
              Serial.println("done.");
      //        checklog();
              } else {
              // if the file didn't open, print an error:
              Serial.println("error STOP opening datalog.txt");
              }
            } 
//            else {
//              // if the file didn't open, print an error:
//              Serial.println("error STOP opening and reading datalog.txt");
//            }
          delay(pushtime);
          digitalWrite(ButtonStopState, LOW);
        }
      }
    lastButtonStopState = ReadingStop;

// UP BUTTON
    if (ReadingUp != lastButtonUpState){  // If the switch changed, due to noise or pressing:   
      lastDebounceTime = millis();  // reset the debouncing timer
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    
    if (ReadingUp != ButtonUpState){  // if the button state has changed:
      ButtonUpState = ReadingUp;
    
        if (ButtonUpState == HIGH) { // only toggle the log action if the new button state is HIGH
          myFile = SD.open("datalog.txt", FILE_WRITE);
           if (myFile) {
              Serial.print("Writing up to InteractorLog...");
              myFile.print("Time setting up: ");
              dateprint();
              // close the file:
              myFile.close();
              Serial.println("done.");
            } 
            else {
            // if the file didn't open, print an error:
            Serial.println("error UP opening datalog.txt");
            }
          } 
//          else {
//            // if the file didn't open, print an error:
//            Serial.println("error UP opening and reading datalog.txt");
//          }
          // checklog();
          delay(pushtime);
          digitalWrite(ButtonUpState, LOW);
        }
      }
    lastButtonUpState = ReadingUp;

// DOWN BUTTON
    if (ReadingDown != lastButtonDownState){  // If the switch changed, due to noise or pressing:   
      lastDebounceTime = millis();  // reset the debouncing timer
    }
    if ((millis() - lastDebounceTime) > debounceDelay) {
      // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    
    if (ReadingDown != ButtonDownState){  // if the button state has changed:
      ButtonDownState = ReadingDown;
    
        if (ButtonDownState == HIGH) { // only toggle the log action if the new button state is HIGH
          myFile = SD.open("datalog.txt", FILE_WRITE);
           if (myFile) {
              Serial.print("Writing down to InteractorLog...");
              myFile.print("Time setting down: ");
              dateprint();
              // close the file:
              myFile.close();
              Serial.println("done.");
            } else {
            // if the file didn't open, print an error:
            Serial.println("error DOWN opening datalog.txt");
            }
          } 
//          else {
//            // if the file didn't open, print an error:
//            Serial.println("error DOWN opening and reading datalog.txt");
//          }
          // checklog();
          delay(pushtime);
          digitalWrite(ButtonDownState, LOW);
        }
      }
    lastButtonDownState = ReadingDown;
//END DEBOUNCING CODE FOR BUTTONS:
}

void dateprint() {

//Create current time label and print it to the serial port
 
    DateTime now = rtc.now();

    myFile.print(now.year(), DEC);
    myFile.print('/');
    myFile.print(now.month(), DEC);
    myFile.print('/');
    myFile.print(now.day(), DEC);
    myFile.print(" ");
 ///  myFile.print(" (");
 //   myFile.print(daysOfTheWeek[now.dayOfTheWeek()]);
 //   myFile.print(") ");
    myFile.print(now.hour(), DEC);
    myFile.print(':');
    myFile.print(now.minute(), DEC);
    myFile.print(':');
    myFile.print(now.second(), DEC);
    myFile.println();
    delay(pushtime);
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
