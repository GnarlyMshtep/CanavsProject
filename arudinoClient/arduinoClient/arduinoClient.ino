/*Additional Info:
    -distance unit of messurments is cm
    -server must be running (so request could be fullfiled)
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
char* ssid = "BadWifi";
char* password =  "Sanmina02";

const Vector CEN_VEC =  Vector(4, 3.5);
const float S_DIST = .05; //the amount of error we are willing to except, exactness can't really be achieved (I belive)

class Action //encapsulated structure to orginize the data for a single action
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

class Vector { //arudino read from top left to avoid negative values (can keep all numbers unsigned)
private:
    double x, y;

public:
    Vector(double x_, double y_) {
        x= x_;
        y=y_;
    }

    double getX() const {
        return x;
    }

    double getY() const {
        return y;
    }

    void setX(double &x_) {
        x= x_;
    }

    void setY(double &y_) {
        x= y_;
    }

    template<typename T>
    Vector operator +(T num) {
        Vector(x+num, y+num);
    }

    template<typename T>
    Vector operator -(T num) {
        Vector(x-num, y+-num);
    }

    Vector operator -(Vector &v) {
        Vector(x-v.getX(), y-v.getY());
    }
};

void setup() { //basiclly just connects to Wifi

    Serial.begin(115200);
    delay(4000);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    Serial.println("Connected to the WiFi network");

}

void loop() { //part1: send a GET request to the server (hosted on a the local network the audrino is connected to) -> get back a string that describes an array of actions.

    centerArm();

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

            //part 2: we take the payload (response to get request) and convert it into an array of Actions (class) using string manipulation


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

                tempDraw= (strData.substring(0, strData.indexOf(","))=="true");
                strData = strData.substring(strData.indexOf(",")+1, strData.length()); //removes the string already used

                tempDir = strData.substring(0, strData.indexOf(","));
                strData = strData.substring(strData.indexOf(",")+1, strData.length()); //removes the string already used


                Serial.println("tempLen: " + String(tempLen) + "tempDraw: " + String(tempDraw) + "tempDir: " + tempDir);
                //once we have data, push to array!
                actions[i] = Action(tempLen, tempDraw, tempDir);
            }

            /* Serial.println("printing all actions: ");
             for (int j =0; j<actionsLen; j++) {
                 Serial.println(actions[j].stringify());
             }*/

             /* psuedocode for part3

             for(action in actions){
                 action.getDraw() ? draw() : noDraw();
                 Vector* dir = &VecFromAct(action)
                 while(abs(dir.getX())<S_DIST && abs(dir.getY())<S_DIST){
                     moveArm()
                 }

             }

             */
        }//end of if http request works

        else {
            Serial.println("Error on HTTP request");
        }

    }

    delay(100000000);
}


void centerArm() {
    Vector diffrence= getCurrentVect() - CEN_VEC;

    noDraw();
    while (abs(diffrence.getX())<S_DIST && abs(diffrence.getY())<S_DIST) {
        moveArm(diffrence);
        diffrence =-S_DIST;
    }
}

void moveArm(Vector dir) {
    //move the arm in the direction of the vector. first move X by an increment and then y (since continous action not allowed)

    //move x an increment in dir by using sensor to messure when its moved that
    //move y an increment in dir sensor to messure when its moved that
}

void noDraw() {
    //check if 
    //raise the pen off the canvas
}

void draw() {
    //check if
    //put the pen on the canvas
}

Vector* VecFromAct(Action &act) {
    switch (act.getDir())
    {
    case "up":
        return &Vector(0, act.getLen());
        break;

    case "down":
        return &Vector(0, -act.getLen());
        break;
    case "right":
        return &Vector(act.getLen(), 0);
        break;
    case "left":
        return &Vector(-act.getLen(), 0)
            break;


    default:
        break;
    }
}