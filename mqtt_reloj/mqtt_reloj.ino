//***********************************//
//Modulo des temperatura y humedad   //
//                                   //
//Matias Pierdominici                //
//mpierdominici@itba.edu.ar          //
//***********************************//
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define DEBUGG


#define PIN_GAUGE_1 D2
#define PIN_GAUGE_2 D1
#define PIN_GAUGE_3 D5
#define PIN_GAUGE_4 D6
#define PIN_LUCES_GAUGE D7
#define PIN_LUCES_EXT D8
#define PIN_RELE D4


int pir1State=0;
int pir2State=0;
int pir3State=0;
int pir4State=0;

class myTimer
{
  public:
  myTimer(unsigned int seconds=0);// lo canvie a ms
  bool timeOver(void);
  void setNewTime(unsigned long seconds_);
  void showInfo();
  
  unsigned long seconds;
  unsigned long startTime;
  void resetTimer(void);
    
};


myTimer sampleTime(250);





char * ssid ="WIFI Pier";
char * pass ="pagle736pagle";
unsigned int mqttPort=1883;

const char MqttUser[]="relojAnalog";
const char MqttPassword[]="1234";
const char MqttClientID[]="relojAnalog";

bool pirSensorState=false;

IPAddress mqttServer(192,168,0,116);

WiFiClient wclient;
PubSubClient mqtt_client(wclient);





void callback(char* topic, byte* payload, unsigned int length);
void  debug_message (char * string, bool newLine)
{
#ifdef DEBUGG
  if(string !=NULL)
  {
    if (!newLine)
    {
      Serial.print(string);
    }else
    {
      Serial.println(string);
    }
  }
  #endif
}

void setUpWifi(char * ssid, char * pass)
{
  String ip;
  debug_message(" ",true);
  debug_message(" ",true);
  debug_message("Conectandose a: ",false);
  debug_message(ssid,true);

  WiFi.begin(ssid,pass);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    debug_message(".",false);
  }
  debug_message(" ",true);
  debug_message("Coneccion realizada",true);
  debug_message("La ip es: ",false);
  ip=WiFi.localIP().toString();
  debug_message((char *)ip.c_str(),true);
}

void setUpMqtt(void)
{
  mqtt_client.setServer(mqttServer,mqttPort);
  mqtt_client.setCallback(callback);
}


void callback(char* topic, byte* payload, unsigned int length)
{
  int tiempo=0;
  payload[length]='\n';
  String message((char *)payload);
  debug_message("Llego un mensage, topic:",false);
  debug_message(topic,false);
  debug_message(", payload : ",false);
  debug_message((char *)payload,true);

   if(!strcmp(topic,"gauge/l/on")){
    digitalWrite(PIN_LUCES_GAUGE,HIGH);
    
   }
   if(!strcmp(topic,"gauge/l/off")){
    digitalWrite(PIN_LUCES_GAUGE,LOW);
    
   }


   if(!strcmp(topic,"gauge/l/e/on")){
    digitalWrite(PIN_LUCES_EXT,HIGH);
    
   }
   if(!strcmp(topic,"gauge/l/e/off")){
    digitalWrite(PIN_LUCES_EXT,LOW);
    
   }

    if(!strcmp(topic,"gauge/r/on")){
    digitalWrite(PIN_RELE,HIGH);
    
   }
   if(!strcmp(topic,"gauge/r/off")){
    digitalWrite(PIN_RELE,LOW);
    
   }
  
     
  
  if(!strcmp(topic,"gauge/1")){
    int temp=message.toInt();
   float t=0.0007*temp*temp*temp-0.0539*temp*temp+3.7*temp+0.7768;
   t=(t/255.0)*1024;
   temp=t;
     analogWrite(PIN_GAUGE_1,temp);
      
  }

    if(!strcmp(topic,"gauge/2")){
      int temp=message.toInt();
   float t=0.0007*temp*temp*temp-0.0539*temp*temp+3.7*temp+0.7768;
   t=(t/255.0)*1024;
   temp=t;
      analogWrite(PIN_GAUGE_2,temp);
      
  }
    if(!strcmp(topic,"gauge/3")){
      int temp=message.toInt();
   float t=0.0007*temp*temp*temp-0.0539*temp*temp+3.7*temp+0.7768;
   t=(t/255.0)*1024;
   temp=t;
      analogWrite(PIN_GAUGE_3,temp);
      
  }
    if(!strcmp(topic,"gauge/4")){
      int temp=message.toInt();
   float t=0.0007*temp*temp*temp-0.0539*temp*temp+3.7*temp+0.7768;
   t=(t/255.0)*1024;
   temp=t;
      analogWrite(PIN_GAUGE_4,temp);
      
  }



}

void reconnect()
{
  while(!mqtt_client.connected())
  {
    debug_message("Intentando conectar al servidor MQTT",true);
    if (mqtt_client.connect(MqttClientID,MqttUser,MqttPassword))
      {
            debug_message("conectado",true);
  
  
            // ...suscrivirse a topicos
            mqtt_client.subscribe("gauge/1");
            mqtt_client.subscribe("gauge/2");
            mqtt_client.subscribe("gauge/3");
            mqtt_client.subscribe("gauge/4");
            mqtt_client.subscribe("gauge/l/on");
            mqtt_client.subscribe("gauge/l/off");
            mqtt_client.subscribe("gauge/l/e/on");
            mqtt_client.subscribe("gauge/l/e/off");
             mqtt_client.subscribe("gauge/r/on");
            mqtt_client.subscribe("gauge/r/off");
      }
      else
      {
        debug_message("intentando conetarse al broker",true);
        delay(3000);
      }
  }
}

void setup() {
  Serial.begin(9600);
  setUpWifi(ssid,pass);
  setUpMqtt();
  pinMode(PIN_LUCES_GAUGE,OUTPUT);
  pinMode(PIN_RELE,OUTPUT);
  pinMode(PIN_LUCES_EXT,OUTPUT);
}



void loop() {

  if (!mqtt_client.connected()) 
  {
      reconnect();
      
 }
 mqtt_client.loop();
}





//***********************TIMER**********************************



myTimer::myTimer(unsigned int seconds)
{
  setNewTime(seconds);
}

//timeOver
//devuelve true si ya paso el tiempo seteado,
//caso contrario devuelve false
//
bool myTimer::timeOver(void)
{
  if((millis())>startTime)
  {
    resetTimer();
    return true;
  }
  else
  {
    return false;
  }
}

void myTimer::resetTimer(void)
{
  unsigned long temp=seconds+millis();
 
  startTime=temp;
  //Serial.print("se llamo a rest timer con: ");
  //Serial.println(startTime);
}

void  myTimer::setNewTime(unsigned long seconds_)
{
  unsigned long temp=seconds_;
  //Serial.println(temp);
  seconds=temp;
 
  //Serial.print("s seteo un timer cada: ");
  //Serial.print(seconds_);
  //Serial.print(" se registro un tirmpo de: ");
  //Serial.println(seconds/1000);
  resetTimer();

}

void myTimer::showInfo()
{
  //Serial.println(startTime);
  unsigned long dif=startTime-millis();
  //Serial.print("Remaining time (seconds):");
  //Serial.println(dif/1000);
  //Serial.println(startTime);
  //Serial.println(millis());
  //Serial.println(seconds/1000);
}
