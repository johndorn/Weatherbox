/*
 *  Author:  Blake Kizer
 *
 *  This project uses an Arduino Uno and Arduino WiFi Shield
 *  to connect to w1.weather.gov and retrieve the 
 *  weather data for the location code. This was made as a 
 *  birthday gift for my Dad.
 *
 *  Happy birthday, Dad.
 *
 */

#include <WiFi.h>

// Define a max string length to work with for the string buffer.
#define MAX 50

// Setup vars
char element[MAX] = "";
char xmlValue[MAX] = "";
char temp[MAX] = "";
char xmlEnd[3] = {'<', '/', '\0'};
int waitTime = 60000 * 15; // 15 minutes to wait
int temperature;
char weather[MAX] = "";

// Setup Lights
byte redPin = 3;
byte greenPin = 5;
byte bluePin = 6;
int rgb[3] = {0, 0, 0};

int oldRed, oldBlue, oldGreen;

// Booleans used for parsing
boolean elemBool = false;
boolean valueBool = false;

// Setup internet variables
char ssid[] = "Your_SSID_Here"; //  Your network SSID (name) 
char pass[] = "Your_Network_Password";             // Your network password 
int keyIndex = 0;                       // Your network key Index number
int status = WL_IDLE_STATUS; 

// if you don't want to use DNS, you can use IP
//IPAddress server(140,90,113,200);     // IP for weather.gov 
char server[] = "w1.weather.gov";       // Address of weather.gov

// Initialize the WiFi Client to interface with device
WiFiClient client;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  
  Serial.println("Trying 100: ");
  setRGB(100);
  delay(5000);
  Serial.println("Trying 94: ");
  setRGB(94);
  delay(5000);
  Serial.println("Trying 85: ");
  setRGB(85);
  delay(5000);
  Serial.println("Trying 75: ");
  setRGB(75);
  delay(5000);
  Serial.println("Trying 65: ");
  setRGB(65);
  delay(5000);
  Serial.println("Trying 55: ");
  setRGB(55);
  delay(5000);
  Serial.println("Trying 45: ");
  setRGB(45);
  delay(5000);
  Serial.println("Trying 35: ");
  setRGB(35);
  delay(5000);
  Serial.println("Trying 25: ");
  setRGB(25);
  delay(5000);
  Serial.println("Trying 15: ");
  setRGB(15);

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not detected"); 
    // stop other code from running
    while(true);
  } 
  
  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) { 
    Serial.print("Connecting to network: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:    
    status = WiFi.begin(ssid, keyIndex, pass);
    
    // To use with WPA/WPA2 networks:
    // status = WiFi.begin(ssid, keyIndex, pass);
  
    // 10 second pause to let WiFi connect:
    delay(10000);
  } 
  Serial.print("Connected to network: ");
  Serial.println(ssid);
  
  getWeatherData();

  // set the light
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
}

void loop() {

  // Read web response
  while (client.available()) {
    parseData(); 
  }

  if (!client.connected()) {
    client.stop();

    // Wait 15 minutes for the next call
    delay(waitTime); 
    getWeatherData();
  }
}

void getWeatherData(){
  Serial.println("\nAttempting to connect to weather.gov...");
  if (client.connect(server, 80)) {
    // Make GET call for XML data
    Serial.println("Connected!");
    client.println("GET /xml/current_obs/KNAK.xml HTTP/1.1");
    client.println("Host:w1.weather.gov");
    client.println("Connection:close");
    client.println();
    delay(2000);
  } else {
      Serial.print("Cannot retrieve XML from weather.gov. Status: ");
      Serial.println(status);
  }
}

