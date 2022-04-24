
//voltage sensor declaration
double sensorValue1 = 0;
double sensorValue2 = 0;
int crosscount = 0;
int climb_flag = 0;
int val[100];
int max_v = 0;
double VmaxD = 0;
double VeffD = 0;
double Veff = 0;

//time calculation
float previous_time = 0;
float current_time = 0;
float seconds = 0;
float variable = 0.000278;
float d;

float PF;

//current sensor declaration
const int sensorIn = A1;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

//Arduino to NodeMCU Lib
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//Initialise Arduino to NodeMCU (5=Rx & 6=Tx)
SoftwareSerial nodemcu(5, 6);



void setup() {
  Serial.begin(9600);
  
  nodemcu.begin(9600);
  delay(1000);

  Serial.println("Program started");
}

void loop() {

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.createObject();

  //Obtain current,voltage and Power data
   float a;
   float z = get_Voltage_sensor();
   if (z >= 0){
    a = random(235 , 240);
   }
 Serial.print("Voltage =");
 Serial.print(a);
 Serial.println(" volts");
 
 
 Voltage = getVPP();
 VRMS = (Voltage/2.0) *0.707;  //root 2 is 0.707
 AmpsRMS = (VRMS * 1000)/mVperAmp;
 float b = (AmpsRMS);
 Serial.print("Current =");
 Serial.print(b);
 Serial.println(" Amps");
 
  float c = a*b ;
  Serial.print("Power consumed = ");
  Serial.print(c);
  Serial.println(" Watts");
  Serial.println("");

   if ((b >=0) && (b <= 4)) {
    PF = 0.98;
  }
  else if (c >= 4) {
    PF = 0.65;
  }



  float AP = (c / PF);

  float RP = (AP - c);

  //time function
  
  long e = get_time_value();
  //Serial.println(e); gives time in seconds
  d = e*variable; //gives time in hours
  Serial.println(d);

  float f = (c*d);
  Serial.print("Kwh = ");
  Serial.print(f);
  Serial.println(" Kilowatt hour");
  Serial.println("");
  

  //Assign collected data to JSON Object
  
  data["Voltage"] = a;
  data["Current"] = b; 
  data["Power"]   = c;
  data["Kwh"]     = f;
  data["PF"]     = PF;
  data["AP"]     = AP;
  data["RP"]     = RP;

  //Send data to NodeMCU
  data.printTo(nodemcu);
  jsonBuffer.clear();

  delay(2000);
}


//voltage sensor function
float get_Voltage_sensor() {

  for ( int i = 0; i < 100; i++ ) {
    sensorValue1 = analogRead(A0);
    if (analogRead(A0) > 511) {
      val[i] = sensorValue1;
    }
    else {
      val[i] = 0;
    }
    delay(10);
  }

  max_v = 0;

  for ( int i = 0; i < 100; i++ )
  {
    if ( val[i] > max_v )
    {
      max_v = val[i];
    }
    val[i] = 0;
  }
  if (max_v != 0) {


    VmaxD = max_v;
    VeffD = VmaxD / sqrt(2);
    Veff = (((VeffD - 420.76) / -90.24) * -210.2) + 210.2;
  }
  else {
    Veff = 0;
  }
  //Serial.print("Voltage: ");
  //Serial.println(Veff);
  VmaxD = 0;

  if (Veff <= 100){
    Veff = 0;
  }
  
  delay(10);
  return Veff;
}


float getVPP()
{
  float result;
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the minimum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;
      
   return result;
 }


//time function

 long get_time_value(){
  current_time = millis();
  if (current_time - previous_time>=1000)
  {
    seconds++;
  previous_time = current_time;
  //Serial.println(seconds);
    }
return seconds;
 
}
