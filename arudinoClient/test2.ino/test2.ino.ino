#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "yourNetworkName";
const char* password =  "yourPassword";

void setup() { //connect to wifi

    Serial.begin(115200);
    delay(4000);
    WiFi.begin(ssid, password);

    Serial.print("Connecting to WiFi..");
    while (WiFi.status() != WL_CONNECTED) {

        delay(500);
        Serial.print(.);
    }

    Serial.println("Connected to the WiFi network");

}

void loop() {

    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

        HTTPClient http;

        http.begin("https://localhost:3000/getAction"); //Specify the URL
        int httpCode = http.GET();                                        //Make the request

        if (httpCode > 0) { //Check for the returning code if less then 0, internal error

            String payload = http.getString();
            Serial.println(httpCode);
            Serial.println(payload);
        }

        else {
            Serial.println("Error on HTTP request");
        }

        http.end(); //Free the resources
    }

    delay(10000);

}
