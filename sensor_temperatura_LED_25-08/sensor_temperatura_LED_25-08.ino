#include <math.h>
#include "AdafruitIO_WiFi.h"
#include "NewPing.h"

//Configurações Wifi e Adafruit
#define WIFI_SSID "..."
#define WIFI_PASS "..."

//Authenticação Adafruit IO
#define IO_USERNAME "..."
#define IO_KEY "..."

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

//Definição dos Feeds
AdafruitIO_Feed *botaoalarme = io.feed("botaoalarme");

// Variáveis de controle
bool alarmeAtivo = false;
unsigned int distancia = 0;
const int LIMITE_DISTANCIA = 15;


#define pinNTC 34  //pino ligado na 34

#define BUZZER_PIN 27
#define LED_ALARME 13
#define BOTAO_FISICO 26
#define TRIG_PIN 12
#define ECHO_PIN 14

//Configuração do ultrassonico
#define MAX_DISTANCE 100
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);


//Controle de envio de dados
float temp_atual = 0;
float temp_anterior = -1;

// variável / ponteiro para referenciar o feed temperatura
AdafruitIO_Feed *temperatura = io.feed("Temperatura");

#define pinNTC 34  //Pino ADC onde o NTC está conectado (no ESP32)
#define pinLed 14  //pino do LED

const float Rfixo = 10000.0;  //resistor do projeto
const float Beta = 3950.0;
const float R0 = 10000.0;        //nominal do sensor
const float T0_kelvin = 298.15;  // 25°c em kelvin
const float Vcc = 3.3;


void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_ALARME, OUTPUT);
  pinMode(BOTAO_FISICO, INPUT);


  delay(1000);

  Serial.begin(115200);
  pinMode(pinNTC, INPUT);  //configurando o pino como entrada
  pinMode(pinLed, OUTPUT);

  while (!Serial)
    ;

  Serial.print("Conectando ao Adafruit IO");
  io.connect();

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println(io.statusText());

  //Configuração do callbeck, quando o fedd receber(atualizar) um valor
  botaoalarme->onMessage(handleAlarme);

  Serial.println("Solicitando o estado unicial do alarme: ");
  botaoalarme->get();

  temperatura->onMessage(handleTemperatura);
  //registra a função de callback
  //ela será chamada sempre que o feed receber um novo dado
}

void loop() {
  io.run();

  // leitura do botão físico
  if (digitalRead(BOTAO_FISICO) == 1) {
    delay(300);  //debounce simples
    alarmeAtivo = !alarmeAtivo;

    botaoalarme->save(String(alarmeAtivo ? "true" : "false"));
    Serial.println(alarmeAtivo ? F("Alarme ARMADO pelo botao fisico") : F("Alarme DESARMADO pelo botao fisico"));
  }

  distancia = sonar.ping_cm();
  Serial.print("Distancia lida: ");
  Serial.println(distancia);
  Serial.println(" cm");

  //ativação ou desativação do alarme
  if(alarmeAtivo && distancia > 0 && distancia < LIMITE_DISTANCIA){
    ativarAlerta();
  }
  else{
    desligarAlerta();
  }
}