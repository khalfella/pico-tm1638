#ifndef __TM1638_H_
#define __TM1638_H_

#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

/*
 * TM1638 controls a gird of 10 7-segment diaplay and 24 buttons.
 * The module is flexible such that it can be wired to control
 * less thatn 10 segments + LEDs. The module I am using LED&KEY
 * uses 8 7-segment units + 8 LED + 8 Keys. This driver is written
 * for this evaluation board. Feel free to customize it for your
 * use case.
 *
 * Link on aliexpress: https://www.aliexpress.us/item/3256806630226234.html
 * Link om Amazon: https://www.amazon.com/dp/B01D140BT6
 *
 *       -LED
 *
 *    --- A ---
 *   |         |
 *   F         B
 *   |         |
 *    --- G ---
 *   |         |
 *   E         C
 *   |         |
 *    --- D ---   -dp
 *
 *
 * TM1638 has 16 bytes of memory to control which GRID segment is
 * enabled. The evaluation board I use has 8 grids and 8 LEDs.
 * The 16 bytes of memory is mapped such that.
 *
 *              BYTE0              BYTE1
 * 0x0000 GRID0 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED0}
 * 0x0002 GRID1 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED1}
 * 0x0004 GRID2 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED2}
 * 0x0006 GRID3 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED3}
 * 0x0008 GRID4 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED4}
 * 0x000a GRID5 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED5}
 * 0x000c GRID6 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED6}
 * 0x000e GRID7 {A,B,C,D,E,F,G,dp} {N,N,N,N,N,N,N,LED7}
 *
 *
 *              KS1     KS2     KS3     KS4     KS5     KS6     KS7     KS8
 *               |       |       |       |       |       |       |       |
 *  K1 ---------[S1]----[S2]----[S3]----[S4]----[S5]----[S6]----[S7]----[S8]
 *               |       |       |       |       |       |       |       |
 *  K2 ---------[S9]---[S10]---[S11]---[S12]---[S13]---[S14]---[S15]---[S16]
 *               |       |       |       |       |       |       |       |
 *  K3 --------[S17]---[S18]---[S19]---[S20]---[S21]---[S22]---[S23]---[S24]
 *
 *  24 switches are scanned as showed above.
 *
 *          +----+----+----+----+----+----+----+----+
 *          | B7 | B6 | B5 | B4 | B3 | B2 | B1 | B0 |
 *  --------+----+----+----+----+----+----+----+----+
 *  |BYTE0  | X  | K1 | K2 | K3 | X  | K1 | K2 | K3 |
 *  |       |   ---- KS2 ----   |   ---- KS1 ----   |
 *  --------+----+----+----+----+----+----+----+----+
 *  |BYTE1  | X  | K1 | K2 | K3 | X  | K1 | K2 | K3 |
 *  |       |   ---- KS4 ----   |   --- KS3 ----    |
 *  --------+----+----+----+----+----+----+----+----+
 *  |BYTE2  | X  | K1 | K2 | K3 | X  | K1 | K2 | K3 |
 *  |       |   ---- KS6 ----   |   ---- KS5 ----   |
 *  --------+----+----+----+----+----+----+----+----+
 *  |BYTE3  | X  | K1 | K2 | K3 | X  | K1 | K2 | K3 |
 *  |       |   ---- KS8 ----   |   ---- KS7 ----   |
 *  --------+----+----+----+----+----+----+----+----+
 *
 * The output of key scanning is 4 bytes telling us which buttons are pressed.
 * In this evaluation board the 8 buttons are connected to K3.
 */

/* Instruction description */
#define	TM1638_DATA_INST		0b01000000	/* Read/Write data */
#define	TM1638_DISP_INST		0b10000000	/* Display on/off  */
#define	TM1638_ADDR_INST		0b11000000	/* Sets write addr */

/* Data instructions */
#define TM1638_DATA_INST_WRITE		0b00000000	/* Write data      */
#define TM1638_DATA_INST_READ		0b00000010	/* Read data       */
#define TM1638_DATA_INST_ADDR_ADD	0b00000000	/* Auto incr. addr */
#define TM1638_DATA_INST_ADDR_FIX	0b00000100	/* Fixed addr      */
#define TM1638_DATA_INST_NORM_MODE	0b00000000	/* Normal mode     */
#define TM1638_DATA_INST_TEST_MODE	0b00001000      /* Test mode?      */

/* Display instructions */
#define TM1638_DISP_INT_ON		0b00001000	/* Set display on  */
#define TM1638_DISP_INT_OFF		0b00000000	/* Set display off */

#define TM1638_DISP_INST_PWM_01_16	0b00000000	/* Set PWM 01/16   */
#define TM1638_DISP_INST_PWM_02_16	0b00000001	/* Set PWM 02/16   */
#define TM1638_DISP_INST_PWM_04_16	0b00000010	/* Set PWM 04/16   */
#define TM1638_DISP_INST_PWM_10_16	0b00000011	/* Set PWM 10/16   */
#define TM1638_DISP_INST_PWM_11_16	0b00000100	/* Set PWM 11/16   */
#define TM1638_DISP_INST_PWM_12_16	0b00000101	/* Set PWM 12/16   */
#define TM1638_DISP_INST_PWM_13_16	0b00000110	/* Set PWM 13/16   */
#define TM1638_DISP_INST_PWM_14_16	0b00000111	/* Set PWM 14/16   */

