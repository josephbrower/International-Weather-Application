#include <WiFiNINA.h>
#include <Arduino_JSON.h>
 
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;
 
char ssid[] = "######"; //  your network NAME between ""
char pass[] = "######";//  your network PASSWOR between ""
 
String apiKey = "d159ec749142225341bd2b07448ba4b8"; //paste weather map API KEY between ""
 
//the cities you want the weather for
String location = "London, UK";
String location2 = "San Francisco, US";
 
bool changeLocation = false;

WiFiClient client;

String cityName;
String weatherDescription;
 
double temperature;
double feelsLike;
double lon;
double lat;

int status = WL_IDLE_STATUS;
char server[] = "api.openweathermap.org";


void setup(){
  Serial.begin(9600);
  
  //Initialize the MKR IoT carrier
  CARRIER_CASE = false;
  carrier.begin();
  
  carrier.display.setTextSize(3);
    
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    
    carrier.display.setCursor(20, 70);
    carrier.display.print("Waiting For");
    carrier.display.setCursor(5, 110);
    carrier.display.print("Connection...");
    
    // wait 1 seconds for connection:
    delay(1000);
  }
  
  carrier.display.fillScreen(ST77XX_BLACK);
  carrier.display.setTextSize(2);
  carrier.display.fillRect(0 , 255, 255);
  carrier.display.setCursor(45, 40);
  carrier.display.println("Connected to");
  carrier.display.setCursor(25, 80);
    carrier.display.setTextSize(3);
  carrier.display.println("** WiFi **");
 
  Serial.println("Connected to wifi");
  delay(1000);
 
  //Retrieve data from the API
  updateWeather();
  
  carrier.display.setTextSize(2);
  carrier.display.setCursor(20, 130);
  carrier.display.println("press any button");
  carrier.display.setCursor(60, 160);
  carrier.display.println("to start");
}

void loop(){
  carrier.Buttons.update();
 
  if (carrier.Buttons.onTouchDown(TOUCH0)) {
    carrier.display.fillScreen(ST7735_BLACK);
    carrier.display.setCursor(20, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.print("Updating data");
    delay(500);
    carrier.display.print(".");
    delay(500);
    carrier.display.print(".");
    delay(500);
    carrier.display.print(".");
    delay(500);
    carrier.display.print(".");
 
    updateWeather();
 
    carrier.display.fillScreen(ST7735_BLACK);
    carrier.display.setCursor(35, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.println("Update Complete");
    carrier.display.setCursor(20, 110);
    carrier.display.println("Weather data from:");
    carrier.display.setCursor(30, 160);
    carrier.display.setTextSize(2);
    
    if (changeLocation == true) {
      carrier.display.println(location);
    }
    else if (changeLocation == false) {
      carrier.display.println(location2);
    }
  }
  
  if (carrier.Buttons.onTouchDown(TOUCH1)) {
    carrier.display.fillScreen(ST7735_BLACK);
    carrier.display.setCursor(30, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.println(cityName);
    carrier.display.setCursor(20, 90);
    carrier.display.println("Temperature: ");
    carrier.display.setCursor(70, 130);
    carrier.display.println(temperature);
    carrier.display.setCursor(60, 160);
    carrier.display.println("Celsius");
    delay(500);
  }
  if (carrier.Buttons.onTouchDown(TOUCH2)) {
    carrier.display.fillScreen(ST7735_BLACK);
    carrier.display.setCursor(30, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.println(cityName);
    carrier.display.setCursor(10, 90);
    carrier.display.println(" Feels like: ");
    carrier.display.setCursor(70, 130);
    carrier.display.println(feelsLike);
    carrier.display.setCursor(60, 160);
    carrier.display.println("Celsius");
    delay(500);
  }
  if (carrier.Buttons.onTouchDown(TOUCH3)) {
    carrier.display.fillScreen(ST7735_BLACK);
    carrier.display.setCursor(30, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.println(cityName);
    carrier.display.setCursor(50, 90);
    carrier.display.print("Weather: ");
    carrier.display.setCursor(5, 130);
    carrier.display.setTextSize(1);
    carrier.display.println(weatherDescription);
    delay(500);
  }
  if (carrier.Buttons.onTouchDown(TOUCH4)) {
    carrier.display.fillScreen(ST77XX_BLACK);
    carrier.display.setCursor(30, 60);
    carrier.display.setTextColor(ST77XX_WHITE);
    carrier.display.setTextSize(2);
    carrier.display.println(cityName);
    carrier.display.setCursor(40, 90);
 
    carrier.display.println("Longitude: ");
    carrier.display.println(lon);
 
    carrier.display.setCursor(40, 150);
    carrier.display.println("Latitude: ");
    carrier.display.println(lat);
    delay(500);
  }
}
void updateWeather(){
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("GET /data/2.5/weather?");
 
    //used to switch between locations
    changeLocation = !changeLocation;
 
    if (changeLocation == true) {
      client.print("q=" + location);
    }
    else {
      client.print("q=" + location2);
    }
 
    client.print("&appid=" + apiKey);
    client.print("&cnt=3");
    client.println("&units=metric");
    client.println("Host: api.openweathermap.org");
    client.println("Connection: close");
    client.println();
  } else {
    Serial.println("unable to connect");
  }
  String line = "";
  if (client.connected()) {
    line = client.readStringUntil('\n');
    Serial.println(line);
    JSONVar myObject = JSON.parse(line);
    cityName = JSON.stringify(myObject["name"]);
    weatherDescription = JSON.stringify(myObject["weather"][0]["description"]);
 
    temperature = myObject["main"]["temp"];
    feelsLike = myObject["main"]["feels_like"];
    lon = myObject["coord"]["lon"];
    lat = myObject["coord"]["lat"];

    carrier.Buzzer.sound(300);
    delay(300);
    carrier.Buzzer.noSound();
    delay(300);
    carrier.Buzzer.sound(300);
    delay(300);
    carrier.Buzzer.noSound();
  }
}

