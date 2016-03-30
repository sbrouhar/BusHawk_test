#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>

//**********************************************************//
//   Senior Design Group:     Bushawk                       //
//                                                          //
//   Group Members:      Seth Brouhard                      // 
//                       Jake Gould                         //
//                       Jeff Kraus                         //
//                       Ryan Land                          //
//                                                          //
//   Written by:        Seth Brouhard                       //
//                      Jeff Kraus                          //
//                                                          //
//**********************************************************//


//Define the Transit/Reciever for the FONA 808

#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4

//Create the Serial Connection for the FONA 808

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

//Char pointer to the database website
char* host = "bushawk.parseapp.com/BusPost";

//API Key used to verify the information being sent
String API_KEY = "XfsZ6Sz28z4ETKFWW23pdoey2QXL7h5oKctJTr3L";
char* app_ID = "\"NPeWLuPl79UnZjQQKAWwsIBfOY6Y3MV2hJqVk8ke\"";

///char array for Bus ID or bus number///

char* bus_ID = "1";
char* txt = "\"ArduinoTest\"";

uint16_t status_;
uint16_t length;

//Variable to show when the gps has
//obtained a fix location
bool GPS_Fix_Flag;

#define inf  3.4028235E+38 



//Variables used to get GPS location
float curr_lat,curr_lon, speed_kph, heading, speed_mph, altitude;

//Create char variables 
char longitude[14];
char latitude[14];
char mph[7];
char alt[7];
char head_N[10];

uint8_t type;


int busID_1 = 0;
int busID_2 = 1;
int busID_3 = 2;
int busID_4 = 3;

int busID_B;

//Pin location for the LED's 
int onLight = 6;
int gpsFixLight = 7;



void setup(){

  //Initialize variables
  GPS_Fix_Flag = false;
  
  curr_lon = 0.0f;
  curr_lat = 0.0f;
  speed_mph = -1.0f;
  altitude = -1.0f;
/*
  pinMode(busID_1,INPUT);
  pinMode(busID_2,INPUT);
  pinMode(busID_3,INPUT);
  pinMode(busID_4,INPUT);
*/

  //Set up pins for LED to show if the 
  //GPS is fixed and if the unit is on
  pinMode(onLight,OUTPUT);
  pinMode(gpsFixLight,OUTPUT);
 

  Serial.begin(115200);

  fonaSerial->begin(4800);
 
  while(!fona.begin(*fonaSerial)) {
  
    Serial.println("Connecting FONA 808");
    
   }
   
 Serial.println("Turning GPS on");
    turnGPS_on();

 
 Serial.println("Turning GPRS on");
    turnGPRS_on();

 
}
void loop(){
  //getBusID();
  
  clear_Serial();

  //Turns on the red LED to signal
  //that the unit is on
  digitalWrite(onLight, HIGH);

  
  if(GPS_Fix_Flag){
    digitalWrite(gpsFixLight, HIGH);
  }
  
  //Get GPS locations and set the values 
  getGPS();

  //Send the coordinates to the Parse database
  sendInfo();
  
}

void getGPS(){

  Serial.println("Get GPS");
 
  GPS_Fix_Flag = fona.getGPS(&curr_lat, &curr_lon, &speed_kph, &heading, &altitude);

  speed_mph = speed_kph * 0.621371192;
  
  
}
void turnGPRS_on(){

  //Enable GPRS network and 
  //Continues to enable until the 
  //GPRS is enabled
  while(!fona.enableGPRS(true)){
    
     Serial.println("Failed enable");

  }
}
void turnGPS_on(){

  //Enable GPS and continue
  //to enable GPS until it
  //is enabled
  
  if (!fona.enableGPS(true)){
      //    Serial.println("Failed to turn on");
  }
 
}

void sendInfo(){


  dtostrf((double)curr_lon,13,8,longitude);
  dtostrf((double)curr_lat,13,8,latitude);
  dtostrf((double)speed_mph,6,2,mph);
  dtostrf((double)altitude,6,0,alt);
  dtostrf((double)heading,9,4,head_N);

  
  char json[256];
  strcpy (json,"{\"appID\":");
  strcat (json, app_ID);
  strcat (json,", \"busID\":");
  strcat (json, bus_ID);


  //IF the GPS Fix Flag is true then the coordinates will be 
  //sent else the coord are not sent. Ensures that not coordinate
  //will be sent with the variables initial value
  if(GPS_Fix_Flag){
  strcat (json,", \"longitude\":");
  strcat (json, longitude);
  strcat (json,", \"latitude\":");
  strcat (json, latitude);
  strcat (json, ", \"altitude\":");
  strcat (json, alt);
  strcat (json, ", \"speed\":");
  strcat (json, mph);
  strcat (json, ", \"heading\":");
  strcat (json, head_N);
  }
  strcat (json, ", \"text\":");
  strcat (json, txt);
  strcat (json, "}");

  char url[80];
  char data[256];
  
  sprintf(url, host);
  sprintf(data, json);
  
  

  //Post data and print out what the data
  //looks like when its sent
  
 Serial.println("Posting Data");
 Serial.println(data);
  
  if(!fona.HTTP_POST_start(host, F("application/json"), (uint8_t *) data, strlen(data), &status_, (uint16_t *)&length)){
 
  //  Serial.println(data);
  //  Serial.println("Failed to post");
  
  }
}

void clear_Serial() {
  
  //Get rid of data in network
  //
  while (Serial.available()){
    
      Serial.read();
    
  }

}
/*
void getBusID(){


  if(digitalRead(busID_1) == HIGH){

    busID_B += 1;
    
  }
  if(digitalRead(busID_2) == HIGH){

    busID_B += 2;
    
  }
  if(digitalRead(busID_3) == HIGH){

    busID_B += 4;
    
  }
  if(digitalRead(busID_4) == HIGH){

    busID_B += 8;
    
  }


}
*/
