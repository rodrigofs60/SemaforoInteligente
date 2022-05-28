/* Semaforo Inteligente
Grupo de Pi 5 Centro Universitario SENAC
Rodrigo Fernandes
Isabela Miranda
Lucas Morais
Gustavo dos Santos */

/*ESP32 Wifi bibliotecas*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 

/*Pinos usados HCSR04 e ESP32*/
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 

/*Pinos usados HCSR04 e ESP32*/
const int trigPin = 5;
const int echoPin = 18;

const int trigPin2 = 23;
const int echoPin2 = 19;

/*Pinos usados e variaveis LEDs e ESP32*/
int pinVermelho = 25;
int pinAmarelo = 27;
int pinVerde = 26;

int pinVermelho2 = 32;
int pinAmarelo2 = 12;
int pinVerde2 = 21;

int pinBotao = 33;

int contador = 0;
int contador2 = 0;
int totalContador;

#define botao 4
int estadoVelho = HIGH;
int estadoNovo;

unsigned long delay1 = 0;
unsigned long delay2 = 0;
unsigned long delay3 = 0;

/*Função para mudar a velocidade do semaforo*/
void tempoSemaforo (int verde, int amarelo, int vermelho);

/*IBM Watson IoT Platform informações*/
#define ORG "9e8of7" 
#define DEVICE_TYPE "ESP32-V1.0" 
#define DEVICE_ID "Semaforo-Inteligente-V1.0" 
#define TOKEN "n+bOVh?*95HUfc?&7!"

/*Velocidade do som em cm/uS*/
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

/*Variaveis HCSR04*/
long duration;
float distanceCm;

long duration2;
float distanceCm2;

/*Wifi login*/
const char* ssid = "MORAIS_2G";
const char* password = "@25070400#";

/*IBM Watson IoT Platform conexão*/
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic1[] = "iot-2/evt/teste1/fmt/json";
char pubTopic2[] = "iot-2/evt/status2/fmt/json";
char pubTopic3[] = "iot-2/evt/status3/fmt/json";
char pubTopic4[] = "iot-2/evt/status4/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void setup() {
  /*Wifi Connection ESP32 (61-74)*/
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("");
  }
  
  if (!client.connected()){
    Serial.println(server);
    while (!client.connect(clientId, authMethod, TOKEN)){
      Serial.print(".");
      delay(100);
    }
  }

  pinMode(botao, INPUT_PULLUP);

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input

  pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin2, INPUT); // Sets the echoPin as an Input

  pinMode(pinVerde, OUTPUT); //Pinos de LED como Output
  pinMode(pinAmarelo, OUTPUT);
  pinMode(pinVermelho, OUTPUT);

  pinMode(pinVerde2, OUTPUT); //Pinos de LED como Output
  pinMode(pinAmarelo2, OUTPUT);
  pinMode(pinVermelho2, OUTPUT);

  pinMode(pinBotao, OUTPUT);
}

long lastMsg = 0;

