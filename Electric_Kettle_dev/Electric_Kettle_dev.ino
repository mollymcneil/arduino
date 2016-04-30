#include <OneWire.h>
#include <DallasTemperature.h>
#include <CapacitiveSensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>

//LED strip
#define NUM_LEDS 8
#define DATA_PIN 7


//OLED
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//Temp probe section
#define ONE_WIRE_BUS 12 // temp: Data wire is plugged into pin 12 on the Arduino
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


//Capacitive Sensor
CapacitiveSensor   cs_4_2 = CapacitiveSensor(4,2);        // 10 megohm resistor between pins 4 & 2, pin 2 is sensor pin, add wire, foil
bool capMode = 0;
bool thingsOn = 0;

//Button
int buttonState;

const int buttonPin = 3; //plug button into pin #3

//relay
int relayLEDpin = 6;
int relayPin = 5;
unsigned long lastToggledRelay = 0;
int relayOn = 0;

//RBG LED
const int greenLEDPin = 9;
const int redLEDPin = 11;
const int blueLEDPin = 10;

      
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

//yellow LED
//const int yellowLEDPin = 6;

//Potentiometer
int potPin = 1;
float potVal = 0;
float tempVal;

//LED strip
CRGB leds[NUM_LEDS];


//Piezo
unsigned int numBeep = 0;
const int piezoPin = 8;


void setup() {
  //for serial monitor
  Serial.begin(9600);

  //Capcitive Sensor
   cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example

  //Button
  pinMode(buttonPin, INPUT);
  
  //Temp probe
  sensors.begin(); 

  
  //LED section
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  
  //RGB LED
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);

  //yellow LED
  //pinMode(yellowLEDPin, OUTPUT);

  //OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)

  display.clearDisplay();

  display.display();

}