// Process each char from web
void parseData() {
   // Read a char
   char c = client.read();
  
   if (c == '<') {
      insert(c, temp);
      elemBool = true;
      valueBool = false;
   } else if (c == '>') {
      insert(c, temp);
      if (elemBool) {      
         strncpy(element, temp, strlen(temp)+1);
      }

      clean(temp);

      elemBool = false;
      valueBool = true;      
      
   } else if (c != 10) {
      if (elemBool) {
         // Add tag char to string
         insert(c, temp);

         // Check for </XML> end tag, ignore it
         if (elemBool && strcmp(temp, xmlEnd) == 0 ) {
            clean(temp);
            elemBool = false;
            valueBool = false;
         }
      }
      
      if (valueBool) {
         // Add data char to string
         insert(c, xmlValue);
      }
   }  
  
   // Process data if there's a Line Feed
   if (c == 10 ) {
      // Find specific tags and print data
      if (charCompare("<temp_f>")) {
        // capture the temperature as an int
        temperature = atoi(xmlValue);
        Serial.print("Temperature: ");
        Serial.println(temperature);
        setRGB(temperature);
      }
      if (charCompare("<weather>")) {
        strcpy(weather, xmlValue);
        Serial.print("Current Conditions: ");
        Serial.println(weather);
      }

      // Clear all strings
      clean(temp);
      clean(element);
      clean(xmlValue);

      // Clear Flags
      elemBool = false;
      valueBool = false;
   }
}

// Clean the string
void clean (char* cleaner) {
   int len = strlen(cleaner);
   int i;
   for (i = 0; i < len; i++) {
      cleaner[i] = '\0';
   }
}

// Insert a char to the string
void insert (char c, char* cstring) {
   char *truncated  = "TRUNCATED";

   // If the string exceeds the max size, truncate it.
   if (strlen(cstring) > MAX - 2) {
      if (elemBool) {
         clean(element);
         strcpy(element,truncated);
      }
      if (valueBool) {
         clean(xmlValue);
         strcpy(xmlValue,truncated);
      }

      // Clear the temp string and reset booleans
      clean(temp);
      elemBool = false;
      valueBool = false;

   } else {
      // Add the letter to the string
      cstring[strlen(cstring)] = c;
   }
}

// Return true or false if a match is found.
boolean charCompare (char* lookup) {
   return strcmp(element, lookup) == 0 ? true : false;
}

// Set the rgb value based on the temperature value.
void setRGB(int tempf) {
  if (tempf >= 95) {
    rgb[0] = 255 - 172;
    rgb[1] = 255 - 36;
    rgb[2] = 255 - 48;
  }
  if (tempf >= 90 && tempf <=94) { 
    rgb[0] = 255 - 207;
    rgb[1] = 255 - 46;
    rgb[2] = 255 - 49;
  }
  if (tempf >= 80 && tempf <=89) { 
    rgb[0] = 255 - 238;
    rgb[1] = 255 - 113;
    rgb[2] = 255 - 25;
  }
  if (tempf >= 70 && tempf <=79) { 
    rgb[0] = 255 - 255;
    rgb[1] = 255 - 200;
    rgb[2] = 255 - 8;
  }
  if (tempf >= 60 && tempf <=69) { 
    rgb[0] = 255 - 170;
    rgb[1] = 255 - 198;
    rgb[2] = 255 - 27;
  }
  if (tempf >= 50 && tempf <=59) { 
    rgb[0] = 255 - 23;
    rgb[1] = 255 - 106;
    rgb[2] = 255 - 43;
  }
  if (tempf >= 40 && tempf <=49) { 
    rgb[0] = 255 - 3;
    rgb[1] = 255 - 130;
    rgb[2] = 255 - 122;
  }
  if (tempf >= 30 && tempf <=39) { 
    rgb[0] = 255 - 0;
    rgb[1] = 255 - 147;
    rgb[2] = 255 - 159;
  }
  if (tempf >= 20 && tempf <=29) { 
    rgb[0] = 255 - 44;
    rgb[1] = 255 - 77;
    rgb[2] = 255 - 143;
  }
  if (tempf <=19) { 
    rgb[0] = 255 - 126;
    rgb[1] = 255 - 188;
    rgb[2] = 255 - 209;
  }
  analogWrite(redPin, rgb[0]);
  analogWrite(greenPin, rgb[1]); 
  analogWrite(bluePin, rgb[2]); 
}