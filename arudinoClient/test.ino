/*
//Connect to Wifi Code 1 Start
// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

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
}*/
//Connect to Wifi Code 1 End


//Connect to Wifi Code 2 Start

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>


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
    server.on("launchActions", launchActions);

    server.onNotFound(handle_NotFound);

    server.begin();
    Serial.println("HTTP server started");
}
void loop() {
    server.handleClient();
    /*if (LED1status)
    {
        digitalWrite(LED1pin, HIGH);
    }
    else
    {
        digitalWrite(LED1pin, LOW);
    }

    if (LED2status)
    {
        digitalWrite(LED2pin, HIGH);
    }
    else
    {
        digitalWrite(LED2pin, LOW);
    }*/
}

void launchActions(){}

void handle_OnConnect() {
    server.send(200, "text/html", SendHTML());
}

void handle_led1on() {
    LED1status = HIGH;
    Serial.println("GPIO7 Status: ON");
    server.send(200, "text/html", SendHTML(true, LED2status));
}

void handle_led1off() {
    LED1status = LOW;
    Serial.println("GPIO7 Status: OFF");
    server.send(200, "text/html", SendHTML(false, LED2status));
}

void handle_led2on() {
    LED2status = HIGH;
    Serial.println("GPIO6 Status: ON");
    server.send(200, "text/html", SendHTML(LED1status, true));
}

void handle_led2off() {
    LED2status = LOW;
    Serial.println("GPIO6 Status: OFF");
    server.send(200, "text/html", SendHTML(LED1status, false));
}

void handle_NotFound() {
    server.send(404, "text/plain", "Not found");
}