void loop() {


    
  //Capacitive Sensor
  long start = millis();
  long capVal =  cs_4_2.capacitiveSensor(30);

  Serial.print("Millis: ");
  Serial.print(millis() - start);        // check on performance in milliseconds
  Serial.print("\t");                    // tab character for debug window spacing
  Serial.print("Cap Val: ");
  Serial.print(capVal);                  // print sensor output 1

  delay(10);                             // arbitrary delay to limit data to serial port 

  //Button
  buttonState = digitalRead(buttonPin);

  Serial.print("Button: ");
  Serial.print(buttonState);
  
  //Temperature probe section
  sensors.requestTemperatures(); 
  tempVal = sensors.getTempCByIndex(0);

  //print to the serial monitor for debugging
  Serial.print("\t");
  Serial.print("Temp Is: ");
  Serial.print(tempVal);
  Serial.print(" C; ");

  pinMode(relayLEDpin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  Serial.print("\t");
  Serial.print("Relay: ");
  Serial.print(relayOn);
  
  //Potentiometer
  //maps the range of the potentiometer from 50 to 100 (don't know why map fxn did not work)
   potVal = (int)(analogRead(potPin) / 1.024) / 16.666 + 40.0;

  //print to the serial monitor for debubbing
  Serial.print("\t");
  Serial.print("Set Temp: ");
  Serial.print(potVal);
  Serial.print(" C; ");
  Serial.print("\n");


  // Capacitance mode switcher
  if (capVal > 1500) {
    if (capMode == 0) {
      thingsOn = !thingsOn;
    }
    capMode = 1;
  }
  else {
    capMode = 0;
  }

  
  //Button mode switcher
/*
  if (buttonState == LOW && capMode == 0) {
      //if off off, turn on kettle led and heating
      thingsOn = 1;
    }
  else if (buttonState == LOW && capMode == 1) {
      //if already on, show LEDs off show that kettle is off the base
      
      leds[0] = CRGB::Black;
      leds[1] = CRGB::Black;
      leds[2] = CRGB::Black;
      leds[3] = CRGB::Black;
      leds[4] = CRGB::Black;
      leds[5] = CRGB::Black;
      leds[6] = CRGB::Black;
      leds[7] = CRGB::Black;
      FastLED.show(); 
      
  }
  
  */
  
  //touch capacitive sensor
  if (thingsOn == 1) {
    
      analogWrite(redLEDPin, 0);
      analogWrite(greenLEDPin, 0);
      analogWrite(blueLEDPin, 1);

      // Display set temp on OLED
      display.clearDisplay();
      display.setCursor(18,16);
      display.setTextSize(5);
      display.setTextColor(WHITE);
  
      display.print((int)potVal, DEC);
      
      display.drawCircle(83, 16, 4, WHITE);
      
      display.setCursor(88,16);
  
      display.print("C");
      display.display();
  

    
  
      //relay section
      Serial.print("Check: \t");
      Serial.print(millis());
      Serial.print("Last toggled: \t");
      Serial.print(lastToggledRelay);
      Serial.print("\n");
  
      if (lastToggledRelay == 0 || (millis() > (lastToggledRelay + 15000))) {
    
        if (tempVal > potVal && relayOn == 1) {
            digitalWrite(relayLEDpin, 0);
            digitalWrite(relayPin, 0);
            relayOn = 0;
            lastToggledRelay = millis();
            
        }
        else if (tempVal < potVal && relayOn == 0) {
            digitalWrite(relayLEDpin, 1);
            digitalWrite(relayPin, 1);
            relayOn = 1;
            lastToggledRelay = millis();
        }
      }
      
      //RGB LED
      int stepVal = (potVal-20)/8;
      char ledCol = 'CHSV( 128, 255, 100)';
      int delTim = 400;
      char ledGoal = 'CRGB::White';
   
      if (tempVal < potVal && relayOn == 1) {
        
        if (tempVal < (potVal - stepVal*7)){
          leds[0] = ledCol;
          FastLED.show();
          delay(delTim);
              leds[0] = ledCol;
              leds[1] = CRGB::Black;
              leds[2] = CRGB::Black;
              leds[3] = CRGB::Black;
              leds[4] = CRGB::Black;
              leds[5] = CRGB::Black;
              leds[6] = CRGB::Black;
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
              leds[0] = CRGB::Black;
              FastLED.show();
              delay(delTim);
          }
          else if (tempVal < (potVal - stepVal*6)){
              leds[0] = ledCol;
              leds[1] = ledCol;
              leds[2] = CRGB::Black;
              leds[3] = CRGB::Black;
              leds[4] = CRGB::Black;
              leds[5] = CRGB::Black;
              leds[6] = CRGB::Black;
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
              leds[1] = CRGB::Black;
              FastLED.show();
              delay(delTim);
           }
           else if (tempVal < (potVal - stepVal*5)){
              leds[0] = ledCol;
              leds[1] =ledCol;
              leds[2] = ledCol;
              leds[3] = CRGB::Black;
              leds[4] = CRGB::Black;
              leds[5] = CRGB::Black;
              leds[6] = CRGB::Black;
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
              leds[2] = CRGB::Black;
              FastLED.show();
              delay(delTim);
           }
           else if (tempVal < (potVal - stepVal*4)){
              leds[0] = ledCol;
              leds[1] = ledCol;
              leds[2] = ledCol;
              leds[3] = ledCol;
              leds[4] = CRGB::Black;
              leds[5] = CRGB::Black;
              leds[6] = CRGB::Black;
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
              leds[3] = CRGB::Black;
              FastLED.show();
              delay(delTim);
           }
           else if (tempVal < (potVal - stepVal*3)){
              leds[0] = ledCol;
              leds[1] = ledCol;
              leds[2] = ledCol;
              leds[3] = ledCol;
              leds[4] = ledCol;
              leds[5] = CRGB::Black;
              leds[6] = CRGB::Black;
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
              leds[4] = CRGB::Black;
              FastLED.show();
              delay(delTim);
           }
           else if (tempVal < (potVal - stepVal*2)){
              leds[0] = ledCol;
              leds[1] = ledCol;
              leds[2] = ledCol;
              leds[3] = ledCol;
              leds[4] = ledCol;
              leds[5] = ledCol; 
              leds[6] = CRGB::Black;
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
              leds[5] = CRGB::Black;
              FastLED.show();
              delay(delTim);
           }
           else if (tempVal < (potVal - stepVal*1)){
              leds[0] = ledCol;
              leds[1] = ledCol;
              leds[2] = ledCol;
              leds[3] = ledCol;
              leds[4] = ledCol;
              leds[5] = ledCol;       
              leds[6] = ledCol;
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
              leds[6] = CRGB::Black;
              FastLED.show();
              delay(delTim);
           }
           else if (tempVal < (potVal)){
              leds[0] = ledCol;
              leds[1] = ledCol;
              leds[2] = ledCol;
              leds[3] = ledCol;
              leds[4] = ledCol;
              leds[5] = ledCol;
              leds[6] = ledCol;
              leds[7] = ledCol;
              FastLED.show();
              delay(delTim);
              leds[7] = CRGB::Black;
              FastLED.show();
              delay(delTim);
           }
        }
        else if (tempVal > potVal && relayOn == 0) {
          // reached goal - all white
          
          leds[0] = ledGoal;
          leds[1] = ledGoal;
          leds[2] = ledGoal;
          leds[3] = ledGoal;
          leds[4] = ledGoal;
          leds[5] = ledGoal;
          leds[6] = ledGoal;
          leds[7] = ledGoal;
          FastLED.show(); 
       
        //Piezo  
          //beeps 3 times once ever
          if (numBeep < 4){
            tone(piezoPin,500,100);
            delay(10);
            numBeep = (numBeep + 1);
          }
          else {
            noTone(piezoPin);
          }
        }
    
        else if (tempVal < potVal && relayOn == 0) {
          // standby mode
          redValue = 180;
          greenValue = 170;
          blueValue = 0;
          analogWrite(redLEDPin, redValue);
          analogWrite(greenLEDPin, greenValue);
          analogWrite(blueLEDPin, blueValue);
         }
   }
  else {
    redValue = 10;
    greenValue = 10;
    blueValue = 0;
    analogWrite(redLEDPin, redValue);
    analogWrite(greenLEDPin, greenValue);
    analogWrite(blueLEDPin, blueValue);

    leds[0] = CRGB::Black;
    leds[1] = CRGB::Black;
    leds[2] = CRGB::Black;
    leds[3] = CRGB::Black;
    leds[4] = CRGB::Black;
    leds[5] = CRGB::Black;
    leds[6] = CRGB::Black;
    leds[7] = CRGB::Black;
    FastLED.show();

  //  digitalWrite(yellowLEDPin, HIGH);

    display.clearDisplay();

    display.display();
    delay(100);

    relayOn = 0; 
    digitalWrite(relayLEDpin, 0);
    
  }
 
 //last bracket

 }
