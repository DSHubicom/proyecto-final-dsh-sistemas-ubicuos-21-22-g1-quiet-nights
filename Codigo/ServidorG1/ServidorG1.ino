#include <WiFi.h>
#include <WebServer.h>
//Alarmas
#include <uri/UriBraces.h>
#include "Arduino.h"
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <iostream>
#include <string>

//Creamos nuestra propia red
const char* ssid= "ssid";
const char* password = "password";

WebServer server(80); // puerto por defecto 80

//Alarmas inicialización 
SoftwareSerial mySoftwareSerial(25, 26); // RX, TX
DFRobotDFPlayerMini Alarmas;
void printDetail(uint8_t type, int value);
//Procedimiento que configura e inicializa los dispositivos y la conexión
void setup() {
  
//CONFIGURACIÓN DE LA CONEXIÓN
Serial.begin(115200);
//creamos el punto de acceso
WiFi.softAP(ssid,password); //tiene mas parámetros opcionales
//WiFi.softAPConfig(ip,gatewaty,subnet);
IPAddress ip = WiFi.softAPIP();

//Informamos de la conexión
Serial.print("Nombre de mi red esp32: ");
Serial.println(ssid);
Serial.print("La IP es: ");
Serial.println(ip);
server.begin();
Serial.println("Servidor HTTP iniciado");
Serial.println("\nDispositivos Contactados:");  
//-----------------------------------------------------------------------------------------------------------
//CONFIGURACIÓN DE LA ALARMA
 mySoftwareSerial.begin(9600);
  Serial.begin(115200); 
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  if (!Alarmas.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
     Alarmas.reset();     //Reset the module
    }
  }
  Serial.println(F("DFPlayer Mini online."));
  
  Alarmas.volume(30);  //Set volume value. From 0 to 30
//-----------------------------------------------------------------------------------------------------------  
//RECEPCIÓN DE LOS DATOS QUE SE MANDAN DESDE LOS CLIENTES AL SERVIDOR
//Si no mandamos nada
server.on("/",handleConnectionRoot);
//Si mandamos la información de la alfombra
server.on("/Alfombra", HTTP_GET, handleAlfombra);
//Si mandamos la información del sensor de movimiento 
server.on("/Incorpora", HTTP_GET, handleIncorpora);
//Si mandamos la información de los sensores de salud
server.on("/Salud",HTTP_GET,handleSalud);
//Si mandamos la información del sensor de temperatura
//server.on("/Temperatura",HTTP_GET,handleTemperatura);
//Si no encuentra la respuesta
server.onNotFound(handleNofFound);
}


//Estamos siempre pendientes de lo que llegue
void loop() {
  server.handleClient();   
   delay(2000);
}

//Responder a la url raíz (root /)
void handleConnectionRoot(){
  server.send(200,"text/plain","Hola desconocido!");
  }
//Sensor de presión de la alfombra
 void handleAlfombra() {  
    Serial.println("Datos Alfombra Servidor: " + server.arg(String("presion")));   
   // devolver respuesta
   server.send(200, "text/plain", String("Respuesta Servidor Alfombra: ") + server.arg(String("presion")));
  //Activamos la alarma en caso de ser necesario
  String pres = server.arg(String("presion"));
    int presion = pres.toInt();
  if (presion > 2000) {
    //Aviso se ha levantado         
     Alarmas.play(3);
  }
}
//Sensor de movimiento del pijama
void handleIncorpora() {

   String posicion = server.arg("posicion");
   String temperatura = server.arg("temperatura"); 
   Serial.println("Datos Movimiento en el Servidor: " + posicion);   
   Serial.println("Datos Temperatura en el Servidor: " + temperatura);   
   
   // devolver respuesta
   server.send(200, "text/plain", String("Respuesta Servidor Movimiento: ") + server.arg(String("posicion")) + " Temperatura: "+ server.arg(String("temperatura")));
  //Activamos la alarma en caso de ser necesario
  String Pos = server.arg(String("posicion"));  
  if (Pos=="Incorporado"){
    //Aviso se ha incorporado
    Alarmas.play(2);         
  }
  String Temperatura = server.arg(String("temperatura"));
  float Temp = Temperatura.toFloat();  
  if (Temp > 38) {    
    //Aviso de Problema de salud.    
     Alarmas.play(1);
  } 
}
//Sensores de salud, ritmo cardíaco y Oxígeno en sangre  
   void handleSalud() {
   Serial.println("Datos Salud Servidor: Frecuencia: " + server.arg(String("frecuencia")) + " Oxígeno: " + server.arg(String("oxigeno")));      
    // devolver respuesta   
   server.send(200, "text/plain", String("Respuesta Servidor Salud: Frecuencia: " + server.arg(String("frecuencia")) + String(" oxígeno: ") + server.arg(String("oxigeno"))));
   //Activamos la alarma en caso de ser necesario
  String Frecuencia = server.arg(String("frecuencia"));  
  float Frec = Frecuencia.toFloat();
  if (Frec < 30 || Frec > 180){
     //Aviso de Problema de salud.    
     Alarmas.play(1);  
  }
  String Oxigeno = server.arg(String("oxigeno"));
  float Oxi = Oxigeno.toFloat();  
  if (Oxi < 60) {    
    //Aviso de Problema de salud.    
     Alarmas.play(1);
  } 
 }  
//Respuesta cuando no se encuentra 
  void handleNofFound(){
    server.send(404,"text/plain","Not found");
    }
    
