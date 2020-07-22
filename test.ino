//Connect to Wifi Code 1 Start
// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "BadWifi";
const char* password = "Sanmina02";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
//String output5State = "off";
//String output4State = "off";
String data = "";
long duration_vertical_sonar;                                                                  
long distance_vertical_sonar;                                                                  
long duration_horizontal_sonar;                                                                      
long distance_horizontal_sonar; 

// Assign output variables to GPIO pins
const int lower_rotation_1 = 5;
const int lower_rotation_2 = 4;
const int higher_rotation_1 = 12;
const int higher_rotation_2 = 14;
const int servo = 10;
const int trigPin_vertical = 13;                                                                        
const int echoPin_vertical = 15;                                                                     
const int trigPin_horizontal = 1;                                                                       
const int echoPin_horizontal = 3;


// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;
void vertical_sonar()
{
  delay(50);
  digitalWrite(trigPin_vertical, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin_vertical, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_vertical, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration_vertical_sonar = pulseIn(echoPin_vertical, HIGH);

  // Calculating the distance
  distance_vertical_sonar = duration_vertical_sonar*0.034/2;
}

void horizontal_sonar()
{
  delay(50);
  digitalWrite(trigPin_horizontal, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin_horizontal, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin_horizontal, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration_horizontal_sonar = pulseIn(echoPin_horizontal, HIGH);

  // Calculating the distance
  distance_horizontal_sonar = duration_horizontal_sonar*0.034/2;
}
void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(servo, OUTPUT);
  pinMode(lower_rotation_1, OUTPUT);
  pinMode(lower_rotation_2, OUTPUT);
  pinMode(higher_rotation_1, OUTPUT);
  pinMode(higher_rotation_2, OUTPUT);
  pinMode(trigPin_vertical, OUTPUT); // Sets the trigPin as an Output                                   
  pinMode(echoPin_vertical, INPUT); // Sets the echoPin as an Input                                     
  pinMode(trigPin_horizontal, OUTPUT); // Sets the trigPin as an Output                                       
  pinMode(echoPin_horizontal, INPUT); // Sets the echoPin as an Input 
  // Set outputs to LOW
  digitalWrite(lower_rotation_1, LOW);
  digitalWrite(lower_rotation_2, LOW);
  digitalWrite(higher_rotation_1, LOW);
  digitalWrite(higher_rotation_2, LOW);
  digitalWrite(servo, HIGH);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              //output5State = "on";
              digitalWrite(lower_rotation_1, HIGH);
              delay(40);
              digitalWrite(lower_rotation_1, LOW);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              //output5State = "off";
              digitalWrite(lower_rotation_2, HIGH);
              delay(100);
              digitalWrite(lower_rotation_2, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              //output4State = "on";
              digitalWrite(higher_rotation_1, HIGH);
              delay(100);
              digitalWrite(higher_rotation_1, LOW);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              //output4State = "off";
              digitalWrite(higher_rotation_2, HIGH);
              delay(100);
              digitalWrite(higher_rotation_2, LOW);
            }
            if (header.indexOf("GET /6/off") >= 0) {
              Serial.println("GPIO 6 off");
              digitalWrite(servo, LOW);
            } else if (header.indexOf("GET /6/on") >= 0) {
              Serial.println("GPIO 6 on");
              digitalWrite(servo, HIGH);
            } 
            vertical_sonar();
            horizontal_sonar();
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #77878A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #195B6A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            //client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            //if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">DOWN</button></a></p>");
            //} else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">UP</button></a></p>");
            //} 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            //client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            //if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">LEFT</button></a></p>");
            //} else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">RIGHT</button></a></p>");
              client.println("<p><a href=\"/6/on\"><button class=\"button\">OFF</button></a></p>");
            //} else {
              client.println("<p><a href=\"/6/off\"><button class=\"button button2\">ON</button></a></p>");
            //}
              client.println("<p>Horizontal Reading: " + String(distance_horizontal_sonar) + "</p>");
              client.println("<p>Vertical Reading: " + String(distance_vertical_sonar) + "</p>");
            client.println("</body></html>");
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
//Connect to Wifi Code 1 End


//Connect to Wifi Code 2 Start
/*
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

// Put your SSID & Password 
//const char* ssid = "NodeMCU";  // Enter SSID here
//const char* password = "12345678";  //Enter Password here
const char *ssid = "BadWifi"; //Enter your WIFI ssid
const char *password = "Sanmina02";
// Put IP Address details 
//IPAddress local_ip(192,168,1,1);
//IPAddress gateway(192,168,1,1);
//IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

uint8_t LED1pin = D7;
bool LED1status = LOW;

uint8_t LED2pin = D6;
bool LED2status = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);

  //WiFi.softAP(ssid, password);
  //WiFi.softAPConfig(local_ip, gateway, subnet);
  //delay(100);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}
  
  if(LED2status)
  {digitalWrite(LED2pin, HIGH);}
  else
  {digitalWrite(LED2pin, LOW);}
}

void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("GPIO6 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true)); 
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat,uint8_t led2stat){
  
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>\n";
  ptr +="body {text-align: center; justify-content: center; font-family: monospace, Helvetica; background-color: burlywood;}\n";
  ptr +=".container {display: flex;justify-content: center;}\n";
  ptr +=".container .off {background-color:khaki; color: red; width: 30%; line-height: 500%; margin: 7%; border-radius: 15%; font-size: 200%;}\n";
  ptr +=".container .off:active {background-color:khaki; color: red; width: 30%; line-height: 500%; margin: 7%; border-radius: 15%; font-size: 200%;}\n";
  ptr +=".container .on {background-color:darkcyan; color: white; width: 30%; line-height: 500%; margin: 7%; border-radius: 15%; font-size: 200%;}\n";
  ptr +=".container .on:active {background-color:darkcyan; color: white; width: 30%; line-height: 500%; margin: 7%; border-radius: 15%; font-size: 200%;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Remote Control Robot</h1>\n";
  ptr +="<h3>AP Mode</h3>\n";
  ptr +="<div class=\"container\">\n";
   if(led1stat)
  {ptr +="<a class=\"on\" href=\"/led1off\">ON</a>\n";}
  else
  {ptr +="<a class=\"off\" href=\"/led1on\">OFF</a>\n";}

  if(led2stat)
  {ptr +="<a class=\"on\" href=\"/led2off\">ON</a>\n";}
  else
  {ptr +="<a class=\"off\" href=\"/led2on\">OFF</a>\n";}
  ptr +="</div>\n";
  ptr +="<script>alert('hello i am happening'); window.location=\"/led1on\";</script>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}*/

//Connect to Wifi Code 2 End