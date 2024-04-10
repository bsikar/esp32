#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define PIN_NUM_CLK 18
#define PIN_NUM_CS  5
#define PIN_NUM_DIN 23

void sendByte(uint8_t reg, uint8_t data) {
  uint16_t word = (reg << 8) | data;
  gpio_set_level(PIN_NUM_CS, 0);
  for (int i = 15; i >= 0; i--) {
    gpio_set_level(PIN_NUM_CLK, 0);
    gpio_set_level(PIN_NUM_DIN, (word >> i) & 0x01);
    gpio_set_level(PIN_NUM_CLK, 1);
  }
  gpio_set_level(PIN_NUM_CS, 1);
}

void sendToMax(uint8_t reg, uint8_t data) { sendByte(reg, data); }

void setupMax7219() {
  gpio_set_direction(PIN_NUM_CLK, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_CS, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_DIN, GPIO_MODE_OUTPUT);

  sendToMax(0x0C, 0x01); // Normal operation mode
  sendToMax(0x0B, 0x07); // Scan limit = 7 (All rows)
  sendToMax(0x09, 0x00); // No decode mode
  sendToMax(0x0A, 0x08); // Intensity control
  sendToMax(0x0F, 0x00); // Display test off
}

void displayO() {
  // Clearing the display
  for (int i = 1; i <= 8; i++) { sendToMax(i, ~0x00); }

  // Setting columns for "O"
  sendToMax(2, ~0b01111110);
  sendToMax(3, ~0b01111110);
  sendToMax(4, ~0b01111110);
  sendToMax(5, ~0b01111110);
  sendToMax(6, ~0b01111110);
  sendToMax(7, ~0b01111110);
}

void app_main(void) {
  setupMax7219();
  while (1) {
    displayO();
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Update display every second
  }
}

