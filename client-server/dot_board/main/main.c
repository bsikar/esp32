#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include <stdint.h>

/* TODO!

  On the msp you will set the CS pins
  You will activate the CS pin you want
  Then you will send the PIN_NUM_PLACE as HIGH

*/

#define PIN_NUM_CLK   15
#define PIN_NUM_DIN   2
#define PIN_NUM_PLACE 5

void sendByte(uint8_t reg, uint8_t data) {
  uint16_t word = (reg << 8) | data;
  for (int i = 15; i >= 0; i--) {
    gpio_set_level(PIN_NUM_CLK, 0);
    gpio_set_level(PIN_NUM_DIN, (word >> i) & 0x01);
    gpio_set_level(PIN_NUM_CLK, 1);
  }
}

void sendToMax(uint8_t reg, uint8_t data) { sendByte(reg, data); }

void setupMax7219() {
  gpio_set_direction(PIN_NUM_CLK, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_DIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(PIN_NUM_PLACE, GPIO_MODE_INPUT);

  sendToMax(0x0C, 0x01); // Normal operation mode
  sendToMax(0x0B, 0x07); // Scan limit = 7 (All rows, cs)
  sendToMax(0x09, 0x00); // No decode mode
  sendToMax(0x0A, 0x08); // Intensity control
  sendToMax(0x0F, 0x00); // Display test off
}

void displayCursor() {
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
void displayX() {
  // Clearing the display
  for (int i = 1; i <= 8; i++) { sendToMax(i, 0x00); }

  // Setting columns for "O"
  sendToMax(1, 0b00000000);
  sendToMax(2, 0b01000010);
  sendToMax(3, 0b00100100);
  sendToMax(4, 0b00011000);
  sendToMax(5, 0b00011000);
  sendToMax(6, 0b00100100);
  sendToMax(7, 0b01000010);
  sendToMax(8, 0b00000000);
}

void displayO() {
  // Clearing the display
  for (int i = 1; i <= 8; i++) { sendToMax(i, 0x00); }

  // Setting columns for "O"
  sendToMax(1, 0b00000000);
  sendToMax(2, 0b00111100);
  sendToMax(3, 0b01100110);
  sendToMax(4, 0b01000010);
  sendToMax(5, 0b01000010);
  sendToMax(6, 0b01100110);
  sendToMax(7, 0b00111100);
  sendToMax(8, 0b00000000);
}

void app_main(void) { // this works to flip between the displays
  gpio_set_direction(PIN_NUM_PLACE, GPIO_MODE_INPUT);
  setupMax7219();
  bool playerOne = true;
  while (1) {
    displayCursor();

    if (gpio_get_level(PIN_NUM_PLACE) == 0) { // this is low enabled
      if (playerOne) {
        displayX();
        playerOne = false;
      } else {
        displayO();
        playerOne = true;
      }
    }
  }
}
