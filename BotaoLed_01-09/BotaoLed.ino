#include "AdafruitIO_WiFi.h"

// ===== CONFIG REDE =====
#define WIFI_SSID     ""
#define WIFI_PASS     ""

// ===== CONFIG ADAFRUIT IO =====
#define IO_USERNAME   ""
#define IO_KEY        ""

// Cria conexão com Adafruit IO
AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// Feed "botaoLed"
AdafruitIO_Feed *botaoLed = io.feed("botaoled");

// ===== PINO DO LED =====
#define pinLed 14   // escolha um pino digital (ex: 14, 23 ou 25)

// ===== SETUP =====
void setup() {
  Serial.begin(115200);
  pinMode(pinLed, OUTPUT);
  digitalWrite(pinLed, LOW);

  // Aguarda inicializar o Serial
  while (!Serial);

  Serial.print("Conectando ao Adafruit IO");
  io.connect();

  // Configura callback do feed
  botaoLed->onMessage(handleBotaoLed);

  // Espera conexão
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // Mostra status quando conecta
  Serial.println();
  Serial.println(io.statusText());

  // Busca o último valor do feed
  botaoLed->get();
}

// ===== LOOP =====
void loop() {
  io.run();  // mantém a conexão ativa
}

// ===== CALLBACK =====
void handleBotaoLed(AdafruitIO_Data *data) {
  Serial.print("Novo valor no feed botaoLed: ");
  Serial.println(data->toString());

  if (data->toBool()) {
    digitalWrite(pinLed, HIGH);
    Serial.println("LED LIGADO");
  } else {
    digitalWrite(pinLed, LOW);
    Serial.println("LED DESLIGADO");
  }
}

