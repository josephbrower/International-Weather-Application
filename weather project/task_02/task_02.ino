#include <WiFiNINA.h>
#include <Arduino_JSON.h>
 
#include <Arduino_MKRIoTCarrier.h>
MKRIoTCarrier carrier;
 
char ssid[] = "home5292"; //  your network NAME between ""
char pass[] = "BeansSleeps3";//  your network PASSWOR between ""
 
String apiKey = "d159ec749142225341bd2b07448ba4b8"; //paste weather map API KEY between ""
 
//the cities you want the weather for
String locations[9] = {"Kotzebue, US", "Anchorage, US", "Juneau, US", "Vancouver, CA",
                        "Los Angeles, US", "Iztapalapa, MX", "Santago de Cali, CO",
                        "San Miguel de Tucuman, AR", "Comodoro Rivadavia, AR"};
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
  carrier.display.setCursor(45, 40);
  carrier.display.println("Connected to");
  carrier.display.setCursor(25, 80);
    carrier.display.setTextSize(3);
  carrier.display.println("** WiFi **");
 
  Serial.println("Connected to wifi");
  delay(1000);
 
  //Retrieve data from the API
  //updateWeather();
  
  carrier.display.setTextSize(2);
  carrier.display.setCursor(20, 130);
  carrier.display.println("press any button");
  carrier.display.setCursor(60, 160);
  carrier.display.println("to start");
}

void loop(){
  carrier.Buttons.update();
 
  if (carrier.Buttons.onTouchDown(TOUCH0) || carrier.Buttons.onTouchDown(TOUCH1) ||
      carrier.Buttons.onTouchDown(TOUCH2) || carrier.Buttons.onTouchDown(TOUCH3) || 
      carrier.Buttons.onTouchDown(TOUCH4)) {
        //updateWeather(locations[0]);
        //double colorRed = map(temperature, 0, 100, 0, 255);
        //double colorBlue = map(temperature, 0, 100, 255, 0);
      carrier.display.fillScreen(ST77XX_BLACK);
      carrier.display.setTextSize(2);
      for(int i = 0; i < 9; i++){
        updateWeather(locations[i]);
        //map the values from temp to color
        //full red are values 29 and up
        double colorRed = map(temperature, 5, 29, 0, 255);
        if(colorRed > 255){
          colorRed = 255;
        }else if(colorRed < 0){
          colorRed = 0;
        }
        //full blue values are 5 and lower
        double colorBlue = map(temperature, 5, 29, 255, 0);
        if(colorBlue > 255){
          colorBlue = 255;
        }else if(colorBlue < 0){
          colorBlue = 0;
        }
        //write to screen using that color
        carrier.display.setTextColor(RGBToUint16((uint16_t)colorRed, 0, (uint16_t)colorBlue));
        carrier.display.setCursor(50, 30 + (i*20));
        carrier.display.print(temperature);
        carrier.display.setCursor(130, 30 + (i*20));
        carrier.display.print(lat);
      }
      carrier.display.setTextColor(RGBToUint16(255, 255, 255));
      carrier.display.setCursor(10, 130);
      carrier.display.print("Tmp");
      carrier.display.setCursor(195, 130);
      carrier.display.print("Lat");
      delay(60000);
  }
}
void updateWeather(String location){
  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.print("GET /data/2.5/weather?");

    //using location
    client.print("q=" + location);

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
  }
}

uint16_t RGBToUint16(uint8_t red, uint8_t green, uint8_t blue) {
    // Convert 8-bit RGB components to 5-6-5 bits format
    uint16_t red5 = (red >> 3) & 0x1F;       // Extract 5 bits for red (31 max value)
    uint16_t green6 = (green >> 2) & 0x3F;   // Extract 6 bits for green (63 max value)
    uint16_t blue5 = (blue >> 3) & 0x1F;     // Extract 5 bits for blue (31 max value)

    // Combine the components into a 16-bit value
    uint16_t result = (red5 << 11) | (green6 << 5) | blue5;

    return result;
}
