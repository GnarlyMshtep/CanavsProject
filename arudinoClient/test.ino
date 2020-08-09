    #include <WiFi.h>
    #include <HTTPClient.h>

    const char* ssid = "BadWifi";
    const char* password =  "Sanmina02";

    class Action
    {
    private:
        double len;
        bool draw;
        String dir;
    public:
        Action(double len0, bool draw0, String direc0) {
            len=len0;
            draw=draw0;
            dir = dir0;
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
            len = len0
        }

        void setDraw(bool draw0) {
            draw = draw0
        }

        void setDir(String dir0) {
            dir = dir0;
        }

        ~Action();
    };

    Action::Action(/* args */)
    {
    }

    Action::~Action() {
        Serial.println("Action deleted");
    }




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

                http.end(); //Free the resources

                int actionsLen = payload.substring(0, payload.indexOf("&"))
                    Action* arr[] = parseStrData();
                for (int i=0; i<arr.length(); i++) {

                }


            }

            else {
                Serial.println("Error on HTTP request");
            }

        }

        delay(10000);

    }

    Action *[] parseStrData(String strData) {
        String tempDir = "";
        bool tempDraw = false;
        double tempLen = 0;
        Action *actPtr[100];
        int i=0; //index fro cycling thru arr

        while (strLen(strData)>0 && strData!=",") { //while there is stil data to parse or 
            tempLen = toDouble(strData.substring(0, strData.indexOf(",")));
            strData = strData.substring(strData.indexOf(","), strData.length()); //removes the string already removed
            tempDraw= (strData.substring(0, strData.indexOf(","))=="true");
            strData = strData.substring(strData.indexOf(","), strData.length()); //removes the string already removed
            tempDir = strData.substring(0, strData.indexOf(","));
            strData = strData.substring(strData.indexOf(","), strData.length()); //removes the string already removed

            //once we have data, push to array!

            actPtr[i]= new Action(tempLen, tempDraw, tempDirec);
            i++;
        }

        return tempDir;


    }