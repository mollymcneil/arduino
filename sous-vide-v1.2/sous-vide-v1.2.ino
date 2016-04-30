// include the library code:
#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

#define ONE_WIRE_BUS 9 // temp: Data wire is plugged into pin 9 on the Arduino

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// servo setup
Servo myServo;

// LCD initialize the library with the numbers of the interface pins
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

// select input pin for Potentiometer
int potPin = 2;
float potVal = 0;
float tempVal;


//relay
int relayLEDpin = 12;
int relayPin = 11;
unsigned long lastToggledRelay;
int relayOn = 0;

unsigned long lastStirred;
int spoonPosition = 0;

void setup() {
  
  // LCD section
  
  lcd.begin(16, 2);

   // Temperature probe section

  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  sensors.begin(); 
  
  // relay section

  pinMode(relayLEDpin, OUTPUT);
  pinMode(relayPin, OUTPUT);

  lastToggledRelay = millis();

  //motor section 
  
  lastStirred = millis();

  myServo.attach(10);
  

}

void loop() {

  // Temperature probe section
   
  sensors.requestTemperatures(); 
  tempVal = sensors.getTempCByIndex(0);
  
   // LCD section

  lcd.setCursor(0,0);
  lcd.print("Temp is: ");
  lcd.print(tempVal);

  // POT section


  potVal = (int)(analogRead(potPin) / 2.048) / 10.0 + 50.0;

  lcd.setCursor(0,1);
  lcd.print("Set Temp: ");

  if (potVal < 100) { 
    lcd.print(" ");
  }
  if (potVal < 10) {
    lcd.print(" ");
  }
  
  lcd.print(potVal,2);

  //relay section

  if (millis() > (lastToggledRelay + 15000)) {

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

  //motor section

  if (millis() > (lastStirred + 2000)) {
    if (spoonPosition == 1) {
      int i = 0;
      for (i = 0; i < 28; i++) {
        myServo.write(90 - i);
        delay(8);
      }

      spoonPosition = 0;
      
    } else {
      int i = 0;
      for (i = 0; i < 28; i++) {
        myServo.write(90 + i);
        delay(8);
      }

      spoonPosition = 1;
    }
    lastStirred = millis();
    
  }
  
}



