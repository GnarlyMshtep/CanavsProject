/*Additional Info:
    -distance unit of messurments is cm
    -server must be running (so request could be fullfiled)
    -(0,0) is at the top left (to avoid negative readings)
    -
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

char* ssid     = "NETGEAR-5G";
char* password = "alon12345";

const int upM = 4;    //up
const int riM = 14;   //right
const int dowM = 5;   //down 
const int lefM = 12;  //left

const int servo = 10; //servo is the pin for the gear that puts the marker on and off the canvas. HIGH = draw LOW = noDraw;
const int trigPin_vertical = 13;
const int echoPin_vertical = 15;
const int trigPin_horizontal = 1;
const int echoPin_horizontal = 3;

const int S_TIME = 500; //in ms
const float S_DIST = .05; //the amount of error we are willing to except, exactness can't really be achieved (I belive)

void setup() { //basiclly just connects to Wifi

    Serial.begin(115200);

    Serial.println("hello I am happening");
    //intialize arduino divices
    pinMode(servo, OUTPUT);
    pinMode(upM, OUTPUT);
    pinMode(riM, OUTPUT);
    pinMode(dowM, OUTPUT);
    pinMode(lefM, OUTPUT);
    pinMode(trigPin_vertical, OUTPUT); // Sets the trigPin as an Output                                   
    pinMode(echoPin_vertical, INPUT); // Sets the echoPin as an Input                                     
    pinMode(trigPin_horizontal, OUTPUT); // Sets the trigPin as an Output                                       
    pinMode(echoPin_horizontal, INPUT); // Sets the echoPin as an Input 
    // Set outputs to LOW, nothing needs to be activated just yet :)
    digitalWrite(upM, LOW);
    digitalWrite(riM, LOW);
    digitalWrite(dowM, LOW);
    digitalWrite(lefM, LOW);
    digitalWrite(servo, HIGH);


    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }

    Serial.println("Connected to the WiFi network");

}


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

class Vector { //arudino read from top left to avoid negative values
private:
    double x, y;

public:
    Vector(double x_, double y_) {
        x = x_;
        y = y_;
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
        Vector(x-num, y-num);
    }

    Vector operator -(Vector v) {
        Vector(x-v.getX(), y-v.getY());
    }

    Vector _abs() {
        return Vector(abs(x), abs(y));
    }
};


const Vector CEN_VEC =  Vector(4, 3.5);

void noDraw() {
    digitalWrite(servo, LOW);
}

void draw() {
    digitalWrite(servo, HIGH);
}

Vector VecFromAct(Action &act) {
    if (act.getDir()=="up") {
        return Vector(0, -act.getLen());
    }
    else if (act.getDir()=="down") {
        return Vector(0, act.getLen());
    }
    else if (act.getDir()=="right") {
        return Vector(act.getLen(), 0);
    }
    else if (act.getDir()=="left") {
        return Vector(-act.getLen(), 0);
    }
    return Vector(0, 0);
}

Vector getCurrentPos() {
    return Vector(horizontal_sonar(), vertical_sonar());
}

double vertical_sonar()
{
    delay(50);
    digitalWrite(trigPin_vertical, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin_vertical, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin_vertical, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    double duration_vertical_sonar = pulseIn(echoPin_vertical, HIGH);

    // Calculating the distance
    return duration_vertical_sonar*0.034/2;
}

double horizontal_sonar()
{
    delay(50);
    digitalWrite(trigPin_horizontal, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin_horizontal, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin_horizontal, LOW);

    // Reads the echoPin, returns the sound wave travel time in microseconds
    double duration_horizontal_sonar = pulseIn(echoPin_horizontal, HIGH);

    // Calculating the distance
    return duration_horizontal_sonar*0.034/2;
}


void loop() { //part1: send a GET request to the server (hosted on a the local network the audrino is connected to) -> get back a string that describes an array of actions.

    centerArm();

    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

        HTTPClient http;
        String payload;

        http.begin("http://192.168.0.156:3000/getAction"); //Specify the URL
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

            Serial.println("printing all actions: ");
            for (int i =0; i<actionsLen; i++) {
                Serial.println(actions[i].stringify());
            }

            for (int i =0;i<actionsLen;i++) {
                actions[i].getDraw() ? draw() : noDraw(); //decide if we are drawing on the canvas or if the pen is raised.

                drawAction(actions[i], getCurrentPos());

            }

        }//end of if http request works

        else {
            Serial.println("Error on HTTP request (oh no!)");
        }

    }

    delay(100000000);
}

void centerArm() {
    //2 dimensonal application of drawAction(), call it horizontaly and verticly depending on the diffrence from current position
    Vector diffrence = getCurrentPos()- CEN_VEC;
    noDraw();

    //first, horizontall

    if (diffrence.getX()<0) {
        Action tempHorAct = Action(abs(diffrence.getX()), false, "left");
        drawAction(tempHorAct, getCurrentPos());
    }
    else
    {
        Action tempHorAct = Action(abs(diffrence.getX()), false, "right");
        drawAction(tempHorAct, getCurrentPos());
    }

    //second, horizontall

    if (diffrence.getY()<0) {
        Action tempHorAct = Action(abs(diffrence.getY()), false, "up");
        drawAction(tempHorAct, getCurrentPos());
    }
    else
    {
        Action tempHorAct = Action(abs(diffrence.getY()), false, "down");
        drawAction(tempHorAct, getCurrentPos());
    }

}


void drawAction(Action act, Vector curPos) {
    Vector delta = Vector(abs(act.getLen()-curPos.getX()), abs(act.getLen()-curPos.getY()));
    Vector pos0 = Vector(0, 0);


    if (act.getDir()=="up") {
        while (delta.getY()>0 + S_DIST) {
            pos0 = getCurrentPos();
            digitalWrite(upM, HIGH);
            delay(S_TIME);
            digitalWrite(upM, LOW);
            delay(S_TIME);
            delta = delta - (pos0 - getCurrentPos())._abs();
        }

    }
    else if (act.getDir()=="down") {
        while (delta.getY()>0 + S_DIST) {
            pos0 = getCurrentPos();

            digitalWrite(dowM, HIGH);
            delay(S_TIME);
            digitalWrite(dowM, LOW);
            delay(S_TIME);
            delta = delta - (pos0 - getCurrentPos())._abs();
        }
    }
    else if (act.getDir()=="right") {
        while (delta.getX()>0 + S_DIST) {
            pos0 = getCurrentPos();

            digitalWrite(riM, HIGH);
            delay(S_TIME);
            digitalWrite(riM, LOW);
            delay(S_TIME);
            delta = delta - (pos0 - getCurrentPos())._abs();
        }
    }
    else if (act.getDir()=="left") {
        while (delta.getX()>0 + S_DIST) {
            pos0 = getCurrentPos();

            digitalWrite(lefM, HIGH);
            delay(S_TIME);
            digitalWrite(lefM, LOW);
            delay(S_TIME);
            delta = delta - (pos0 - getCurrentPos())._abs();
        }
    }

}
