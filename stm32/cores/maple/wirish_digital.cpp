/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

/*
 * Arduino-compatible digital I/O implementation.
 */

#include "wirish.h"
#include "io.h"

void pinMode(uint8 pin, WiringPinMode mode) {
    gpio_pin_mode outputMode;
    boolean pwm = false;

    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }

    switch(mode) {
    case OUTPUT:
        outputMode = GPIO_OUTPUT_PP;
        break;
    case OUTPUT_OPEN_DRAIN:
        outputMode = GPIO_OUTPUT_OD;
        break;
    case INPUT:
    case INPUT_FLOATING:
        outputMode = GPIO_INPUT_FLOATING;
        break;
    case INPUT_ANALOG:
        outputMode = GPIO_INPUT_ANALOG;
        break;
    case INPUT_PULLUP:
        outputMode = GPIO_INPUT_PU;
        break;
    case INPUT_PULLDOWN:
        outputMode = GPIO_INPUT_PD;
        break;
    case PWM:
        outputMode = GPIO_AF_OUTPUT_PP;
        pwm = true;
        break;
    case PWM_OPEN_DRAIN:
        outputMode = GPIO_AF_OUTPUT_OD;
        pwm = true;
        break;
    default:
        ASSERT(0);
        return;
    }

    const stm32_pin_info *info = &PIN_MAP[pin];

    if (pwm)
    {
      if (info->timer_device != NULL) {
        int afnum = timer_get_af(info->timer_device);
        timer_set_mode(info->timer_device, info->timer_channel, TIMER_PWM);
        gpio_set_af_mode(info->gpio_device, info->gpio_bit, afnum);

        /* Enable/disable timer channels if we're switching into or * out of PWM. */
//        timer_set_mode(info->timer_device, info->timer_channel, pwm ? TIMER_PWM : TIMER_DISABLED);
      }
    }

    gpio_set_mode(info->gpio_device, info->gpio_bit, outputMode);
}

char const* pinName(uint8_t pin)
{
  static char name[5];
  memclr(name, sizeof(name));

  name[0] = 'P';
  name[1] = (pin >> 4) + 'A';

  uint8_t bit = (pin & 0x0f);
  if ( bit > 10)
  {
    name[2] = '0' + bit/10;
    name[3] = '0' + bit%10;
  }else
  {
    name[2] = '0' + bit%10;
  }

  return name;
}

uint32 digitalRead(uint8 pin) {
    if (pin >= BOARD_NR_GPIO_PINS) {
        return 0;
    }

    return gpio_read_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit) ?
        HIGH : LOW;
}

void digitalWrite(uint8 pin, uint8 val) {
    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }

    gpio_write_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit, val);
}

void togglePin(uint8 pin) {
    if (pin >= BOARD_NR_GPIO_PINS) {
        return;
    }

    gpio_toggle_bit(PIN_MAP[pin].gpio_device, PIN_MAP[pin].gpio_bit);
}

#define BUTTON_DEBOUNCE_DELAY 1

uint8 isButtonPressed() {
    if (digitalRead(BOARD_BUTTON_PIN)) {
        delay(BUTTON_DEBOUNCE_DELAY);
        while (digitalRead(BOARD_BUTTON_PIN))
            ;
        return true;
    }
    return false;
}

uint8 waitForButtonPress(uint32 timeout) {
    uint32 start = millis();
    uint32 time;
    if (timeout == 0) {
        while (!isButtonPressed())
            ;
        return true;
    }
    do {
        time = millis();
        /* properly handle wrap-around */
        if ((start > time && time + (0xffffffffU - start) > timeout) ||
            time - start > timeout) {
            return false;
        }
    } while (!isButtonPressed());
    return true;
}
