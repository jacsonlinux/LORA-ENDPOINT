#include <LoRa.h>
#include "DHT.h"
#include "esp_sleep.h"

DHT dht;

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 23
#define LORA_IRQ 26

#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  10 //minutes

float humidity;
float temperature;

void initLora() {
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_IRQ);
  if (!LoRa.begin(915E6)) {
    while (1);
  };
  LoRa.setSpreadingFactor(12);
}

void getDataSensor() {
  delay(dht.getMinimumSamplingPeriod());
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();
  while (isnan(temperature) || isnan(humidity)) {
    humidity = dht.getHumidity();
    temperature = dht.getTemperature();
  }
}

void sendData() {
  LoRa.beginPacket();
  LoRa.print(humidity);
  LoRa.print(temperature);
  LoRa.endPacket();
}

void setup() {
  Serial.begin(115200);
  while (!Serial);
  initLora();
  dht.setup(14);
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_sleep_pd_config(ESP_PD_DOMAIN_MAX, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
}

void loop() {
  getDataSensor();
  sendData();
  esp_deep_sleep_start();
}
