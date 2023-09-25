/*
 * Copyright (c) 2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <nrfx_rtc.h>
#include <nrfx_dppi.h>
#include <nrfx_gpiote.h> // Need to figure out how I can connect this to

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 5000 // ms
#define TICKS_US NRFX_RTC_US_TO_TICKS(5000000, 1)

/*define pin number for led 1, could also do #define LED_NODE DT_ALIAS(led0), and then #define PIN_LED0 DT_GPIO_PIN(LED_NODE, gpios) I think*/
#define PIN_LED0 2

static const nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(1);

// NRFX_RTC_INT_COMPARE0 = 0, /**< Interrupt from COMPARE0 event. */
// NRFX_RTC_INT_COMPARE1 = 1, /**< Interrupt from COMPARE1 event. */
// NRFX_RTC_INT_COMPARE2 = 2, /**< Interrupt from COMPARE2 event. */
// NRFX_RTC_INT_COMPARE3 = 3, /**< Interrupt from COMPARE3 event. */
// NRFX_RTC_INT_TICK     = 4, /**< Interrupt from TICK event. */
// NRFX_RTC_INT_OVERFLOW = 5  /**< Interrupt from OVERFLOW event. */
void rtc_handler(nrfx_rtc_int_type_t evt)
{
	if (evt == 0)
	{
		printk("Timer\n");
		// toggle a light, but if I use this one, would I then use the CPU?
	}
	else if (evt == 1)
	{
		printk("comp1");
	}
	else if (evt == 2)
	{
		printk("comp2");
	}
	else if (evt == 3)
	{
		printk("comp3");
	}
	else if (evt == 4)
	{
		printk("interrupt from TICK event");
	}
	else if (evt == 5)
	{
		printk("interrupt from OVERFLOW event");
	}

	else
	{
		printk("something wierd is happening");
	}
}

int main(void)
{
	int err;

	printk("Start\n");
	printk("%d\n", TICKS_US);

	// configuring the RTC settings, just using the default
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
	err = nrfx_rtc_init(&rtc, &config, &rtc_handler);
	if (err != NRFX_SUCCESS)
	{
		printk("failed to init\n");
	}
	if (err == NRFX_SUCCESS)
	{
		printk("init succeded\n");
	}

	// initializing the rtc, need to define the handler

	// uint32_t ticks = calculate_ticks(3, 1);
	nrfx_rtc_counter_clear(&rtc);
	nrfx_rtc_enable(&rtc);

	// Enable the rtc, not sure if I was supposed to use the rtc or if I was supposed to just use a timer
	err = nrfx_rtc_cc_set(&rtc, 0, 100, false); // setting compare register 0
	if (err != NRFX_SUCCESS)
	{
		printk("cc set failed\n");
	}
	if (err == NRFX_SUCCESS)
	{
		printk("cc set succeded\n");
	}

	/*WHATS LEFT TO DO*/
	// 1. Configure a led light and turn it on - should I here use gpiote?
	// 1. connect to a dppi channel, so the event can trigger a task
	// 2. configure the task, which is that a led should toggle, connect this to the event (timer event)
	// Not sure wether I should start configuring the dppi channel or if I should start with just initializing the led and toggle the led
	//

	return 0;
}

// /* The devicetree node identifier for the "led0" alias. */
// #define LED0_NODE DT_ALIAS(led0)
// #define LED1_NODE DT_ALIAS(led1)
// #define STACKSIZE 1024
// #define THREAD0_PRIORITY 7
// #define THREAD1_PRIORITY 7
// /*
//  * A build error on this line means your board is unsupported.
//  * See the sample documentation for information on how to fix this.
//  */
// static const struct gpio_dt_spec led_0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
// static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);

// int main(void)
// {
// 	int ret;
// 	if (!gpio_is_ready_dt(&led_0)) {
// 		return 0;
// 	}

// 	ret = gpio_pin_configure_dt(&led_0, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
// 		return 0;
// 	}
// 	if (!gpio_is_ready_dt(&led_1)) {
// 		return 0;
// 	}

// 	ret = gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
// 	if (ret < 0) {
// 		return 0;
// 	}

// 	while (1) {
// 		ret = gpio_pin_toggle_dt(&led_0);
// 		if (ret < 0) {
// 			return 0;
// 		}
// 		k_msleep(SLEEP_TIME_MS);
// 		ret = gpio_pin_toggle_dt(&led_0);
// 		if (ret < 0) {
// 			return 0;
// 		}
// 		ret = gpio_pin_toggle_dt(&led_1);
// 		if (ret < 0) {
// 			return 0;
// 		}
// 		k_msleep(SLEEP_TIME_MS);
// 		ret = gpio_pin_toggle_dt(&led_1);
// 		if (ret < 0) {
// 			return 0;
// 		}
// 	}
// }
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
