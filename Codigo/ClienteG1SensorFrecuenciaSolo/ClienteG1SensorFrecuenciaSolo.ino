#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "MAX30100_PulseOximeter.h"


//Credenciales de la red a la cual nos conectaremos
const char* ssid = "ssid";
const char* password = "password"; 
//Constante con la que comparar el tiempo de lecturas
#define REPORTING_PERIOD_MS     1000

//  Variables del sensor de frecuencia cardiaca y oxigeno
PulseOximeter pox;
uint32_t tsLastReport = 0;
float SFrecuencia;
float SOxigeno;

//Definimos los dos cores
TaskHandle_t Task1;
TaskHandle_t Task2;

//Configuración del proceso
void setup() {
  Serial.begin(115200);


 //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
                    Task1code,   /* Task function. */
                    "Task1",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                    

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
                    Task2code,   /* Task function. */
                    "Task2",     /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
}

//Task1code: Lee los datos del sensor de frecuencia cardíaca y oxígeno y los almacena en las variables
void Task1code( void * pvParameters ){
  
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  // Inicializar el sensor de frecuencia cardiaca y oxigeno
  Serial.print("Initializing pulse oximeter..");

  if (!pox.begin()) {
      Serial.println("FAILED");
      for(;;);
  } else {
      Serial.println("SUCCESS");
  }
   pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  for(;;){
   
  // Actualizar el sensor
  pox.update();
  //Comprobamos el tiempo para leer
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {    
      Serial.print("Frecuencia cardiaca:");
      Serial.print(pox.getHeartRate());
      Serial.print(" Oxigeno:");
      Serial.print(pox.getSpO2());
      
      //Asignamos los valores a las variables
      SFrecuencia = pox.getHeartRate();      
      SOxigeno = pox.getSpO2();
      Serial.println("%");

      tsLastReport = millis();
  }
  } 
}

//Task2code: Se encarga de realizar la conexión y enviar los datos de los sensores cuando son mayor de 0, los dos.
void Task2code( void * pvParameters ){
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

   //nos conectamos a la red
  WiFi.begin(ssid,password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    }
  Serial.print("");
  Serial.print("Conectado a la red con la IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  for(;;){
     if (WiFi.status() == WL_CONNECTED){ 
        HTTPClient http;   
        http.begin("http://192.168.4.1/Salud?frecuencia=" + String(SFrecuencia) + "&oxigeno=" + String(SOxigeno));
        http.addHeader("Content-Type","text/plain");
        //if (SFrecuencia > 0.00 && SFrecuencia < 30.00 && SOxigeno > 0 && SOxigeno < 60) {
        if (SFrecuencia > 0.00 && SOxigeno > 0) {
               int httpResponseCode = http.GET();
               if (httpResponseCode > 0) {
                  Serial.print("HTTP Response code: ");
                  Serial.println(httpResponseCode);
                  String payload = http.getString();
                  Serial.println(payload);
               }else {
                  Serial.print("Error code: ");
                  Serial.println(httpResponseCode);
               }
        http.end();    
      } else{
          Serial.println("Error in WIFI Connection");
      } 
    }
  }
}

//El loop no lo utilizamos 
void loop() {

}
                                  
