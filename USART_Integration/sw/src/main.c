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


#define CONFIGURE 0
#define PING 1
#define UPDATE 2

typedef struct {
int type;
int id;
} ping_t;

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



void btn_callback(u32 btn){

    u8 StoreUpdate[10] = "[UPDATE]\n\r";
    u8 StorePing[8] = "[PING]\n\r";
    u8 StoreTrigger[3] = "$$$";

if(btn & 0x01){
led_toggle(1);
led_bitwise(1);
XUartPs_Send(&XUartPs_Instance1, &StoreUpdate[0], 10);
XUartPs_Send(&XUartPs_Instance0, &StoreTrigger[0], 3);

}
else if(btn & 0x02){
led_toggle(2);
led_bitwise(2);
XUartPs_Send(&XUartPs_Instance1, &StorePing[0], 8);
ping();
}
else if(btn & 0x04){
led_toggle(3);
led_bitwise(4);
XUartPs_Send(&XUartPs_Instance1, &StoreUpdate[0], 10);

}
else if(btn & 0x08){
led_toggle(4);
done = 1;

}

}

void sw_callback(u32 sw){

if(sw & 0x01){
led_toggle(1);

}
else if(sw & 0x02){
led_toggle(2);

}
else if(sw & 0x04){
led_toggle(3);

}
else if(sw & 0x08){
led_toggle(4);

}

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


int main() {




    init_platform();



    setvbuf(stdin, NULL, _IONBF, 0); //turn off buffering



    if (gic_init() != XST_SUCCESS) {
        printf("GIC initialization failed\n\r");
        return XST_FAILURE;
    }

    led_init();
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

    while (!done) {
        usleep(50000);
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







