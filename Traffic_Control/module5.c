/*
 * main.c --
 *  Some useful values:
 *  -- XPAR_AXI_GPIO_1_DEVICE_ID -- xparameters.h
 *  -- XPAR_FABRIC_GPIO_1_VEC_ID -- xparameters.h
 *  -- XGPIO_IR_CH1_MASK         -- xgpio_l.h (included by xgpio.h)
 */
#include <stdio.h> /* getchar,printf */
#include <stdlib.h> /* strtod */
#include <stdbool.h> /* type bool */
#include <unistd.h> /* sleep */
#include <string.h>

#include "platform.h" /* ZYBO board interface */
#include "xil_types.h" /* u32, s32 etc */
#include "xparameters.h" /* constants used by hardware */

#include "gic.h" /* interrupt controller interface */
#include "xgpio.h" /* axi gpio interface */
#include "led.h"
#include "io.h"
#include "servo.h"
#include "adc.h"



#define CONFIGURE 0
#define PING 1
#define UPDATE 2
// Define the delay for each light state in microseconds (3 seconds)
#define TRAFFIC_DELAY 3000000

// Global flag for maintenance mode (set this when entering maintenance mode)
volatile bool maintenance_mode = false;

volatile bool pedestrian_request = false; // stores pedestrian status


#define GATE_OPEN_DUTY 5.0  // e.g., 5.0%
#define GATE_CLOSED_DUTY 10.0 // e.g., 10.0%

bool red = false;
bool yellow = false;
bool green = true;
bool force = true;

typedef struct {
int type;
int id;
} ping_t;

void clear_colors(){
	led_set(YELLOW, LED_OFF);
    led_set(BLUE, LED_OFF);
	led_set(GREEN, LED_OFF);
	led_set(RED, LED_OFF);
}

/* hidden private state */


/*
 * control is passed to this function when a button is pushed
 *
 * devicep -- ptr to the device that caused the interrupt
 */


int done = 0;
static XUartPs XUartPs_Instance0; // void pointer to an instance of a UART
static XUartPs XUartPs_Instance1; // void pointer to an instance of a UART



void ping(){
ping_t myPing;
myPing.id =8;
myPing.type = PING;
u8 StoreLoss[8] = "[Fail]\n\r";
XUartPs_Send(&XUartPs_Instance0, (u8*)&myPing, sizeof(myPing));

u8 RecvBuffer[64] = {0};
    int packet = XUartPs_Recv(&XUartPs_Instance0, &RecvBuffer[0], sizeof(ping_t));
    if (packet != sizeof(ping_t)){
XUartPs_Send(&XUartPs_Instance1, &StoreLoss[0], 8);
    }
    else{
ping_t* gotPing = (ping_t*)&RecvBuffer;
u8 id =(u8) gotPing->id;
u8 type =(u8) gotPing->type;
XUartPs_Send(&XUartPs_Instance1, &id, 1);
XUartPs_Send(&XUartPs_Instance1, &type, 1);
    }

}


// Function to turn off all traffic lights
void traffic_lights_off() {
	led_set(RED, LED_OFF);
	led_set(YELLOW, LED_OFF);
	led_set(BLUE, LED_OFF);
	led_set(GREEN, LED_OFF);
}


//GATE HANDLING

void gate_open(void) {
    servo_set(GATE_OPEN_DUTY);
    printf("Gate is now open.\n");
}

void gate_close(void) {
    servo_set(GATE_CLOSED_DUTY);
    printf("Gate is now closed.\n");
}



bool crosswalk_requested = false;

void btn_callback(u32 btn){

    u8 StoreUpdate[10] = "[UPDATE]\n\r";
    u8 StorePing[8] = "[PING]\n\r";
    u8 StoreTrigger[3] = "$$$";

if(btn & 0x01){
XUartPs_Send(&XUartPs_Instance1, &StoreUpdate[0], 10);
XUartPs_Send(&XUartPs_Instance0, &StoreTrigger[0], 3);
crosswalk_requested = true;
        printf("Pedestrian crossing request detected.\n");

}
else if(btn & 0x02){
XUartPs_Send(&XUartPs_Instance1, &StorePing[0], 8);
XUartPs_Send(&XUartPs_Instance1, &StoreUpdate[0], 10);
XUartPs_Send(&XUartPs_Instance0, &StoreTrigger[0], 3);
crosswalk_requested = true;
        printf("Pedestrian crossing request detected.\n");
ping();
}
else if(btn & 0x04){
XUartPs_Send(&XUartPs_Instance1, &StoreUpdate[0], 10);

}
else if(btn & 0x08){
done = 1;

}

}
bool wait_20sec_after_train = false;
bool train_status = false;
u32 currentswitches = 0;
int second_timer = 0;
void sw_callback(u32 sw){




if((sw ^ currentswitches) == 0x01){
	traffic_lights_off();
	maintenance_mode = !maintenance_mode;  // Toggle maintenance mode
        if (maintenance_mode) {
            printf("Maintenance Mode Enabled.\n");
            force = false;
            // Optionally, update LEDs or display messages to indicate maintenance mode.
        } else {
            printf("Maintenance Mode Disabled.\n");
            second_timer = 0;
            red = true;
            green = false;
            yellow = false;
            force = true;
            clear_colors();
            led_set(RED, LED_ON);
        }

}

else if((sw ^ currentswitches) == 0x02){
	train_status = !train_status;
	traffic_lights_off();
		if(train_status){
			force = false;
			printf("Train arriving!\n");
        	//close the gate immediately:
			wait_20sec_after_train = true;
	    	second_timer = 0;
	    	gate_close();
		}
		else{
			printf("Train departed!\n");
			force = true;
		}
}

else if((sw ^ currentswitches) == 0x04){
}

else if((sw ^ currentswitches) == 0x08){
}

currentswitches = sw;


}



