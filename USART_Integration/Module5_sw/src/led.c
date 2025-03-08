/*
 * blinky.c -- working with Serial I/O and GPIO
 *
 * Assumes the LED's are connected to AXI_GPIO_0, on channel 1
 *
 * Terminal Settings:
 *  -Baud: 115200
 *  -Data bits: 8
 *  -Parity: no
 *  -Stop bits: 1
 */
#include <stdio.h>							/* printf(), getchar() */
#include "xil_types.h"					/* u32, u16 etc */
#include "platform.h"						/* ZYBOboard interface */
#include <xgpio.h>							/* Xilinx GPIO functions */
#include <xgpiops.h>
#include "xparameters.h"				/* constants used by the hardware */
#include <stdbool.h>
#include <math.h>

#define OUTPUT 0x1							/* setting GPIO direction to output */
#define CHANNEL1 1							/* channel 1 of the GPIO port */
#define LED4_PIN 7

static XGpio port;
static XGpioPs ps_gpio_port;
/*
 * Initialize the led module
 */
void led_init(void){
	   XGpio_Initialize(&port, XPAR_AXI_GPIO_0_DEVICE_ID);	/* initialize device AXI_GPIO_0 */
	   XGpio_SetDataDirection(&port, CHANNEL1, OUTPUT);



		// Initialize PS GPIO for LED4
		XGpioPs_Config *ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
		XGpioPs_CfgInitialize(&ps_gpio_port, ps_config, ps_config->BaseAddr);

		// Configure LED4 as output
		XGpioPs_SetDirectionPin(&ps_gpio_port, LED4_PIN, OUTPUT); // Set as output
		XGpioPs_SetOutputEnablePin(&ps_gpio_port, LED4_PIN, 1); // Enable output
		//Set LED4 as OFF
		XGpioPs_WritePin(&ps_gpio_port, LED4_PIN, 0);

}

/*
 * Set <led> to one of {LED_ON,LED_OFF,...}
 *
 * <led> is either ALL or a number >= 0
 * Does nothing if <led> is invalid
 */
void led_set(u32 led, bool tostate) {
    if (led < 1 || led > 4) {
        return;
    }

    u32 current_leds = XGpio_DiscreteRead(&port, CHANNEL1);

    switch (led) {
        case 1:
            current_leds = (current_leds & ~0x1) | (tostate ? 0x1 : 0x0);
            break;
        case 2:
            current_leds = (current_leds & ~0x2) | (tostate ? 0x2 : 0x0);
            break;
        case 3:
            current_leds = (current_leds & ~0x4) | (tostate ? 0x4 : 0x0);
            break;
        case 4:
            current_leds = (current_leds & ~0x8) | (tostate ? 0x8 : 0x0);
            break;
        default:

            return;
    }

    XGpio_DiscreteWrite(&port, CHANNEL1, current_leds);
}

/*
 * Get the status of <led>
 *
 * <led> is a number >= 0
 * returns {LED_ON,LED_OFF,...}; LED_OFF if <led> is invalid
 */
bool led_get(u32 led) {
    if (led < 1 || led > 4) {
        return 0;
    }


    u32 hardware_leds = XGpio_DiscreteRead(&port, CHANNEL1);


    switch (led) {
        case 1:
            return (hardware_leds & 0x1) ? 1 : 0; // LED_ON = 1, LED_OFF = 0
        case 2:
            return (hardware_leds & 0x2) ? 1 : 0;
        case 3:
            return (hardware_leds & 0x4) ? 1 : 0;
        case 4:
            return (hardware_leds & 0x8) ? 1 : 0;
        default:
            return 0;
    }
}

int twopower(int val){

	int pow = 1;
	for(int i = 1; i < val; i++)
	{
		pow = pow * 2;
	}

	return pow;
}
/*
 * Toggle <led>
 *
 * <led> is a number >= 0
 * Does nothing if <led> is invalid
 */

bool led4 = false;

void led_toggle(u32 led){


	if(led == 5){

		if(led4 == false){
			XGpioPs_WritePin(&ps_gpio_port, LED4_PIN, 1);
			led4 = true;
		}
		else{
			XGpioPs_WritePin(&ps_gpio_port, LED4_PIN, 0);
			led4 = false;
		}
	}
	else{
		u32 current_leds = XGpio_DiscreteRead(&port, CHANNEL1);
		current_leds = current_leds ^ (u32)twopower((int)led);
		XGpio_DiscreteWrite(&port, CHANNEL1, current_leds);
	}

}

void led_bitwise(u32 bitwise){
if(bitwise > 1111){
return;
}

XGpio_DiscreteWrite(&port, CHANNEL1, bitwise);
}
