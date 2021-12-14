#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_MLX90614.h> //temperatura
//Credenciales de la red a la cual nos conectaremos
const char* ssid = "ReinoCamelot";
const char* password = "Samsung+20";
//  Variables del sensor de temperatura
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
String STemperatura; 
//  Variables del sensor tilt
String SPosicion;
const int interruptor = 25;
const int led = 27;
//Inicialización de la conexión y los sensores
void setup() {
  Serial.begin(115200);
  //Serial.begin(9600);

   // Inicializar el sensor de temperatura
   mlx.begin();
   // Inicializar los pins del sensor tilt   
   pinMode(interruptor, INPUT);
   pinMode(led, OUTPUT);

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
//Bucle de lectura de los sensores
void loop() {
//Comprobamos el estado de la conexión, si está conectado enviamos datos 
if (WiFi.status() == WL_CONNECTED){
  HTTPClient http;  
  //Leemos el sensor de temperatura
  STemperatura = sensorTemperatura();
  Serial.print("Temperatura: " + STemperatura);
  //Leemos el sensor de posición
  SPosicion = sensorPosicion();
  Serial.print(" Posición: " + SPosicion);
  //Enviamos la información al servidor
  http.begin("http://192.168.4.1/Incorpora?posicion="+ SPosicion + "&temperatura=" + STemperatura);  
  //configuración de la cabecera del mensaje
  http.addHeader("Content-Type","text/plain");
  //Obtenemos la tespuesta del servidor
  int httpResponseCode = http.GET();
  //Comprobamos la respuesta del servidor
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
}else{
  Serial.println("Error in WIFI Connection");
} 
//Mostramos una línea en blanco y esperamos medio segundo para la siguiente lectura
Serial.println();
delay(500);
}             
//Función que devuelve el valor del sensor de temperatura
String sensorTemperatura() { 
  //Leemos el sensor de temperatura
  float Temp = mlx.readObjectTempC();
  //Devolvemos el valor en un string  
   return String(Temp);
}
//Función que devuelve el valor del sensor de incorporación
String sensorPosicion(){  
  //Devolvemos el valor del sensor
  if (digitalRead(interruptor) == 0) {
    digitalWrite(led,0);
    return "Acostado";  
  }else{   
    digitalWrite(led,1);
    return "Incorporado";
  }  
}  
                     
