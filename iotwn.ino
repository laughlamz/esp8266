#include <ESP8266WiFi.h>

WiFiClient client;

void parseATText(char b);
void parseData(char *data);
void resetBuffer(void);
void control_device(void);

const char* ssid     = "Teslateq Co., Ltd";
const char* password = "TapTrung15Ty2018";
const char* host = "iotwhynot.teslateq.vn";
const int httpPort = 80;

enum _parseState {
  HTTP_DETECT_TYPE,
  HTTP_STATUS,
  HTTP_LENGTH,
  HTTP_CONTENT_TYPE,
  NEW_LINE,
  HTTP_CONTENT
};
byte parseState = HTTP_DETECT_TYPE;

char buffer[200];
char iBuf[200];
char device[10];
unsigned int pos;

int8_t staDevice1, staDevice2, staDevice3;
float sen1, sen2;
float espTemp=42.8;

unsigned long ovftime1, timeout1;

char httpStatus[10];
char httpLength[10];
char httpContent[200] = "000";
unsigned int posHttp;
unsigned int len;


void setup()
{   

    Serial.begin(115200);
    pinMode(2,OUTPUT);
    delay(10);
    
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    delay(500);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    delay(500);
}

void loop()
{
    if( client.connect(host, httpPort) ) {
      
      client.print(String("GET /data.php?") + "s=" + httpContent);
//      if(millis() - ovftime1 > 2000){
//        ovftime1 = millis();
//        client.print(String("&sensor1=") + sen1 + "&sensor2=" + sen2 + "&sensor3=" + String(espTemp,2));
//      }
      client.print(String(" HTTP/1.1\r\n") +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
                   
      timeout1 = millis();           
      while(!client.available()){
        delay(1);
        if(millis() - timeout1 > 2000){
          Serial.println("Timeout1");
          break;
        }
      }
      
      while (client.available()) {
        char ch = client.read();
        Serial.print(ch);         //DEBUG (useful)
        parseATText(ch);
      }
      
      client.stop();
      client.flush();
    }
    else
      Serial.println("xxx Connection failed!");
    
    Serial.println();
    Serial.println("xxx");
    Serial.println(httpStatus);
    Serial.println(atoi(httpLength));
    Serial.println(httpContent);  
    //Serial.println(parseState);
    Serial.println();

    if( strcmp(httpStatus, "200") == 0) {
      for(int i=0; i<atoi(httpLength); i++)
        iBuf[i] = httpContent[i];
    }
    Serial.println(iBuf);
    
    control_device();

    delay(500);
        
}


void control_device(void){
  if(device[0] == '1'){
    digitalWrite(2,LOW);
    staDevice1 = 1;
  }
  else if(device[0] == '0'){  //prevent Dat or orther words
    digitalWrite(2,HIGH);
    staDevice1 = 0;
  }

  if(device[1] == '1'){
//  digitalWrite(12,HIGH);
    staDevice2 = 1;
  }
  else if(device[1] == '0'){
//  digitalWrite(12,LOW);
    staDevice2 = 0;
  }  

  if(device[2] == '1'){
//  digitalWrite(4,HIGH);
    staDevice3 = 1;
  }
  else if(device[2] == '0'){
//  digitalWrite(4,LOW);
    staDevice3 = 0;
  }  
}

void resetBuffer(void) {
  memset(buffer, 0, sizeof(buffer));
  pos = 0;
}


void parseATText(char b) {
  
  buffer[pos++] = b;
  
  if(pos >= sizeof(buffer))
    resetBuffer();
    
  switch(parseState)
  {
    case HTTP_DETECT_TYPE: 
      if(b == '\n')
        resetBuffer();
      else if(b == '/')
      {
        if( strcmp(buffer, "HTTP/") == 0 )
          parseState = HTTP_STATUS;
      }
      else if(b == ':')
      {
        if( strcmp(buffer, "Content-Length:") == 0 )
          parseState = HTTP_LENGTH;
        else if( strcmp(buffer, "Content-Type:") == 0 )
          parseState = HTTP_CONTENT_TYPE;
      }
      break;

    case HTTP_STATUS:
      if(b == '\n') {
        for(int i=9; i<9+3; i++)
          httpStatus[i-9] = buffer[i];

        parseState = HTTP_DETECT_TYPE;
        resetBuffer();
      }
      break;

    case HTTP_LENGTH:
      if(b == '\n') {
        len = atoi(httpLength);
        posHttp = 0;
        parseState = HTTP_DETECT_TYPE;
        resetBuffer();
      }
      else {
        httpLength[posHttp++] = b;
      }
      break;
    
    case HTTP_CONTENT_TYPE:
      if(b == '\n') {
        parseState = NEW_LINE;
        resetBuffer();
      }
      break;
    
    case NEW_LINE:
      if(b == '\n') {
        parseState = HTTP_CONTENT;
        resetBuffer();
      }
      break;

    case HTTP_CONTENT:
      if(len--) {
        httpContent[posHttp++] = b;
      }
      if(len == 0) {
        posHttp = 0;
        parseState = HTTP_DETECT_TYPE;
        resetBuffer();  
      }
      break;
    
  }
}
