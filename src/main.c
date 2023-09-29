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
#include <helpers/nrfx_gppi.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 50000 // ms
#define TICKS_US NRFX_RTC_US_TO_TICKS(50000000000, 1)
#define RTC_IDX 0
/*define pin number for led 1, could also do #define LED_NODE DT_ALIAS(led0), and then #define PIN_LED0 DT_GPIO_PIN(LED_NODE, gpios), but uneccessary steps*/
#define LED_PIN_NUMBER 2
#define DPPI_CHANNEL 7
#define DPPI_CHANNEL_2 6
#define GPIOTE_CHANNEL 3
#define INTR_PRIORITY 6
#define COMPARE_CHANNEL 0
#define GPIOTE_CHANNEL_2 2

static const nrfx_rtc_t rtc = NRFX_RTC_INSTANCE(RTC_IDX);
uint8_t dppi_channel = DPPI_CHANNEL;
uint8_t dppi_channel_2 = DPPI_CHANNEL_2;
uint8_t gpiote_channel = GPIOTE_CHANNEL;
uint8_t gpiote_channel_2 = GPIOTE_CHANNEL_2;

static void csn_event_handler(nrfx_gpiote_pin_t pin,
							  nrfx_gpiote_trigger_t trigger,
							  void *p_context)
{
	(void)pin;
	(void)trigger;
	(void)p_context;
}
void rtc_handler(nrfx_rtc_int_type_t evt)
{
	// This can just be left empty, it's not being used in this sample
	if (evt == 0)
	{
		printk("Timer\n");
		// toggle a light, but if I use this one, would I then use the CPU?
		nrf_gpio_pin_toggle(LED_PIN_NUMBER);
		nrfx_rtc_counter_clear(&rtc);
	}
	else if (evt == 1)
	{
		printk("comp1\n");
		nrf_gpio_pin_toggle(LED_PIN_NUMBER);
		nrfx_rtc_counter_clear(&rtc);
	}
	else if (evt == 2)
	{
		printk("comp2\n");
		nrf_gpio_pin_toggle(LED_PIN_NUMBER);
		nrfx_rtc_counter_clear(&rtc);
	}
	else if (evt == 3)
	{
		printk("comp1\n");
		nrf_gpio_pin_toggle(LED_PIN_NUMBER);
		nrfx_rtc_counter_clear(&rtc);
	}

	else
	{
		printk("something wierd is happening\n");
	}
}

void led_init(void)
{
	nrf_gpio_cfg_output(LED_PIN_NUMBER);
	nrf_gpio_pin_clear(LED_PIN_NUMBER); // Initially turn the LED off
}

void conf_gpiote(void)
{
	nrfx_err_t res;

	res = nrfx_gpiote_channel_alloc(&gpiote_channel); // allocates GPIOTE channel, not 100% sure if i need this
	// initializing the driver
	if (res != NRFX_SUCCESS)
	{
		printk("There is no avaiable channels - gpiote \n");
	}
	res = nrfx_gpiote_channel_alloc(&gpiote_channel_2); // allocates GPIOTE channel
	if (res != NRFX_SUCCESS)
	{
		printk("There is no avaiable channels - gpiote \n");
	}
	if (!nrfx_gpiote_is_init())
	{
		res = nrfx_gpiote_init((uint8_t)INTR_PRIORITY);
		if (res != NRFX_SUCCESS)
		{
			printk("The driver was already initalized, which should not happen, since we check before initializing- gpiote\n ");
		}
	}

	// Configuration for task
	nrfx_gpiote_task_config_t config_gpiote_task;
	config_gpiote_task.init_val = GPIOTE_CONFIG_OUTINIT_Low;	 // We start with the led being off.
	config_gpiote_task.polarity = GPIOTE_CONFIG_POLARITY_Toggle; // We want to toggle the led, so I assume this is correct
	config_gpiote_task.task_ch = gpiote_channel;				 // setting the task channel to be 4 - I assume that we also could have made a macro for this

	// configuration for output pin
	nrfx_gpiote_output_config_t config_gpiote = NRFX_GPIOTE_DEFAULT_INPUT_CONFIG;

	res = nrfx_gpiote_output_configure((nrfx_gpiote_pin_t)LED_PIN_NUMBER, &config_gpiote, &config_gpiote_task); // (nrfx_gpiote_pin_t) this is used to make sure
	// to treat the value of LED_PIN_NUMBER as a nrfx_gpio_pin_t type (wich esentially just is a 32-bit unsigned integer)

	// Configuration of event
	nrfx_gpiote_trigger_config_t config_gpiote_event;
	config_gpiote_event.p_in_channel = &gpiote_channel_2;
	config_gpiote_event.trigger = NRFX_GPIOTE_TRIGGER_TOGGLE;

	// Config input event
	nrfx_gpiote_input_config_t config_gpiote_input;
	nrfx_gpiote_handler_config_t handler_config = {
		.handler = csn_event_handler};

	// Configuration for the input pin
	// nrfx_gpiote_input_configure((nrfx_gpiote_pin_t)LED_PIN_NUMBER, &config_gpiote_input, &config_gpiote_event, &handler_config);

	if (res != NRFX_SUCCESS)
	{
		printk("error configuring the gpiote event");
	}

	nrfx_gpiote_out_task_enable((nrfx_gpiote_pin_t)LED_PIN_NUMBER);
	// nrfx_gpiote_trigger_enable((nrfx_gpiote_pin_t)LED_PIN_NUMBER, false);
}