void handler1(void *CallBackRef, u32 Event, unsigned int EventData) {
    XUartPs *UartInstPtr = (XUartPs *)CallBackRef;
    u8 RecvBuffer[64] = {0};  // Buffer to hold received character
    u8 StoreChar = (u8)'\n';
    if (Event == XUARTPS_EVENT_RECV_DATA) {
    int rVal = XUartPs_Recv(UartInstPtr, &RecvBuffer[0], 1);
if (RecvBuffer[0] == '\r'){
XUartPs_Send(&XUartPs_Instance0, &StoreChar, rVal);  // Echo character
}
XUartPs_Send(&XUartPs_Instance0, &RecvBuffer[0], rVal);  // Echo character
    }
}



void handler0(void *CallBackRef, u32 Event, unsigned int EventData) {
    XUartPs *UartInstPtr = (XUartPs *)CallBackRef;
    u8 RecvBuffer[64] = {0};  // Buffer to hold received character
    if (Event == XUARTPS_EVENT_RECV_DATA) {
    int rVal = XUartPs_Recv(UartInstPtr, &RecvBuffer[0], 1);
    XUartPs_Send(&XUartPs_Instance1, &RecvBuffer[0], rVal);  // Echo character

    }

}







int tenthofsecond = 1000000/10;
int tenthofsecond_timer = 0;
int hundredthofsecond = 1000000/100;
int hundredthofsecond_timer = 0;
bool blueflash = false;
float voltage = 0;


void traffic_light_sequence() {
	if(train_status){
		clear_colors();
		led_set(RED, LED_ON);
		tenthofsecond_timer = 0;
		second_timer = 0;
	}
	else if(wait_20sec_after_train && !maintenance_mode){
		usleep(tenthofsecond);
		tenthofsecond_timer++;
		if(tenthofsecond_timer >= 10){
			tenthofsecond_timer = 0;
			second_timer++;
		}
		if(second_timer >= 10){
			tenthofsecond_timer = 0;
			second_timer = 0;
			clear_colors();
        	wait_20sec_after_train = false;
		}
	}
	else if(maintenance_mode){
    	wait_20sec_after_train = false;
    	usleep(hundredthofsecond);
    	hundredthofsecond_timer++;
    	if(hundredthofsecond_timer >= 100){
    		hundredthofsecond_timer = 0;
    		blueflash = !blueflash;
    	}

        led_set(BLUE, blueflash);

		voltage= adc_get_pot();
		double duty = ((voltage/3)/20 + .05) * 100;
		servo_set(duty);
	}
	else if(red == true){
		if(force == true){
			led_set(RED, LED_ON);
			force = false;
	        gate_open();
		}
    	usleep(tenthofsecond);
    	tenthofsecond_timer++;
    	if(tenthofsecond_timer >= 10){
    		tenthofsecond_timer = 0;
    		second_timer++;
    	}
    	if(second_timer >= 10000){
    		second_timer = 10000;
    	}
    	if(second_timer >= 10){
    		tenthofsecond_timer = 0;
    		second_timer = 0;
    		yellow = false;
    		green = true;
        	red = false;

    		//Green State Transition
        	traffic_lights_off();
        	led_set(YELLOW, LED_OFF);
        	led_set(RED, LED_OFF);
            led_set(BLUE, LED_OFF);
        	led_set(GREEN, LED_ON);
    	}
    }

    //defines behavior when in yellow state
    else if(yellow == true){
    	usleep(TRAFFIC_DELAY);

        // Red state transition
		yellow = false;
		green = false;
    	red = true;
    	traffic_lights_off();
        led_set(YELLOW, LED_OFF);
        led_set(GREEN, LED_OFF);
        led_set(BLUE, LED_OFF);
        led_set(RED, LED_ON);
    	led_bitwise(0);



    }


    //defines behavior when in green state
    else if(green == true){
		if(force == true){
			led_set(GREEN, LED_ON);
			force = false;
		}
    	usleep(tenthofsecond);
    	tenthofsecond_timer++;
    	if(tenthofsecond_timer >= 10){
    		tenthofsecond_timer = 0;
    		second_timer++;
    	}
    	if(second_timer >= 10000){
    		second_timer = 10000;
    	}
    	if(crosswalk_requested == true && second_timer >= 10){
    		second_timer = 0;
    		tenthofsecond_timer = 0;
    		//Yellow State Transition
    		yellow = true;
    		green = false;
    		red = false;
    		traffic_lights_off();
        	led_set(RED, LED_OFF);
        	led_set(GREEN, LED_OFF);
            led_set(BLUE, LED_OFF);
        	led_set(YELLOW, LED_ON);
        	crosswalk_requested = false;
        	led_bitwise(15);

    		}

    	}
    }



