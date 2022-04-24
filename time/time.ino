float previous_time = 0;
float current_time = 0;
float seconds = 0;
float variable = 0.000278;
float d;


void setup() {
 Serial.begin(9600);

}


void loop(){

  long e = get_time_value();
  //Serial.println(e);

  d = e*variable;
  Serial.println(d);
  }

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
