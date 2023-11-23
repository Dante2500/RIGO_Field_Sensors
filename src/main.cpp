#include <Arduino.h>
#include <rak3272_lib_p2p.h>
#include <timer_functions.h> 
#include "pin_manager.h"
#include <DHTesp.h>
#include <rele_lib.h>
#include <encrypt_lib.h>


#define FREQ 915000000
#define SF 7
#define BW 0
#define CR 0
#define PREAMB_LEN 20
#define TX_PW 19


// variables estáticas de límites de calculo de temperatura y humedad
const float temp_limit_inf = 15;
const float temp_limit_sup = 60;
const float hum_limit_inf = 15;
const float hum_limit_sup = 65;
const float velvientomax = 1.5;

//ARREGLO DE DATOS
/*
  data[0] = temperatura externa
  data[1] = temperatura interna
  data[2] = humedad externa
  data[3] = humedad interna
*/

//Datos de manejo para el calculo de las heladas:
float data[4];
//float datat[4]={-11.4, 98, 1.3, 45.9};

byte intensidad=10;
char TipoHelada= '-';

//datos del timer
extern hw_timer_t *timer;
extern volatile bool toggle;
extern volatile bool flag;


//objetos: DHT y Relay
DHTesp dhtext, dhtint;
Relay rele(RELEPIN);
RAK3272 node1(FREQ, SF, BW, CR, PREAMB_LEN, TX_PW);

//valores de control del RELE
bool encendido=false; //True -> encender relay, False -> apagar Relay
bool automatico=true; //True -> MODO AUTOMATICO, False -> modo manual

//headers de las funciones
void init();
void actuadorReleMAN(bool Encen);
byte calculohelada(float EXTTemp, float EXTHum,float INTTemp,float INTHum, float VelViento);
void actuadorReleAUT(byte intensidadnueva);
void waitMessage();

void setup() {
  // put your setup code here, to run once:
  InitializeTimers(8000);
  pinMode(2, OUTPUT);
  init();
  node1.inicialize();
  delay(500);
  node1.sendCommand("AT+PRECV=65533",true);
  //loranode_inicialize
  //Serial.onReceive
}

void loop() {
  // put your main code here, to run repeatedly:
  if(flag==true){
    timerStop(timer);
    digitalWrite(2, toggle? HIGH:LOW);
    data[0] = dhtext.getTemperature();
    data[1] = dhtext.getHumidity();
    data[2] = dhtint.getTemperature();
    data[3] = dhtint.getHumidity();
    float velviento = 0.1;
    byte intensidadNueva;
    intensidadNueva=calculohelada(data[0], data[1], data[2], data[3], velviento);

    if(automatico){
      if(intensidadNueva != intensidad){
        actuadorReleAUT(intensidadNueva);
        intensidad = intensidadNueva;
      }
    }
    else{
      actuadorReleMAN(encendido);
    }
    String aux ="";
    aux= codificacion(data, 4);
    node1.sendMessage(aux);
    Serial.print("Data enviada: ");
    Serial.print(data[0]);
    Serial.print(" ");
    Serial.print(data[1]);
    Serial.print(" ");
    Serial.print(data[2]);
    Serial.print(" ");
    Serial.print(data[3]);
    Serial.println();
    Serial.println("MENSAJE ENVIADO : /"+aux +"/FIN DEL MENSAJE");
    flag=false;
    timerStart(timer);
  }
  //Serial.on

  //LEER VALORES SERIALES
  waitMessage();
}

void init(){
  Serial.begin(9600);
  Serial2.begin(115200);
  dhtint.setup(DHTDATA1, DHTesp::DHT22);
  dhtext.setup(DHTDATA2, DHTesp::DHT22);
  // override the default CS, reset, and IRQ pins (optional)
  pinMode(LEDCOM, OUTPUT);
  pinMode(LEDREC, OUTPUT);
}

// put function definitions here:


byte calculohelada(float EXTTemp, float EXTHum,float INTTemp,float INTHum, float VelViento){
  byte intensidadCalc;
  if(EXTTemp <= temp_limit_sup){
    if(EXTHum <= hum_limit_sup){
      TipoHelada = 'N';
      intensidadCalc = 3;  
    }
    else{
      TipoHelada = 'B';
      if(VelViento > velvientomax){
        intensidadCalc = 2;

      }
      else{
        intensidadCalc = 1;
      }
    }
    //rele.on();
  }
  else{
    intensidadCalc = 0;
    //rele.off();
  }

  return intensidadCalc;
}


void actuadorReleAUT(byte intensidadnueva){
    if(intensidadnueva == 0){
      rele.off();
    }
    else{
      rele.on();
    }
}


void actuadorReleMAN(bool Encen){
    if(Encen==true){
      rele.on();
    }
    else{
      rele.off();
    }
}

void waitMessage(){
  String message_LoRa=node1.receiveMessage();
  float datarec[2];
  if(message_LoRa != ""){
    Serial.println(message_LoRa);
    decodificacion(message_LoRa,datarec,2);
    Serial.println("La data decodificada es:");
    automatico = bool(datarec[0]);
    encendido = bool(datarec[1]);
    Serial.print(String(datarec[0])+" ");
    Serial.print(String(datarec[1])+" ");
    message_LoRa="";
  }
  
}