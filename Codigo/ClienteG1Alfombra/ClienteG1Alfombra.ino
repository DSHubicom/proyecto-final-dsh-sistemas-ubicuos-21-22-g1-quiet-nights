#include <WiFi.h>
#include <HTTPClient.h>

//Credenciales de la red a la cual nos conectaremos
const char* ssid = "ReinoCamelot";
const char* password = "Samsung+20";

//Url para hacer las peticiones
const char* esp32Server = "http://192.168.4.2/Alfombra";

//Variables 
String sensorValue; 
String answer;

void setup() {
  Serial.begin(115200);


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
}

void loop() {
if (WiFi.status() == WL_CONNECTED){

  HTTPClient http;  
   sensorValue = analogRead(A0); // read analog input pin 0
   Serial.println("Datos Alfombra Cliente: " + sensorValue);
  http.begin("http://192.168.4.1/Alfombra?presion=" + sensorValue);
  http.addHeader("Content-Type","text/plain");

  int httpResponseCode = http.GET();

 
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
  }
  else {
   Serial.print("Error code: ");
        Serial.println(httpResponseCode);
  }

  http.end();
}
else
{Serial.println("Error in WIFI Connection");
  }
 delay(1000);  

}                                  


String getRequest(const char* serverName) {
  HTTPClient http;
  http.begin(serverName);

  //Enviamos petición HTTP
  int httpResponseCode = http.GET();
  String payload = "...";

  
  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode); 
  }
    //liberamos
    http.end();

    return payload;
  }