void conf_dppi(void)
{
	nrfx_err_t res;
	nrfx_dppi_free();

	// Allocate first channel for triggering the LED to toggle on signal from clock
	res = nrfx_dppi_channel_alloc(&dppi_channel);
	if (res != NRFX_SUCCESS)
	{
		printk("There is no avaiable channels - dppi \n");
	}

	// Allocate second channel for resetting clock eack time the LED is toogled
	res = nrfx_dppi_channel_alloc(&dppi_channel_2);
	if (res != NRFX_SUCCESS)
	{
		printk("There is no avaiable channels - dppi \n");
	}

	// Enable the first channel
	res = nrfx_dppi_channel_enable(dppi_channel);
	if (res != NRFX_SUCCESS)
	{
		printk("The spesific channel is not allocated - dppi \n");
	}

	// Enable second channel
	res = nrfx_dppi_channel_enable(dppi_channel_2);
	if (res != NRFX_SUCCESS)
	{
		printk("The spesific channel is not allocated - dppi \n");
	}

	// setting the first dppi channel - where led is toggled from rtc clock
	uint32_t evt_addr = nrfx_rtc_event_address_get(&rtc, NRF_RTC_EVENT_COMPARE_0);
	uint32_t tsk_addr = nrfx_gpiote_out_task_address_get((nrfx_gpiote_pin_t)LED_PIN_NUMBER);
	nrfx_gppi_channel_endpoints_setup(dppi_channel, evt_addr, tsk_addr);

	// setting up second dppi channel - where rtc is cleared based on toggle from led
	uint32_t tsk_addr_2 = nrfx_rtc_task_address_get(&rtc, NRF_RTC_TASK_CLEAR);
	// uint32_t evt_addr_2 = nrfx_gpiote_in_event_address_get((nrfx_gpiote_pin_t)LED_PIN_NUMBER);
	nrfx_gppi_channel_endpoints_setup(dppi_channel, evt_addr, tsk_addr_2);
}

void rtc_setup(void)
{
	int err;

	// configuring the RTC settings, just using the default
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;
	// config.prescaler = 3276;
	// led_init();
	err = nrfx_rtc_init(&rtc, &config, &rtc_handler);
	if (err != NRFX_SUCCESS)
	{
		printk("did not manage to initialize the rtc");
	}
	// //	IRQ_DIRECT_CONNECT(RTC1_IRQn, IRQ_PRIO_LOWEST, nrfx_rtc_1_irq_handler, 0);
	// IRQ_DIRECT_CONNECT(NRFX_IRQ_NUMBER_GET(NRF_RTC_INST_GET(RTC_IDX)), IRQ_PRIO_LOWEST,
	// 				   NRFX_RTC_INST_HANDLER_GET(RTC_IDX), 0);

	// initializing the rtc, need to define the handler

	// uint32_t ticks = calculate_ticks(3, 1);
	nrfx_rtc_counter_clear(&rtc);
	nrfx_rtc_enable(&rtc);

	// Enable the rtc, not sure if I was supposed to use the rtc or if I was supposed to just use a timer
	err = nrfx_rtc_cc_set(&rtc, COMPARE_CHANNEL, TICKS_US, false); // setting compare register 0

	if (err != NRFX_SUCCESS)
	{
		printk("cc set failed\n");
	}
	if (err == NRFX_SUCCESS)
	{
		printk("cc set succeded\n");
	}
}

int main(void)
{

	printk("Start\n");
	printk("dppi channel: %d\n", dppi_channel);

	rtc_setup();

	//
	conf_gpiote();
	conf_dppi();
	//  nrfx_rtc_counter_get(&rtc);
	while (1)
	{
		// k_msleep(5000);
		// printk("counter value: %d \n", nrfx_rtc_counter_get(&rtc));
	}

	/*WHATS LEFT TO DO*/
	// 1. Configure a led light and turn it on - should I here use gpiote?
	// 1. connect to a dppi channel, so the event can trigger a task
	// 2. configure the task, which is that a led should toggle, connect this to the event (timer event)
	// Not sure wether I should start configuring the dppi channel or if I should start with just initializing the led and toggle the led
	//

	return 0;
}
