#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
char* ssid = "BadWifi";
char* password =  "Sanmina02";


class Action
{
private:
    double len;
    bool draw;
    String dir;
public:
    Action(double len0, bool draw0, String dir0) {
        len=len0;
        draw=draw0;
        dir = dir0;
    }

    Action() {
        len=0;
        draw=0;
        dir = "null";
    }

    double getLen() {
        return len;
    }

    bool getDraw() {
        return draw;
    }

    String getDir() {
        return dir;
    }

    void setLen(double len0) {
        len = len0;
    }

    void setDraw(bool draw0) {
        draw = draw0;
    }

    void setDir(String dir0) {
        dir = dir0;
    }

    String stringify() {
        return "Len: " + String(len)+ ". Draw: " + String(draw) + ". Dir: " + dir;
    }

    ~Action() {
        Serial.println("Action deleted");
    }
};



void setup() {

    Serial.begin(115200);
    delay(4000);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println("Connected to the WiFi network");

}

void loop() {

    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

        HTTPClient http;
        String payload;

        http.begin("http://192.168.0.33:3000/getAction"); //Specify the URL
        int httpCode = http.GET();                                        //Make the request

        if (httpCode > 0) { //Check for the returning code

            payload = http.getString();
            Serial.println(httpCode);
            Serial.println(payload);
            http.end(); //Free the resources

            int actionsLen = (payload.substring(0, payload.indexOf("&"))).toInt();
            String strData = payload.substring(payload.indexOf("&")+1, payload.length());
            Serial.println(strData);

            String tempDir = "";
            bool tempDraw = false;
            double tempLen = 0;
            Action actions[actionsLen];

            for (int i=0;i<actionsLen;i++) {
                tempLen = (strData.substring(0, strData.indexOf(","))).toDouble();
                strData = strData.substring(strData.indexOf(",")+1, strData.length()); //removes the string already used
                Serial.println(strData);
                tempDraw= (strData.substring(0, strData.indexOf(","))=="true");
                strData = strData.substring(strData.indexOf(",")+1, strData.length()); //removes the string already used
                Serial.println(strData);
                tempDir = strData.substring(0, strData.indexOf(","));
                strData = strData.substring(strData.indexOf(",")+1, strData.length()); //removes the string already used
                Serial.println(strData);
                  
                Serial.println("tempLen: " + String(tempLen) + "tempDraw: " + String(tempDraw) + "tempDir: " + tempDraw);
                    //once we have data, push to array!
                    actions[i] = Action(tempLen, tempDraw, tempDir);
            }

            Serial.println("printing all actions: ");
            for (int j =0; j<actionsLen; j++) {
                Serial.println(actions[j].stringify());
            }




        }//end of if http request works

        else {
            Serial.println("Error on HTTP request");
        }

    }

    delay(100000000);
}
