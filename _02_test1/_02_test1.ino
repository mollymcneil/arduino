
const int buzzerPin=7; //Connect the Buzzer Grove module to Pin3, Digital 3
float WarningValue= 19.5;  //The minimum sate concentration of O2 in air  
void setup()
{
   Serial.begin(9600);                //Start the Serial connection
   pinMode(4,OUTPUT);
   pinMode(5,OUTPUT);
   pinMode(6,OUTPUT);
   pinMode(7,OUTPUT);
   
}
void loop()
{
//long unsigned a;
float sensorValue;
float sensorVoltage; 
float Value_O2;
sensorValue = analogRead(A0);
sensorVoltage =(sensorValue/1024)*5.0;
sensorVoltage = sensorVoltage/201*10000;
Value_O2 = sensorVoltage/7.43;
 
Serial.print("Concentration of O2 is ");
Serial.print(Value_O2,1);
Serial.println("%");
 
if(15<Value_O2 && Value_O2<=WarningValue)
 {
   digitalWrite(4,HIGH);
   digitalWrite(5,HIGH);
   digitalWrite(6,HIGH);
   digitalWrite(7,LOW);
 }
 else if(Value_O2>19.6){
   digitalWrite(4,LOW);
   digitalWrite(5,LOW);
   digitalWrite(6,LOW);
   digitalWrite(7,HIGH);
}
else if(Value_O2<=15){
   digitalWrite(4,LOW);
   digitalWrite(5,LOW);
   digitalWrite(6,HIGH);
   digitalWrite(7,LOW);
}
delay(1000);
}
