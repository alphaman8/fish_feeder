#include <ArduinoJson.h>
#include <Servo.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

 
#define SERVO D6 // Porta Digital 6 PWM
 
Servo s; // Variável Servo
int pos; // Posição Servo
int intervalo = 5;

int const bufferSize = 500;

unsigned long previousMillis = 0; 
String get_host = "http://cipweb.com.br"; 
const char* ssid = "developer";
const char* password = "pachamama";

int relay = D10; // relay turns trigger signal - active high;
 
void setup ()
{
  s.attach(SERVO);
  Serial.begin(9600);
  pinMode (relay, OUTPUT); // Define port attribute is output;
  s.write(0); // Inicia motor posição zero
  delay(100);
  beginWifi();
}
 
void loop()
{

  if (WiFi.status() == WL_CONNECTED) {  
    unsigned long currentMillis = millis();

  
    if (currentMillis - previousMillis >= 1000*intervalo) {
      previousMillis = currentMillis;
  //    lerAlimentacao();
  //    lerLigarBomba();
      verificarTudo();
    }
  }

}

void ligarBomba(boolean ligar) {
  if (ligar) {
    digitalWrite (relay, HIGH); // relay conduction;
    delay (100);
  } else {
    digitalWrite (relay, LOW); // relay conduction;
    delay (100);    
  }
}

void darRacao() {
//  for(pos = 0; pos < 180; pos++)
//  {
//    s.write(pos);
//    delay(5);
//  }
  for(pos = 180; pos >= 0; pos--)
  {
    s.write(pos);
    delay(10);
  }
}

void verificarTudo() {
  
        HTTPClient http;
        String url = get_host+"/Arduino/tanque/verificar_tudo.php";
        
        http.begin(url);
               
        //GET method
        int httpCode = http.GET();

        if (httpCode > 0) {
        
          String data = http.getString();
          http.end();

          const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
//          StaticJsonBuffer<bufferSize> jsonBuffer;
          DynamicJsonBuffer jsonBuffer(capacity);
          
          // Parse JSON
          int size = data.length() + 1;
        
          JsonObject& json_parsed = jsonBuffer.parseObject(data);

          if (!json_parsed.success())
          {
            Serial.println("parseObject() failed");
            return;
          }          
          
          const int ligar_bomba = json_parsed["tanque"][0]["ligar_bomba"];
          const int alimentar = json_parsed["tanque"][0]["alimentar"];
          intervalo = json_parsed["tanque"][0]["intervalo_verificacao"];
  
          if(alimentar==1)
          {
            darRacao();
    //          Serial.println("On");
    //          digitalWrite(ledPin, HIGH);
          }
          else
          {
    //          Serial.println("Off");
    //          digitalWrite(ledPin, LOW);
          }
          
  //        Serial.println(alimenta);        
          if(ligar_bomba==1)
          {
            ligarBomba(true);
//            Serial.print("Bomba: ");
//            Serial.println(ligar_bomba);
  //          digitalWrite(ledPin, HIGH);
          }
          else
          {
            ligarBomba(false);
//            Serial.print("Bomba: ");
//            Serial.println(ligar_bomba);
  //          digitalWrite(ledPin, LOW);
          }
        } else {
          Serial.print("HTTP Code: ");
          Serial.println(httpCode);
        }

        delay(100);        
}

void lerLigarBomba() {
  
        HTTPClient http;
        String url = get_host+"/Arduino/tanque/bomba_verificar.php";
        
        http.begin(url);
       
        //GET method
        int httpCode = http.GET();
        String ligar = http.getString();
//        Serial.println(alimenta);        
        if(ligar=="1")
        {
          ligarBomba(true);
//          Serial.println("On");
//          digitalWrite(ledPin, HIGH);
        }
        else
        {
          ligarBomba(false);
//          Serial.println("Off");
//          digitalWrite(ledPin, LOW);
        }
        http.end();
        delay(100);
}

void lerAlimentacao() {
  
        HTTPClient http;
        String url = get_host+"/Arduino/tanque/racao_verificar.php";
        
        http.begin(url);
       
        //GET method
        int httpCode = http.GET();
        String alimenta = http.getString();
//        Serial.println(alimenta);        
        if(alimenta=="1")
        {
          darRacao();
//          Serial.println("On");
//          digitalWrite(ledPin, HIGH);
        }
        else
        {
//          Serial.println("Off");
//          digitalWrite(ledPin, LOW);
        }
        http.end();
        delay(100);
}

void beginWifi() {
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

