/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 5000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE        DT_ALIAS(led0)
#define LED1_NODE        DT_ALIAS(led1)
#define STACKSIZE        1024
#define THREAD0_PRIORITY 7
#define THREAD1_PRIORITY 7
/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec led_0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

int main(void)
{
	int ret;
	if (!gpio_is_ready_dt(&led_0)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_0, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}
	if (!gpio_is_ready_dt(&led_1)) {
		return 0;
	}

	ret = gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 0;
	}

	while (1) {
		ret = gpio_pin_toggle_dt(&led_0);
		if (ret < 0) {
			return 0;
		}
		k_msleep(SLEEP_TIME_MS);
		ret = gpio_pin_toggle_dt(&led_0);
		if (ret < 0) {
			return 0;
		}
		ret = gpio_pin_toggle_dt(&led_1);
		if (ret < 0) {
			return 0;
		}
		k_msleep(SLEEP_TIME_MS);
		ret = gpio_pin_toggle_dt(&led_1);
		if (ret < 0) {
			return 0;
		}
	}
}
// int thread0(void)
// {
// 	int ret;

// 	if (!gpio_is_ready_dt(&led_0)) {
// 		return 0;
// 	}

// 	ret = gpio_pin_configure_dt(&led_0, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
// 		return 0;
// 	}

// 	while (1) {
// 		k_msleep(SLEEP_TIME_MS);
// 		ret = gpio_pin_toggle_dt(&led_0);
// 		if (ret < 0) {
// 			return 0;
// 		}
// 	}
// 	return 0;
// }

// int thread1(void)
// {
// 	int ret;

// 	if (!gpio_is_ready_dt(&led_1)) {
// 		return 0;
// 	}

// 	ret = gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
// 		return 0;
// 	}

// 	while (1) {
// 		ret = gpio_pin_toggle_dt(&led_1);
// 		if (ret < 0) {
// 			return 0;
// 		}
// 		k_msleep(SLEEP_TIME_MS);
// 	}
// 	return 0;
// }

// K_THREAD_DEFINE(thread0_id, STACKSIZE, thread0, NULL, NULL, NULL, THREAD0_PRIORITY, 0, 0);
// K_THREAD_DEFINE(thread1_id, STACKSIZE, thread1, NULL, NULL, NULL, THREAD1_PRIORITY, 0, 0);
