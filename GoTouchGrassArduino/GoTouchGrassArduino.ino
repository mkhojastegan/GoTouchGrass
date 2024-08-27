#define USE_ARDUINO_INTERRUPTS true
#include "Arduino.h"
//#include "HackPublisher.h"
#include "constants.h"
#include <PulseSensorPlayground.h>
#include "DFRobotDFPlayerMini.h"

// Credit to https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299 documentation for MP3 Module

// Create PulseSensorPlayground object
PulseSensorPlayground pulseSensor;


#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/RX_FOR_MP3, /*tx =*/TX_FOR_MP3);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

DFRobotDFPlayerMini myDFPlayer;

/* 
Function definitions. Unfortunately, Arduino IDE
does not accept most header/C files.
*/
void setupProject();
int startStory(unsigned long startTime);
int midStory(unsigned long chapter);
int endStory(unsigned long chapter);
int readButton();
void checkHeartbeat();
void printDetail(uint8_t type, int value);
void BPMLightsConfigure(int currentBPM);


void setup()
{
#if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/D3, /*tx =*/D2);
#else
  FPSerial.begin(9600);
#endif

  Serial.begin(9600);

  setupProject();
}

void loop()
{
  // MP3 Player
  static unsigned long timer = millis();

  if(!startStoryDone){
    int branch = startStory(timer);
    if (branch == -1){
      return;
    }
  }

  if(!midStoryDone){
    int branch = midStory(currentBranch);
    if(branch == -1){
      return;
    }
  }

  if(!endStoryDone){
    int branch = endStory(currentBranch);
    if(branch == -1){
      return;
    }
  }


  /*
  Debug information
  */

  // Debug MP3 Player
  // if (myDFPlayer.available()) {
  //   printDetail(myDFPlayer.readType(), myDFPlayer.read()); //Print the detail message from DFPlayer to handle different errors and states.
  // }

  // Serial.print("MP3Player State: ");
  // Serial.println(myDFPlayer.readState()); //read mp3 state

  // Debug Pulse sensor
  // Serial.print("Current pulse: ");
  // Serial.println(currentBPM);


  delay(500);
}

// Sets up project sensors and Arduino Uno
void setupProject(){
  pinMode(LED_BLUE,OUTPUT);
  pinMode(LED_GREEN,OUTPUT);
  pinMode(LED_YELLOW,OUTPUT);
  pinMode(LED_RED,OUTPUT);

  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }
  Serial.println(F("DFPlayer Mini online."));

  //Pulse sensor
  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  // pulseSensor.blinkOnPulse(LED_BLUE);
  pulseSensor.setThreshold(READ_THRESHOLD);

  // Check pulsesensor
  if(pulseSensor.begin()){
    Serial.println("PulseSensor object created successfully!");
  }

  myDFPlayer.volume(MP3_VOLUME);  //Set volume value. From 0 to 30
  myDFPlayer.play(1);  //Play the first mp3
}

/*
Starts story and determines which branch to go next,
returns -1 if error with myDFPlayer
*/
int startStory(unsigned long startTime){
  if (myDFPlayer.available()){
    if (millis() - startTime > 3000){
      // Play welcome sound, rules
      myDFPlayer.playFolder(1, 1); // Welcome to the game
      myDFPlayer.playFolder(1, 2); // Rules
      myDFPlayer.playFolder(2, 1); // Beginning of story
      checkHeartbeat();
      myDFPlayer.playFolder(2, 2); // Story pt. 2
      checkHeartbeat();
      myDFPlayer.playFolder(2, 3); // Story pt. 3
      checkHeartbeat();
      myDFPlayer.playFolder(2, 4); // Story pt. 4
      checkHeartbeat();
      myDFPlayer.playFolder(2, 5); // Branching decision

      while(readButton() != 0 && readButton() != 3){
        int current = readButton();
        switch(current){
          case 1:
            currentBranch = 3;
            break;
          case 2:
            currentBranch = 4;
            break;
        }
      }

      return 0;
    }
    
  } else {
    return -1;
  }
}