/* Commands */
#define TM1638_CMD_ON		\
	(TM1638_DISP_INST|TM1638_DISP_INT_ON|TM1638_DISP_INST_PWM_14_16)

#define TM1638_CMD_WRITE	(TM1638_DATA_INST|TM1638_DATA_INST_WRITE)
#define TM1638_CMD_READ		(TM1638_DATA_INST|TM1638_DATA_INST_READ)


struct tm1638 {
	int	stb_gpio;
	int	clk_gpio;
	int	dio_gpio;
};

/*
 * Output 8bit vaule. Each bit takes one clock cycle
 */
static void tm1638_shift_out_byte(struct tm1638 *tm, uint8_t val)
{
	for (int i = 0; i < 8; i++, val >>= 1) {
		gpio_put(tm->clk_gpio, false);
		gpio_put(tm->dio_gpio, val & 0x01);
		sleep_us(1);
		gpio_put(tm->clk_gpio, true);
		sleep_us(1);
	}
}

/*
 * Input 8bit value. Each bit takes one clock cycle
 */
static uint8_t tm1638_shift_in_byte(struct tm1638 *tm)
{
	uint8_t byte = 0;
	for (int i = 0; i < 8; i++) {
		gpio_put(tm->clk_gpio, false);
		sleep_us(1);
		byte |= (gpio_get(tm->dio_gpio) ? 1 : 0) << i;
		gpio_put(tm->clk_gpio, true);
		sleep_us(1);
	}
	return byte;
}

static void tm1638_send_cmd(struct tm1638 *tm, uint8_t cmd)
{
	gpio_put(tm->stb_gpio, false);
	sleep_us(1);
	tm1638_shift_out_byte(tm, cmd);
	gpio_put(tm->stb_gpio, true);
	sleep_us(1);
}

static void tm1638_set_segment(struct tm1638 *tm, int seg, uint16_t val)
{
	uint8_t offset;

	gpio_put(tm->stb_gpio, false);
	sleep_us(1);

	/* Set offset to specified segment */
	offset = TM1638_ADDR_INST | (seg << 1);
	tm1638_shift_out_byte(tm, offset);

	tm1638_shift_out_byte(tm, (val>>0) & 0xff);
	tm1638_shift_out_byte(tm, (val>>8) & 0xff);
	gpio_put(tm->stb_gpio, true);
	sleep_us(1);
}

static void tm1638_set_segments(struct tm1638 *tm, uint16_t *val)
{
	gpio_put(tm->stb_gpio, false);
	sleep_us(1);

	/* Set offset to first segment */
	tm1638_shift_out_byte(tm, TM1638_ADDR_INST);
	for (int i = 0; i < 8; i++) {
		tm1638_shift_out_byte(tm, (val[i]>>0) & 0xff);
		tm1638_shift_out_byte(tm, (val[i]>>8) & 0xff);
	}
	gpio_put(tm->stb_gpio, true);
	sleep_us(1);
}

static void tm1638_read_keys_raw(struct tm1638 *tm, uint8_t *keys)
{
	gpio_put(tm->stb_gpio, false);
	sleep_us(1);
	tm1638_shift_out_byte(tm, TM1638_CMD_READ);

	/* switch to input */
	gpio_set_dir(tm->dio_gpio, GPIO_IN);

	for (int i = 0; i < 4; i++)
		keys[i] = tm1638_shift_in_byte(tm);

	gpio_put(tm->stb_gpio, true);
	sleep_us(1);
	/* switch back to output */
	gpio_set_dir(tm->dio_gpio, GPIO_OUT);
}

/*
 * Converts 4 bytes keys read from TM1638 into one bytes such
 * that every bit maps to one key.
 */
static uint8_t tm1638_keys_raw_to_one_byte(uint8_t *keys)
{
	uint8_t val = 0;
	int i, bit0, bit4;

	for (i = 0; i < 4; i++) {
		bit0 = !!(keys[i] & 0x01);
		bit4 = !!(keys[i] & 0x10);
		val |= (bit4 << (i + 4)) | (bit0 << (i));
	}
	return val;
}

static uint8_t tm1638_read_keys_one_byte(struct tm1638 *tm)
{
	uint8_t keys[4], val = 0;

	tm1638_read_keys_raw(tm, keys);
	return tm1638_keys_raw_to_one_byte(keys);
}

static void tm1638_init(struct tm1638 *tm, int stb, int clk, int dio)
{
	/*
	* Initiatlize all 3 GPIO lines as output and establish
	* resting idle state
	*/

	uint16_t segments[8] = {};

	tm->stb_gpio = stb;
	tm->clk_gpio = clk;
	tm->dio_gpio = dio;

	gpio_init(tm->stb_gpio);
	gpio_init(tm->clk_gpio);
	gpio_init(tm->dio_gpio);

	gpio_set_dir(tm->stb_gpio, GPIO_OUT);
	gpio_set_dir(tm->clk_gpio, GPIO_OUT);
	gpio_set_dir(tm->dio_gpio, GPIO_OUT);

	gpio_put(tm->stb_gpio, true);
	gpio_put(tm->clk_gpio, true);

	/* Give the chip time to boot up */
	sleep_ms(10);

	tm1638_send_cmd(tm, TM1638_CMD_WRITE);
	tm1638_set_segments(tm, segments);
	tm1638_send_cmd(tm, TM1638_CMD_ON);
}

#endif /* __TM1638_H_ */
