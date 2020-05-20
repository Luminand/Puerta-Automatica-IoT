#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
/******* definicion de puertos de la placa**********/
#define PUERTA            4
#define LED               5
#define Boton_exterior    18
#define Boton_interior    19
#define LED_INICIO        2
/*******funciones de ejecucion********/
void Puerta_Abierta();
void Puerta_Cerrada();
/*********variables**********/
boolean Bandera_manual=true;
boolean BanderaD;
int BotonE;
int BotonI;
char Puerta_state;
/************************* WiFi Access Point *********************************/
#define WLAN_SSID       "INFINITUMAAEE_2.4"
#define WLAN_PASS       "krBLdkz36G"
/************************* Adafruit.io Setup *********************************/
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "ErickRH"
#define AIO_KEY       "aio_OJIT41WHpPzuIQCoszjgjNtOuvN8"

WiFiClient client;
/*********************** declaracion de clientes y publicaciones************************/
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish puertaE = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/puertaE");
Adafruit_MQTT_Subscribe puerta = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/puerta");
Adafruit_MQTT_Subscribe controlmanual = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/controlmanual");

void MQTT_connect();
void setup() {
  
  Serial.begin(115200);
  delay(10);
/**** inicializacion de puertos*****/
  pinMode(PUERTA, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(Boton_exterior,INPUT);
  pinMode(Boton_interior,INPUT);
  pinMode(LED_INICIO, OUTPUT); 
/****** conexion**************/
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED_INICIO,HIGH);
        delay(200);
    digitalWrite(LED_INICIO,LOW);
        delay(200);
     }

  digitalWrite(LED_INICIO,HIGH);
  /******subscripciones****/
  mqtt.subscribe(&puerta);
  mqtt.subscribe(&controlmanual);
}



void loop() {
   
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  /*******lectura de subscripciones********/
  while ((subscription = mqtt.readSubscription(300))) {
        if (subscription == &puerta) {
                  Puerta_state = *puerta.lastread;                 
                  if(Puerta_state=='A'){  
                       Puerta_Abierta();                                     
                  }
                  if(Puerta_state=='C'){
                     Puerta_Cerrada();                 
                  } 
          }      
         if (subscription == &controlmanual) {
                  char Control_state = *controlmanual.lastread;
                  if(Control_state=='A'){
                       Bandera_manual=true;
                  }
                  if(Control_state=='D'){
                       Bandera_manual=false;
                  }  
          }              
 }
/*********lectura de botones************/
  BotonE=digitalRead(Boton_exterior);
  BotonI=digitalRead(Boton_interior);
  if(Bandera_manual==true&&((BotonI==HIGH)||(BotonE==HIGH))){
        if(BanderaD==true){
          Puerta_Cerrada();
        }
        else{
          Puerta_Abierta();
        }
  }
}


/*****apertura de puerta *****/
void Puerta_Abierta(){
  digitalWrite(PUERTA,HIGH);
  digitalWrite(LED,HIGH);
  BanderaD=true;
  puertaE.publish("1");
}


/*****cierre de puerta*****/
void Puerta_Cerrada(){
digitalWrite(PUERTA,LOW);
digitalWrite(LED,LOW);
BanderaD=false;
puertaE.publish("0");                 
}



/*************reconexion con la red**************/
void MQTT_connect() {
  int8_t ret;

    if (mqtt.connected()) {
      return;
    }
  uint8_t retries = 3;

    digitalWrite(LED_INICIO,HIGH);
        delay(200);
    digitalWrite(LED_INICIO,LOW);
        delay(200);
    digitalWrite(LED_INICIO,HIGH);
        delay(200);
    digitalWrite(LED_INICIO,LOW);
        delay(200);
  
  while ((ret = mqtt.connect()) != 0) {        
            mqtt.disconnect();
            delay(5000);  
            retries--;
            if (retries == 0) {
              while (1)
              digitalWrite(LED_INICIO,LOW);
            }
     }
     
  Serial.println("MQTT Connected!");
  digitalWrite(LED_INICIO,HIGH);
  delay(1000);
  digitalWrite(LED_INICIO,LOW);
  delay(1000);
  digitalWrite(LED_INICIO,HIGH);
}
