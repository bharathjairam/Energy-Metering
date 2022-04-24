#include <FirebaseESP8266.h>
#include <ESP8266WiFi.h>
#include <OneWire.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <TimeLib.h>

//D6 = Rx & D5 = Tx
SoftwareSerial nodemcu(D6, D5);

#define WIFI_SSID "MAJOR PROJECT"
#define WIFI_PASSWORD "12345678"

#define FIREBASE_HOST "smart-energy-metering-5df43-default-rtdb.firebaseio.com"



#define FIREBASE_AUTH "PqD5fILyIO5TuGKJz7u3UQnIcaOia2fkRGpg7tXk"




FirebaseData fbdo;

FirebaseJson json;

int getResponse(FirebaseData &data);

int inbuilt_led = 2;

void printError(FirebaseData &data);

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 19800, 60000);

char Time[ ] = "TIME:00:00:00";
char Date[ ] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;

void setup(void) {

  // Initialize Serial port
  Serial.begin(9600);
  nodemcu.begin(9600);
  while (!Serial) continue;

  pinMode(inbuilt_led, OUTPUT);
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  timeClient.begin();


  //Set the size of WiFi rx/tx buffers in the case where we want to work with large data.
  fbdo.setBSSLBufferSize(1024, 1024);

  //Set the size of HTTP response buffers in the case where we want to work with large data.
  fbdo.setResponseSize(1024);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(fbdo, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(fbdo, "tiny");
}


void loop(void) {


  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& data = jsonBuffer.parseObject(nodemcu);

  if (data == JsonObject::invalid()) {
    //Serial.println("Invalid Json Object");
    jsonBuffer.clear();
    return;
  }

 

  Serial.println("JSON Object Recieved");
  Serial.print("Voltage:  ");
  float a = data["Voltage"];
  Serial.println(a);
  Serial.print("Current:  ");
  float b = data["Current"];
  Serial.println(b);
  Serial.print("Power:  ");
  float c = data["Power"];
  Serial.println(c);
  Serial.print("Kwh:  ");
  float f = data["Kwh"];
  Serial.println(f);
  Serial.print("PF:  ");
  float PF = data["PF"];
  Serial.println(PF);
  Serial.print("AP:  ");
  float AP = data["AP"];
  Serial.println(AP);
  Serial.print("RP:  ");
  float RP = data["RP"];
  Serial.println(RP);
  Serial.println("-----------------------------------------");

  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server

  second_ = second(unix_epoch);
  if (last_second != second_) {


    minute_ = minute(unix_epoch);
    hour_   = hour(unix_epoch);
    day_    = day(unix_epoch);
    month_  = month(unix_epoch);
    year_   = year(unix_epoch);



    Time[12] = second_ % 10 + 48;
    Time[11] = second_ / 10 + 48;
    Time[9]  = minute_ % 10 + 48;
    Time[8]  = minute_ / 10 + 48;
    Time[6]  = hour_   % 10 + 48;
    Time[5]  = hour_   / 10 + 48;



    Date[5]  = day_   / 10 + 48;
    Date[6]  = day_   % 10 + 48;
    Date[8]  = month_  / 10 + 48;
    Date[9]  = month_  % 10 + 48;
    Date[13] = (year_   / 10) % 10 + 48;
    Date[14] = year_   % 10 % 10 + 48;

    Serial.println(Time);
    Serial.println(Date);


    last_second = second_;




    delay(3000);
    Serial.print("Current date: ");
    Serial.println(Date);
    Serial.print("Current time: ");
    Serial.println(Time);
    json.clear();
    json.add("LATEST_CURRENT_VALUE", (b));
    json.add("LATEST_VOLTAGE_VALUE", (a));
    json.add("LATEST_POWER_VALUE", (c));
    json.add("LATEST_Kwh_VALUE", (f));
    json.add("LATEST_PF_VALUE", (PF));
    json.add("LATEST_AP_VALUE", (AP));
    json.add("LATEST_RP_VALUE", (RP));
    json.add("UPDATED_TIME", Time);
    json.add("UPDATED_DATE", Date);
    Firebase.updateNode(fbdo, "/LATEST_DATA", json);
    json.clear();
    json.add("LATEST_CURRENT_VALUE", (b));
    json.add("LATEST_VOLTAGE_VALUE", (a));
    json.add("LATEST_POWER_VALUE", (c));
    json.add("LATEST_Kwh_VALUE", (f));
    json.add("LATEST_PF_VALUE", (PF));
    json.add("LATEST_AP_VALUE", (AP));
    json.add("LATEST_RP_VALUE", (RP));
    Firebase.updateNode(fbdo, "HISTORICAL_DATA/" + String(Date) + "/" + String(Time), json);

  }
}


void printError(FirebaseData &data) {
  Serial.println("------------------------------------");
  Serial.println("FAILED");
  Serial.println("REASON: " + fbdo.errorReason());
  Serial.println("------------------------------------");
}

int getResponse(FirebaseData &data) {
  if (data.dataType() == "int")
    return data.intData();
  else
    return 100;
}
