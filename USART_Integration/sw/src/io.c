/*
 * io.c -- io module implementation
 */

#include "io.h"
#include "xgpio.h"
#include "xparameters.h"
#include <stdio.h>  /* For debugging output */
#include <stdlib.h> /* strtod */
#include <stdbool.h> /* type bool */
#include <unistd.h> /* sleep */
#include <string.h>

#include "platform.h" /* ZYBO board interface */
#include "xil_types.h" /* u32, s32 etc */
#include "xparameters.h" /* constants used by hardware */

#include "gic.h" /* interrupt controller interface */
#include "xgpio.h" /* axi gpio interface */

/* Private GPIO instances */
static XGpio btnport;  // Button GPIO instance
static XGpio swport;   // Switch GPIO instance

/* Callback function pointers */
static void (*btn_callback)(u32 btn) = NULL;
static void (*sw_callback)(u32 sw) = NULL;


/*
 * Button interrupt handler
 */
void io_btn_handler(void *callbackRef) {
	XGpio *dev = (XGpio*) callbackRef;
    u32 btn_status = XGpio_DiscreteRead(dev, 1);

    // Call the registered callback function
    if (btn_callback != NULL) {
        btn_callback(btn_status);
    }

    // Clear interrupt flag to allow further interrupts
    XGpio_InterruptClear(dev, XGPIO_IR_CH1_MASK);
}


/*
 * Switch reading function (to be called periodically)
 */
void io_sw_handler(void *callbackRef) {
	XGpio *dev = (XGpio*) callbackRef;
    u32 sw_status = XGpio_DiscreteRead(dev, 1);

    // Invoke the registered callback function if it's set
    if (sw_callback != NULL) {
        sw_callback(sw_status);
    }
}


/*
 * Initialize button GPIO and register the callback function
 */
void io_btn_init(void (*callback)(u32 btn)) {
    if (callback == NULL) {
        printf("Error: Button callback is NULL.\n");
        return;
    }

    int status = XGpio_Initialize(&btnport, XPAR_AXI_GPIO_1_DEVICE_ID);
    if (status != XST_SUCCESS) {
        printf("Button GPIO initialization failed.\n");
        return;
    }

    btn_callback = callback;

    gic_init();


    gic_connect(XPAR_FABRIC_GPIO_1_VEC_ID, io_btn_handler, &btnport);
    // Disable and clear interrupts before enabling
    XGpio_InterruptDisable(&btnport, XGPIO_IR_CH1_MASK);
    XGpio_InterruptClear(&btnport, XGPIO_IR_CH1_MASK);


    // Enable interrupts
    XGpio_InterruptEnable(&btnport, XGPIO_IR_CH1_MASK);
    XGpio_InterruptGlobalEnable(&btnport);


}



/*
 * Cleanup button resources
 */
void io_btn_close() {
    btn_callback = NULL;
    gic_disconnect(XPAR_FABRIC_GPIO_1_VEC_ID);
    gic_close();
}

/*
 * Initialize switch GPIO and register the callback function
 */
void io_sw_init(void (*callback)(u32 sw)) {
    if (callback == NULL) {
        printf("Error: Switch callback is NULL.\n");
        return;
    }

    int status = XGpio_Initialize(&swport, XPAR_AXI_GPIO_2_DEVICE_ID);
    if (status != XST_SUCCESS) {
        printf("Switch GPIO initialization failed.\n");
        return;
    }

    sw_callback = callback;


    gic_connect(XPAR_FABRIC_GPIO_2_VEC_ID, io_sw_handler, &swport);
    // Disable and clear interrupts before enabling
    XGpio_InterruptDisable(&swport, XGPIO_IR_CH1_MASK);
    XGpio_InterruptClear(&swport, XGPIO_IR_CH1_MASK);


        // Enable interrupts
    XGpio_InterruptEnable(&swport, XGPIO_IR_CH1_MASK);
    XGpio_InterruptGlobalEnable(&swport);

    //gic_connect(XPAR_FABRIC_GPIO_2_VEC_ID, (Xil_InterruptHandler) io_sw_handler, &swport);

}


/*
 * Cleanup switch resources
 */
void io_sw_close() {
    sw_callback = NULL;
    gic_disconnect(XPAR_FABRIC_GPIO_2_VEC_ID);
    printf("Switch GPIO resources cleaned up.\n");
}