void loop(){
  //Limpo o trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  //Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  //Calcula a distância
  distanceCm = duration * SOUND_SPEED/2;

  //Limpo o trigPin
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  
  //Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);

  //Reads the echoPin, returns the sound wave travel time in microseconds
  duration2 = pulseIn(echoPin2, HIGH);
  
  //Calcula a distância
  distanceCm2 = duration2 * SOUND_SPEED/2;

  //Calculando contador
  if (distanceCm < 20){
    contador = contador + 1;
    delay(1000);
  }

  if (distanceCm2 < 20){
    contador2 = contador2 + 1;
    delay(1000);
  }

  int estadoBotao = digitalRead(botao);

  if (estadoBotao == 0){
    digitalWrite(pinBotao, HIGH);
  } else {
    digitalWrite(pinBotao, LOW);
  }

  if (!estadoBotao){
    if ((millis() - delay3) >= 1000){
      contador = contador - 1;
      delay3 = millis();
    }
  }
  
  Serial.println(contador);
  Serial.println(contador2);
  Serial.println("Estado do botao: ");
  Serial.println(estadoBotao);

  //Calculando o tipo de fluxo do semáforo
  totalContador = contador - contador2;

  Serial.println("Total Contador: ");
  Serial.println(totalContador);

  // Condição para dar prioridade ao semáforo 1
  if (totalContador > 0 && totalContador <= 2){
    tempoSemaforo(5000, 2000, 5000);
  }
  else if (totalContador > 2 && totalContador <= 4){
    tempoSemaforo(6000, 2000, 4000);
  }
  else if (totalContador > 4){
    tempoSemaforo(7000, 2000, 3000);

  // Condição para dar prioridade ao semáforo 2
  }
  else if (totalContador <= 0 && totalContador >= -2){
    tempoSemaforo(7000, 2000, 3000);
  }
  else if (totalContador < -2 && totalContador >= -4){
    tempoSemaforo(4000, 2000, 6000);
  }
  else if (totalContador < -4){
    tempoSemaforo(3000, 2000, 7000);
  }

  client.loop();
  long now = millis();

  //Enviando as informações para a plataforma IBM (131-158)
  if (now - lastMsg > 1000){
    lastMsg = now;
    float distancia = distanceCm;
    String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload += ",\"distancia\":";
           payload += distancia;
           payload += "}}";
    
    Serial.print("Sending payload: ");
    Serial.println(payload);

    if (client.publish(pubTopic1, (char*) payload.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }
    
    String payload1 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload1 += ",\"contador\":";
           payload1 += contador;
           payload1 += "}}";
       
    if (client.publish(pubTopic2, (char*) payload1.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }

    String payload2 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload2 += ",\"contador2\":";
           payload2 += contador2;
           payload2 += "}}";
       
    if (client.publish(pubTopic3, (char*) payload2.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }

    String payload3 = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
           payload3 += ",\"distancia2\":";
           payload3 += distanceCm2;
           payload3 += "}}";
       
    if (client.publish(pubTopic4, (char*) payload3.c_str())){
      Serial.println("Publish ok");
    }else{
      Serial.println("Publish failed");
    }
  }
}

//Função que define o tipo de fluxo do semáforo
void tempoSemaforo (int verde, int amarelo, int vermelho) {
  if ((millis() - delay1) <= verde){
    digitalWrite(pinVerde, HIGH);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, LOW);

    digitalWrite(pinVerde2, LOW);
    digitalWrite(pinAmarelo2, LOW);
    digitalWrite(pinVermelho2, HIGH);
  }

  else if ((millis() - delay1) > verde && (millis() - delay1) <= (verde + amarelo)){
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, HIGH);
    digitalWrite(pinVermelho, LOW);

    digitalWrite(pinVerde2, LOW);
    digitalWrite(pinAmarelo2, LOW);
    digitalWrite(pinVermelho2, HIGH);
  }

  else if ((millis() - delay1) > (verde + amarelo) && (millis() - delay1) <= (verde + amarelo + vermelho)){
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, HIGH);

    digitalWrite(pinVerde2, HIGH);
    digitalWrite(pinAmarelo2, LOW);
    digitalWrite(pinVermelho2, LOW);
  }

  else if ((millis() - delay1) > (verde + amarelo + vermelho) && (millis() - delay1) <= (verde + amarelo + amarelo + vermelho)){
    digitalWrite(pinVerde, LOW);
    digitalWrite(pinAmarelo, LOW);
    digitalWrite(pinVermelho, HIGH);

    digitalWrite(pinVerde2, LOW);
    digitalWrite(pinAmarelo2, HIGH);
    digitalWrite(pinVermelho2, LOW);
  }

  else{
    delay1 = millis();
  }
}

/*

Código controlando os 2 semáforos e utilizando os 2 sensores.
Ambos possuem a lógica do Semáforo Inteligente.
O tempo total do ciclo do semáforo é de 2 segundos a mais (incluso no sinal vermelho), devido a condição "if" a mais na função, por conta do sinal amarelo.
Um botão foi adicionado e está funcionando. A sua lógica vai subtrair um valor X do contador. Será subtraido do CONTADOR pois o pedestre atravessará a rua em que este sensor irá contar a quantidade de carros.
OBS.: TODAS as condições dos "ifs" dos semáforos foram testadas e estão funcionando perfeitamente.

 */
