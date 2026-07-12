#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "tm1638.h"

#define STB_GPIO 21
#define CLK_GPIO 20
#define DIO_GPIO 19

static struct tm1638 tm;

int main() {
	uint8_t val = 0;

	stdio_init_all();

	tm1638_init(&tm, STB_GPIO, CLK_GPIO, DIO_GPIO);

	while (true) {
		val = tm1638_read_keys_one_byte(&tm);
		/* printf("val = %02X\n", val); */

		tm1638_set_segment(&tm, 0, (val<<8)|val);
		/* sleep_ms(1000); */
	}
	return 0;
}
