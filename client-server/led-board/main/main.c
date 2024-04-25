#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"

#define LED1 22
#define LED2 21
#define LED3 19
#define LED4 18
#define LED5 17
#define LED6 16
#define LED7 2
#define LED8 5
#define LED9 4

static const uint8_t led_pins[] = {LED1, LED2, LED3, LED4, LED5,
                                   LED6, LED7, LED8, LED9};
static const uint8_t leds_len   = sizeof(led_pins) / sizeof(*led_pins);

#define INPUT1  39 // cs pin (VN)
#define INPUT2  36 // cs pin (VP)
#define INPUT3  35 // cs pin
#define INPUT4  34 // cs pin
#define INPUT5  33 // cs pin
#define INPUT6  32 // cs pin
#define INPUT7  27 // cs pin
#define INPUT8  26 // cs pin
#define INPUT9  25 // cs pin
#define INPUT10 23 // place pin

static const uint8_t input_pins[] = {INPUT1, INPUT2, INPUT3, INPUT4, INPUT5,
                                     INPUT6, INPUT7, INPUT8, INPUT9, INPUT10};
static const uint8_t input_len    = sizeof(input_pins) / sizeof(*input_pins);

void configure_output_leds(uint8_t state) {
  for (uint8_t i = 0; i < leds_len; ++i) {
    gpio_reset_pin(led_pins[i]);
    esp_rom_gpio_pad_select_gpio(led_pins[i]);
    gpio_set_direction(led_pins[i], GPIO_MODE_OUTPUT);
    gpio_set_level(led_pins[i], state);
  }
}

void set_output_leds(uint8_t state) {
  for (uint8_t i = 0; i < leds_len; ++i) { gpio_set_level(led_pins[i], state); }
}

void configure_input(void) {
  for (uint8_t i = 0; i < input_len; ++i) {
    gpio_reset_pin(input_pins[i]);
    esp_rom_gpio_pad_select_gpio(input_pins[i]);
    gpio_set_direction(input_pins[i], GPIO_MODE_INPUT);
    gpio_set_pull_mode(input_pins[i], GPIO_PULLDOWN_ONLY);
  }
}

void update_leds(char *state) {
  for (uint8_t i = 0; i < leds_len; ++i) {
    if (state[i] == 'X') {
      gpio_set_level(led_pins[i], 1); // Turn LED on solid
    } else if (state[i] == 'O') {
      // Blink LED slowly
      gpio_set_level(led_pins[i],
                     (xTaskGetTickCount() / pdMS_TO_TICKS(500)) % 2);
    } else {
      gpio_set_level(led_pins[i], 0); // Turn LED off
    }
  }
}

void app_main(void) {
  configure_output_leds(0);
  configure_input();

  char state[leds_len];
  for (uint8_t i = 0; i < leds_len; ++i) { state[i] = '\0'; }

  bool player1 = true;
  while (1) {
    // Check if the place pin is active
    if (gpio_get_level(INPUT10) == 1) {
      for (uint8_t i = 0; i < input_len - 1; ++i) {
        if (state[i] == '\0') {
          if (gpio_get_level(input_pins[i]) == 1) {
            state[i] = player1 ? 'X' : 'O';
            player1  = !player1;
            break;
          }
        }
      }
      vTaskDelay(pdMS_TO_TICKS(100)); // debounce delay
    }
    update_leds(state);
  }
}
