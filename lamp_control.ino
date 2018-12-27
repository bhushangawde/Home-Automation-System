// Required libraries
#include <ESP8266WiFi.h>
#include "DHT.h"

// Pin
#define DHTPIN 2

// Use DHT11 sensor
#define DHTTYPE DHT11

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE, 15);

// WiFi parameters
const char* ssid = "CCF2";
const char* password = "ABCD1234";

// Create an instance of the server
WiFiServer server(80);

// Pin
int output_pin = 0;

void setup() {

  // Start Serial
  Serial.begin(115200);
  delay(10);
  
  // Prepare GPIO5
  pinMode(output_pin, OUTPUT);
  digitalWrite(output_pin, 0);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.println(WiFi.localIP());
}

void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius
  float t = dht.readTemperature();

   // Display data
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.println(" *C ");
  delay(2000);
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
 /* while(!client.available()){
    Serial.println("hello");
    delay(1);
  }
  */
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  if (req.indexOf("/on") != -1){
    digitalWrite(output_pin, 1);
  }
  else if (req.indexOf("/off") != -1) {
    digitalWrite(output_pin, 0);
  }

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<head>";
  s += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  s += "<script src=\"https://code.jquery.com/jquery-2.1.3.min.js\"></script>";
  s += "<script src=\"https://www.gstatic.com/firebasejs/3.6.2/firebase.js\"></script>";
  s += "<script>  var config = {apiKey: \"AIzaSyCLN6CfPNEzpGHKy2MRmNgxLvhs-0-Phto\",authDomain:\"iotindustrialautomation.firebaseapp.com\",";
  s += "databaseURL: \"https://iotindustrialautomation.firebaseio.com\", storageBucket: \"iotindustrialautomation.appspot.com\",";
  s += "messagingSenderId: \"206640203823\" }; firebase.initializeApp(config); </script>";

  
  s += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.4/css/bootstrap.min.css\">";
  s += "<style>body{font-size: 24px;} .voffset {margin-top: 30px;}</style>"; //added from 1
  s += "</head>";
  s += "<div class=\"container\">";
  s += "<h1>WiFi Weather Monitoring</h1>";
//Added
  s += "<div class=\"row voffset\">";
  s += "<div class=\"col-md-3\">Temperature: </div><div class=\"col-md-3\">" + String(t) + "</div>";
  s += "<div class=\"col-md-3\">Humidity: </div><div class=\"col-md-3\">" + String(h) + "</div>";
  s += "</div>";
  s += "</div>";
//Added
  s += "<h1>LED Control</h1>";
  s += "<div class=\"row\">";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-primary\" type=\"button\" value=\"Off\" onclick=\"on()\"></div>";
  s += "<div class=\"col-md-2\"><input class=\"btn btn-block btn-lg btn-danger\" type=\"button\" value=\"On\" onclick=\"off()\"></div>";
  s += "</div></div>";
  s += "<script>function on() {$.get(\"/on\");}</script>";
  s += "<script>function off() {$.get(\"/off\");}</script>";
  
//Adding data in Real Time
s += "<script>";
s += "var val = firebase.database().ref(\"Data\")";
s += "val.update({ output: {Led_State : " + String(output_pin) + "}";
s += "Readings : {Temperature : " + String(t) + " , Humidity: " + String(h) + "}" ;
s += "});";
s += "</script>";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disconnected");

  // The client will actually be disconnected
  // when the function returns and 'client' object is detroyed
}
