
#include <stdio.h> /* printf(), getchar() */
#include "xil_types.h" /* u32, u16 etc */
#include "platform.h" /* ZYBOboard interface */
#include <xgpio.h> /* Xilinx GPIO functions */
#include <xgpiops.h>
#include "xparameters.h" /* constants used by the hardware */
#include <stdbool.h>
#include <math.h>
/* led states */
#define LED_ON true
#define LED_OFF false

/* led colors */
#define RED 5
#define BLUE 6
#define GREEN 7
#define YELLOW 8

#define ALL 0xFFFFFFFF /* A value designating ALL leds */


#define OUTPUT 0x1 /* setting GPIO direction to output */
#define CHANNEL1 1 /* channel 1 of the GPIO port */
#define LED4_PIN 7

static XGpio port;
static XGpioPs ps_gpio_port;
/*
 * Initialize the led module
 */
static XGpio port;          // For AXI_GPIO_0 (Regular LEDs)
static XGpio port_led6;     // For AXI_GPIO_1 (LED6)
static XGpioPs ps_gpio_port;


int twopower(int val){

int pow = 1;
for(int i = 1; i < val; i++)
{
pow = pow * 2;
}

return pow;
}

void led_init(void){
    // Initialize AXI_GPIO_0 (for standard LEDs)
    XGpio_Initialize(&port, XPAR_AXI_GPIO_0_DEVICE_ID);
    XGpio_SetDataDirection(&port, CHANNEL1, OUTPUT);

    // Initialize AXI_GPIO_1 (for LED6)
    XGpio_Initialize(&port_led6, XPAR_AXI_GPIO_3_DEVICE_ID);
    XGpio_SetDataDirection(&port_led6, CHANNEL1, OUTPUT);

    // Initialize PS GPIO for LED4
    XGpioPs_Config *ps_config = XGpioPs_LookupConfig(XPAR_PS7_GPIO_0_DEVICE_ID);
    XGpioPs_CfgInitialize(&ps_gpio_port, ps_config, ps_config->BaseAddr);

    // Configure LED4 as output
    XGpioPs_SetDirectionPin(&ps_gpio_port, LED4_PIN, OUTPUT);
    XGpioPs_SetOutputEnablePin(&ps_gpio_port, LED4_PIN, 1);
    XGpioPs_WritePin(&ps_gpio_port, LED4_PIN, 0);  // Turn off LED4
}


/*
 * Set <led> to one of {LED_ON,LED_OFF,...}
 *
 * <led> is either ALL or a number >= 0
 * Does nothing if <led> is invalid
 */
void led_set(u32 led, bool tostate) {
    u32 current_leds;

    // Handle LED6 separately (on AXI_GPIO_1)
    if (led == RED || led == GREEN || led == BLUE || led == YELLOW) {
        current_leds = 0;
        XGpio_DiscreteWrite(&port_led6, CHANNEL1, 0);
        if (led == RED) {
            if (tostate) {
                current_leds |= 0x4; // Turn on bit 0 (Red)
            } else {
                current_leds &= ~0x4; // Turn off bit 0
            }
        }

        if (led == GREEN) {
            if (tostate) {
                current_leds |= 0x2; // Turn on bit 2 (Green)
            } else {
                current_leds &= ~0x2; // Turn off bit 2
            }
        }

        if (led == BLUE) {
            if (tostate) {
                current_leds |= 0x1; // Turn on bit 1 (Blue)
            } else {
                current_leds &= ~0x1; // Turn off bit 1
            }
        }

        if (led == YELLOW) {  // Yellow = Red + Green
            if (tostate) {
                current_leds |= 0x6; // Turn on Red (0x1) + Green (0x4) = 0x5
            } else {
                current_leds &= ~0x6; // Turn off Red and Green
            }
        }

        XGpio_DiscreteWrite(&port_led6, CHANNEL1, current_leds);
        return;
    }

    // Handle LEDs 1-4 (on AXI_GPIO_0)
    if (led >= 1 && led <= 4) {
        current_leds = XGpio_DiscreteRead(&port, CHANNEL1);

        if (tostate) {
            current_leds |= twopower((int)led - 1);  // Turn on LED
        } else {
            current_leds &= ~twopower((int)led - 1); // Turn off LED
        }

        XGpio_DiscreteWrite(&port, CHANNEL1, current_leds);
    }
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