char* SendHTML() {

    const char* condensedClientCode = R"(<!DOCTYPE html>
      <html>

      <head>
          <meta charset="UTF-8">
          <meta name="description" content="client side applet">
          <meta name="keywords" content="Canvas drawing client-side">
          <meta name="author" content="Matan Shtepe;">
          <meta name="viewport" content="width=device-width, initial-scale=1.0">
          <link rel="stylesheet" href="style.css">

          <style>
              * {
          margin: 0;
          padding: 0;
          box-sizing: border-box;
      }

      body {
          font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;
          color: #6C464F;
          background-color: #D2EFDC;
      }

      .page-wrapper {
          width: 75%;
          margin: 0 auto;
          padding: 3vh 0;

          /*display: flex;
          flex-direction: column;
          justify-content: center;
          align-items: flex-start;*/
      }


      .main-title {
          text-align: center;
      }

      .canvas-display {
          background-color: white;
          display: block;
          margin: 5vh auto 5vh;

          /*border: 10px solid #b7e2ec;*/
          border-radius: 7px;
      }

      .description-p {
          font-weight: 900;
          font-size: 1.2vw;
          text-align: center;
      }

      .form-wrapper {
          max-width: 81.5%;
          margin: 3vh auto;
          display: block;
          background-color: #b7e2ec;
          padding: 1.5vw;
          border-radius: 5px;


      }

      .action-form {
          display: flex;
          flex-direction: row;

          width: 100%;
          color: #9E768F;
          font-weight: 900;
          font-size: 1.7vw;

      }

      .form-cm-input {
          font-weight: 900;
          padding: .5vw;
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;

          color: #9E768F;
          font-size: 1.7vw;
          margin-right: 1vw;

      }

      .cm-span {
          padding-top: 1vh;
          margin-right: 2vw;
      }

      .radio-btts-container {
          padding: 1vh;
          margin-right: 2.8vw;
      }

      .form-select-direc {
          padding: .5vh.5vw 1vh;
          font-size: 1.7vw;
          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
          font-weight: 900;
          color: #9E768F;
          margin-right: 2vw;
      }

      input[type="radio"]:checked+label {
          color: #836076;
      }

      .form-select-direc option {
          font-weight: 900;
      }

      .form-add-action-butt {
          font-weight: 900;
          color: #9E768F;
          font-size: 1.4vw;
          padding: 0 1vw;
          border: none;
          cursor: pointer;


      }

      .form-add-action-butt:hover {
          color: #157A6E;
          background-color: #ED7D3A;
      }

      .run-actions-butt {
          display: block;
          font-weight: 900;
          color: #9E768F;
          font-size: 2vw;
          cursor: pointer;
          border: palevioletred solid 5px;
          display: block;

          padding: 0 1vw;
          margin: 0 auto;
      }

      .run-actions-butt:hover {
          font-weight: 900;
          color: #39b2bb;
          font-size: 2vw;
          cursor: pointer;
          border: rgb(112, 216, 129) solid 5px;
          padding: 0 1vw;

      }

      .action-display-container {
          width: 65%;
          margin: 3vh auto;
          min-height: 10vh;
          overflow: auto;

          background-color: #cab6b6;
          border-radius: 8px;

          display: flex;

      }

      .action-box {
          max-width: 7.5em;
          min-width: 7.5em;
          height: 13vh;
          padding: .3vw;
          margin: 10px;
          border-radius: 3px;

          font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
          font-weight: 900;

          background-color:
              /*#4E4187*/
              #b7e2ec;
          color:
              /*#FFBF00*/
              #6C464F;

          box-sizing: content-box;


      }

      .action-box:last-child {
          border-right: 15px solid #cab6b6;

      }

      .action-num {
          font-size: .8vw;
      }

      .delete-action-x {
          font-size: 1.3vw;
          color: #921c14;
          float: right;
          padding-right: .3vw;
      }

      .delete-action-x:hover {
          color: #E3170A;
          cursor: pointer;
          font-size: ;
      }

      .action-desc {
          padding: .6vh 0 1.2vh .6vw;

      }
          </style>
      </head>

      <body>
          <div class="page-wrapper">
              <h1 class="main-title">Dang & Matan's Canvas Applet</h1>

              <canvas class="canvas-display"></canvas>
              <p class="description-p">Select the attributes for the action you want to create and add to the sequence!</p>
              <div class="form-wrapper">
                  <form class="action-form">
                      <input class="form-cm-input" type="number" placeholder="3.14" value="1" min=".05" step=".05" max="8">
                      <span class="cm-span">CM</span>
                      <div class="radio-btts-container">
                          <input class="radio-bttn" checked="checked" type="radio" id="draw" name="draw-setting" value="Draw">
                          <label for="draw">Draw</label>
                          <input class="radio-bttn" type="radio" id="no-draw" name="draw-setting" value="No-Draw">
                          <label for="no-draw">No-Draw</label>
                      </div>

                      <select class="form-select-direc">
                          <option value="up">up</option>
                          <option value="right">right</option>
                          <option value="down">down</option>
                          <option value="left">left</option>
                      </select>
                      <input type="button" value="Add This Action" class="form-add-action-butt">
                  </form>
              </div>
              <p class="description-p">Add Actions for them to be displayed here</p>

              <div class="action-display-container">
              </div>

              <button class="run-actions-butt">Execute These Actions!</button>



          </div>

          <script>
              -console.log('client js running :)');

      const addActionButt = document.querySelector(".form-add-action-butt");
      const launchActionsButt = document.querySelector(".run-actions-butt");
      const canvas = document.querySelector(".canvas-display");
      canvas.width = 400
      canvas.height = 350


      const distInp = document.querySelector(".form-cm-input");
      const drawBoolInp = document.querySelector(".radio-bttn"); //gets only the first radio  button
      const selDirInput = document.querySelector(".form-select-direc");
      const actDispBox = document.querySelector(".action-display-container");
      const ctx = canvas.getContext('2d');
      const canActHei = 7;
      const canActWid = 8;
      let invalidPosPresent = false;


      let canPos = {
          'posX': canvas.offsetWidth / 2,
          'posY': canvas.offsetHeight / 2
      };

      let lastCanPos = {
          'posX': 0,
          'posY': 0
      };

      let actions = [];

      //event listeners
      addActionButt.addEventListener('click', addAction, false);
      launchActionsButt.addEventListener('click', launchActions, false);
      actDispBox.addEventListener('click', deleteEvent, false)
      //able deletion of action

      function deleteEvent(e) {
          if (e.target !== e.currentTarget) {
              if (e.target.className === 'delete-action-x') {
                  actions.splice(e.target.parentNode.id, 1);
                  refreshHtmlActions(refreshCanvasDisplay());
              }
          }
      }

      function addAction() {
          /*do stuff when action is added 
              1.create action object
              2.display it to the action display
              3.add action to array of all actions
          */
          let action = getFormData() //return an object {distance float, drawBool, direction string}
          console.log(action);
          actions.push(action);
          refreshHtmlActions(refreshCanvasDisplay()); //refresh canvas display return int for number of 
          //actions that did not violate the canvas bounderies.
      }

      async function launchActions() {
          /*do stuff when actions are added
              1.create the messege
              2.send and print response
              
              not doing whats below
              send the first action
              await confirmation that its done
              send the second
          */

          const options = {
              method: 'POST',
              headers: {
                  'Content-Type': 'application/json'
              },
              body: JSON.stringify(actions)
          };

          alert("sending your data to the server right now for execution!")
            response = await fetch('/launchActions', options);
          alert(response)
           

      }

      function getFormData() {
          return new Action(
              (distInp.value.length > 0) ? parseFloat(distInp.value) : 1,
              (drawBoolInp.checked),
              selDirInput.value
          );
      }

      function initCanvas() {
          canPos.posX = canvas.offsetWidth / 2;
          canPos.posY = canvas.offsetHeight / 2;

          ctx.clearRect(0, 0, canvas.width, canvas.height); //clear html5 canvas

          ctx.fillStyle = 'blue';
          ctx.lineWidth = 4.5;

          ctx.beginPath();
          ctx.moveTo(canPos.posX, canPos.posY);
      }

      function refreshCanvasDisplay() {
          initCanvas(); //init canvas to middle and no actions

          for (let i = 0; i < actions.length; i++) { //run thru all actions

              actions[i].moveCtxPos(); //move the canvas position but dont make the change on the canvas quite yes
              console.log(canPos);
              if (canPos.posX < 0 || canPos.posY < 0 || canPos.posX > canvas.offsetWidth || canPos.posY > canvas.offsetHeight) { //validation for too small
                  invalidPosPresent = true;
                  alert("action number " + i + " has violated the canvas boundery by moving to: (" + canPos.posX + "," + canPos.posY + ").\n Remember, max is (400,350). Action will not be counted to avoid hardware incidets");
                  canPos = lastCanPos;
                  actions.pop()
                  return i
              } else if (actions[i].draw) { //draw if draw
                  ctx.lineTo(canPos.posX, canPos.posY);
                  console.log("invoked draw");
              } else if (!(actions[i].draw)) { //move if no draw and error
                  ctx.moveTo(canPos.posX, canPos.posY);
              }
              invalidPosPresent = false;
              ctx.stroke()
          }
          return actions.length

      }

      function refreshHtmlActions(numOfActionsNotViolateCanBoundery) {
          actDispBox.innerHTML = ""; //delete everything in the box

          //every time an action is added or popped from
          // actions this should be invoked to redraw the actions\

          for (let i = 0; i < numOfActionsNotViolateCanBoundery; i++) {
              actDispBox.innerHTML += (actions[i].toHtmlString(i));
          }
      }

      class Action {

          constructor(length0, draw0, direction0) {
              //len in cm draw bool draw or just move marker
              //direction up down left or right
              this.len = length0;
              this.draw = draw0;
              this.direc = direction0;
              this.drawStr = this.draw ? "DRAW" : "NO DRAW";
          }

          toPost() {

          }

          moveCtxPos() {
              //save last pos in case of accident
              lastCanPos.posX = canPos.posX;
              lastCanPos.posY = canPos.posY;

              // let pxLen = (3 / 100) * window.innerWidth * this.len; below px len for trial version
              let pxLen = this.len * 50
              console.log("pxLen is: " + pxLen);
              switch (this.direc) {
                  case "up":
                      canPos.posY -= pxLen;
                      break;
                  case "right":
                      canPos.posX += pxLen;
                      break;
                  case "down":
                      canPos.posY += pxLen;
                      break;
                  case "left":
                      canPos.posX -= pxLen;
                      break;
                  default:
                      console.log("Something isn't working, an action with no direc entered the array.");
              }
          }

          toHtmlString(num) {
              return `<div class="action-box" id="` + String(num) + `">
              <!--later wanna add a way to rearraneg-->
              <span class="action-num">` + String(num) + `</span>
              <span class="delete-action-x">X</span>

              <div class="action-desc">
                  <p class="action-draw">` + this.drawStr + `</p>
                  <p class="action-len">` + String(this.len) + ` CM</p>
                  <p class="action-direc">` + (this.direc).toUpperCase() + `</p>
              </div>
          </div> 
          `
          }

      }

      /* 
      <div class="action-box">
          <!--later wanna add a way to rearraneg-->
          <span class="action-num">1</span>
          <span class="delete-action-x">X</span>

          <div class="action-desc">
              <p class="action-draw">DRAW</p>
              <p class="action-len">5 CM</p>
              <p class="action-direc">UP</p>
          </div>
      </div>
      */
          </script>
      </body>

      </html> 
      )";

    Serial.println("this is the condensedClientCode: "*condensedClientCode);
    return *condensedClientCode;
}

