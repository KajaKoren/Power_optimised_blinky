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
#include <nrfx_timer.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 50000 // ms
#define TICKS_US NRFX_RTC_US_TO_TICKS(50000000000, 1)
#define RTC_IDX 0
#define LED_PIN_NUMBER 2

/*define pin number for led 1, could also do #define LED_NODE DT_ALIAS(led0), and then #define PIN_LED0 DT_GPIO_PIN(LED_NODE, gpios) I think*/
#define PIN_LED0 2
// static const nrfx_timer_t timer = NRFX_TIMER_INSTANCE(2);
static const nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(RTC_IDX);

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
		nrfx_rtc_counter_clear(&rtc);
	}
	else if (evt == 1)
	{
		printk("comp1\n");
	}
	else if (evt == 2)
	{
		printk("comp2\n");
	}
	else if (evt == 3)
	{
		printk("comp3\n");
	}
	else if (evt == 4)
	{
		printk("interrupt from TICK event\n");
	}
	else if (evt == 5)
	{
		printk("interrupt from OVERFLOW event\n");
	}

	else
	{
		printk("something wierd is happening\n");
	}
}

int main(void)
{
	int err;

	printk("Start\n");

	// configuring the RTC settings, just using the default
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
	// config.prescaler = 3276;

	err = nrfx_rtc_init(&rtc, &config, &rtc_handler);
	if (err != NRFX_SUCCESS)
	{
		printk("failed to init\n");
	}
	if (err == NRFX_SUCCESS)
	{
		printk("init succeded\n");
	}
#if defined(__ZEPHYR__)
	IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_RTC_INST_GET(RTC_IDX)), IRQ_PRIO_LOWEST,
					   NRFX_RTC_INST_HANDLER_GET(RTC_IDX), 0);
#endif
	// initializing the rtc, need to define the handler

	// uint32_t ticks = calculate_ticks(3, 1);
	nrfx_rtc_counter_clear(&rtc);
	nrfx_rtc_enable(&rtc);

	// Enable the rtc, not sure if I was supposed to use the rtc or if I was supposed to just use a timer
	err = nrfx_rtc_cc_set(&rtc, 0, TICKS_US, true); // setting compare register 0
	if (err != NRFX_SUCCESS)
	{
		printk("cc set failed\n");
	}
	if (err == NRFX_SUCCESS)
	{
		printk("cc set succeded\n");
	}
	while (1)
	{
		k_msleep(500);
		printk("counter value: %d \n", nrfx_rtc_counter_get(&rtc));
	}

	/*WHATS LEFT TO DO*/
	// 1. Configure a led light and turn it on - should I here use gpiote?
	// 1. connect to a dppi channel, so the event can trigger a task
	// 2. configure the task, which is that a led should toggle, connect this to the event (timer event)
	// Not sure wether I should start configuring the dppi channel or if I should start with just initializing the led and toggle the led
	//

	return 0;
}