int duty_cycle = 7.5;
int main() {




    init_platform();



    setvbuf(stdin, NULL, _IONBF, 0); //turn off buffering



    if (gic_init() != XST_SUCCESS) {
        printf("GIC initialization failed\n\r");
        return XST_FAILURE;
    }

    led_init();

    servo_init();

	adc_init();
	servo_set(duty_cycle);


    io_sw_init(sw_callback);
    io_btn_init(btn_callback);


    //
    //
    //
    //UART 1
    //
    //
    //


    XUartPs_Config *configPtr1 = XUartPs_LookupConfig(XPAR_PS7_UART_1_DEVICE_ID);
    if (configPtr1 == NULL) {
        printf("UART1 lookup failed!\n\r");
        return XST_FAILURE;
    }

    int Status1 = XUartPs_CfgInitialize(&XUartPs_Instance1, configPtr1, configPtr1->BaseAddress);
    if (Status1 != XST_SUCCESS) {
        printf("UART1 Initialization failed!\n\r");
        return XST_FAILURE;
    }

    XUartPs_SetBaudRate(&XUartPs_Instance1, 115200);
    XUartPs_SetFifoThreshold(&XUartPs_Instance1,1);
    XUartPs_SetHandler(&XUartPs_Instance1, (XUartPs_Handler)handler1, &XUartPs_Instance1);

    Status1 = gic_connect(XPAR_XUARTPS_1_INTR, (Xil_ExceptionHandler)XUartPs_InterruptHandler, &XUartPs_Instance1);
    if (Status1 != XST_SUCCESS) {
        printf("UART1 interrupt connection failed!\n\r");
        return XST_FAILURE;
    }


    //
    //
    //
    //UART 0
    //
	//
	//


    XUartPs_Config *configPtr0 = XUartPs_LookupConfig(XPAR_PS7_UART_0_DEVICE_ID);
    if (configPtr0 == NULL) {
        printf("UART0 lookup failed!\n\r");
        return XST_FAILURE;
    }

    int Status0 = XUartPs_CfgInitialize(&XUartPs_Instance0, configPtr0, configPtr0->BaseAddress);
    if (Status0 != XST_SUCCESS) {
        printf("UART0 Initialization failed!\n\r");
        return XST_FAILURE;
    }

    XUartPs_SetBaudRate(&XUartPs_Instance0, 9600);
    XUartPs_SetInterruptMask(&XUartPs_Instance0, XUARTPS_IXR_RXOVR );
    XUartPs_SetFifoThreshold(&XUartPs_Instance0,1);
    XUartPs_SetHandler(&XUartPs_Instance0, (XUartPs_Handler)handler0, &XUartPs_Instance0);

    Status0 = gic_connect(XPAR_XUARTPS_0_INTR, (Xil_ExceptionHandler)XUartPs_InterruptHandler, &XUartPs_Instance0);
    if (Status0 != XST_SUCCESS) {
        printf("UART interrupt connection failed!\n\r");
        return XST_FAILURE;
    }






	printf("Program Started\n\r");
	fflush(stdout);

    printf("\n[hello]\n\r");
    led_set(RED, LED_ON);






	//initialize traffic light to green

	led_set(YELLOW, LED_OFF);
	led_set(RED, LED_OFF);
	led_set(BLUE, LED_OFF);
	led_set(GREEN, LED_ON);

    while (!done) {
        traffic_light_sequence();
    }

    printf("\n[done]\n\r");
    sleep(1);

    // Cleanup
    XUartPs_DisableUart(&XUartPs_Instance1);
    XUartPs_DisableUart(&XUartPs_Instance0);
    gic_disconnect(XPAR_XUARTPS_0_INTR);
    gic_close();
    io_sw_close();
    io_btn_close();
    cleanup_platform();

    return 0;
}