int midStory(unsigned long chapter){
  if (myDFPlayer.available()){
    switch(chapter){
      case 3:
        // Branch 3
        myDFPlayer.playFolder(3, 1);
        checkHeartbeat();
        myDFPlayer.playFolder(3, 2);
        checkHeartbeat();
        myDFPlayer.playFolder(3, 3);
        checkHeartbeat();
        myDFPlayer.playFolder(3, 4); // Branching decision

        while(readButton() != 0){
          int current = readButton();
          switch(current){
            case 1:
              currentBranch = 4;
              break;
            case 2:
              currentBranch = 5;
              break;
            case 3:
              currentBranch = 10;
              midStoryDone = true;
              break;
          }
        }
        break;
      case 4:
        // Branch 4
        myDFPlayer.playFolder(4, 1);
        checkHeartbeat();
        myDFPlayer.playFolder(4, 2);
        checkHeartbeat();
        myDFPlayer.playFolder(4, 3); // Branching decision

        while(readButton() != 0){
          int current = readButton();
          switch(current){
            case 1:
              currentBranch = 11;
              midStoryDone = true;
              break;
            case 2:
              currentBranch = 6;
              break;
            case 3:
              currentBranch = 10;
              midStoryDone = true;
              break;
          }
        }
        break;
      case 5:
        // Branch 5
        myDFPlayer.playFolder(5, 1);
        checkHeartbeat();
        myDFPlayer.playFolder(5, 2);
        checkHeartbeat();
        myDFPlayer.playFolder(5, 3);
        checkHeartbeat();
        myDFPlayer.playFolder(5, 4); // Branching decision

        while(readButton() != 0 && readButton() != 3){
          int current = readButton();
          switch(current){
            case 1:
              currentBranch = 10;
              midStoryDone = true;
              break;
            case 2:
              currentBranch = 6;
              break;
          }
        }
        break;
      case 6:
        myDFPlayer.playFolder(6, 1);
        checkHeartbeat();
        myDFPlayer.playFolder(6, 2); // Branching decision
        checkHeartbeat();

        while(readButton() != 0 && readButton() != 3){
          int current = readButton();
          switch(current){
            case 1:
              currentBranch = 10;
              midStoryDone = true;
              break;
            case 2:
              currentBranch = 20; // Winning branch
              midStoryDone = true;
              break;
          }
        }
        break;
      default:
        return -1;
        break;
    }
    return 0;
  } else {
    return -1;
  }
}

/*
Ends story,
returns -1 if error with myDFPlayer
*/
int endStory(unsigned long chapter){
  if(myDFPlayer.available()){
    switch(currentBranch){
      case 10:
        myDFPlayer.playFolder(10, 1);
        myDFPlayer.playFolder(10, 2);
        myDFPlayer.playFolder(10, 3);

        endStoryDone = true;
        break;
      case 11:
        myDFPlayer.playFolder(11, 1);
        myDFPlayer.playFolder(11, 2);

        endStoryDone = true;
        break;
      case 20:
        // Win!
        myDFPlayer.playFolder(20, 1);
        myDFPlayer.playFolder(20, 2);
        myDFPlayer.playFolder(20, 3);

        endStoryDone = true;
        break;
    }
    return 0;
  } else {
    return -1;
  }
}

// Returns current button pressed, 0 if none
int readButton(){
  int buttonState1 = digitalRead(BUTTON_PIN_1);
  int buttonState2 = digitalRead(BUTTON_PIN_2);
  int buttonState3 = digitalRead(BUTTON_PIN_3);

  if (buttonState1 == HIGH){
    return 1;
  } else if (buttonState2 == HIGH) {
    return 2;
  } else if (buttonState3 == HIGH) {
    return 3;
  } else {
    return 0;
  }
}

/*
Checks heartbeat and changes story and sensors accordingly
*/
void checkHeartbeat() {
  currentBPM = pulseSensor.getBeatsPerMinute();
  BPMLightsConfigure(currentBPM);
  if(currentBPM < MED_THRESHOLD){
    myDFPlayer.playFolder(1, 3); // "You're running too slowly!"
  }
}

// Prints messages according to state of MP3 Player
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

}

// Configures lights according to heartbeat and thresholds
void BPMLightsConfigure(int currentBPM){
  if(currentBPM > VERY_HIGH_THRESHOLD){ // VERY HIGH = RED
     digitalWrite(LED_BLUE, LOW);
     digitalWrite(LED_GREEN, LOW);
     digitalWrite(LED_YELLOW, LOW);
     digitalWrite(LED_RED, HIGH);
  } else if (currentBPM > HIGH_THRESHOLD) { // HIGH = YELLOW
     digitalWrite(LED_BLUE, LOW);
     digitalWrite(LED_GREEN, LOW);
     digitalWrite(LED_YELLOW, HIGH);
     digitalWrite(LED_RED, LOW);
  } else if(currentBPM > MED_THRESHOLD){ // MEDIUM = GREEN
     digitalWrite(LED_BLUE, LOW);
     digitalWrite(LED_GREEN, HIGH);
     digitalWrite(LED_YELLOW, LOW);
     digitalWrite(LED_RED, LOW);
  } else if (currentBPM > LOW_THRESHOLD){ // LOW = BLUE
    digitalWrite(LED_BLUE, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  } else {                               // NOTHING = NOTHING (surprising right?)
    digitalWrite(LED_BLUE, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }
}