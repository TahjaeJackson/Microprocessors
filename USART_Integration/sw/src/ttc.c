/* Authors: Tahjae Jackson and Harry Leiter
assignment: ttc.c module (implementation of ttc.h)

*/

#include "ttc.h"
#include <stdio.h>
#include "xttcps.h"
#include "xparameters.h"    /* constants used by the hardware */
#include "xil_types.h"      /* types used by xilinx */
#include "gic.h"

// TTC instance
XTtcPs TtcInstance;

// TTC interrupt callback
void (*TtcCallback)(void);

// TTC interrupt handler
static void TtcInterruptHandler(void *CallBackRef) {
    XTtcPs *InstancePtr = (XTtcPs *)CallBackRef;

    // Clear interrupt status
    u32 Status = XTtcPs_GetInterruptStatus(InstancePtr);
    XTtcPs_ClearInterruptStatus(InstancePtr, Status);

    // Call the user-provided callback, if available
     if (TtcCallback != NULL) {
         TtcCallback();
     }


}

// Initialize the TTC
void ttc_init(u32 freq, void (*ttc_callback)(void)) {
    // Set the user-provided callback
    TtcCallback = ttc_callback;

    // Lookup TTC configuration
    XTtcPs_Config *Config = XTtcPs_LookupConfig(XPAR_XTTCPS_0_DEVICE_ID);
    if (Config == NULL) {
        printf("Error: TTC configuration not found!\n");
        return;
    }

    // Initialize the TTC instance
    int Status = XTtcPs_CfgInitialize(&TtcInstance, Config, Config->BaseAddress);
    if (Status != XST_SUCCESS) {
        printf("Error: TTC initialization failed!\n");
        return;
    }

    // Calculate interval and prescaler for the given frequency
    u8 Prescaler;
    XInterval Interval;
    XTtcPs_CalcIntervalFromFreq(&TtcInstance, freq, &Interval, &Prescaler);

    // Set the interval and prescaler
    XTtcPs_SetPrescaler(&TtcInstance, Prescaler);
    XTtcPs_SetInterval(&TtcInstance, Interval);

    // Set TTC options (interval mode)
    XTtcPs_SetOptions(&TtcInstance, XTTCPS_OPTION_INTERVAL_MODE);

    // Enable TTC interrupts
    XTtcPs_EnableInterrupts(&TtcInstance, XTTCPS_IXR_INTERVAL_MASK);

	XTtcPs_Start(&TtcInstance);

	gic_init();
	gic_connect(XPAR_XTTCPS_0_INTR, TtcInterruptHandler, &TtcInstance);


    printf("TTC initialized with frequency: %lu Hz\n", freq);
}

// Start the TTC
void ttc_start(void) {
    XTtcPs_Start(&TtcInstance);
    printf("TTC started.\n");
}

// Stop the TTC
void ttc_stop(void) {
    XTtcPs_Stop(&TtcInstance);
    printf("TTC stopped.\n");
}

// Close the TTC
void ttc_close(void) {
	  /* disconnect the interrupts (c.f. gic.h) */
	  gic_disconnect(XPAR_XTTCPS_0_INTR);

	  /* close the gic (c.f. gic.h)   */
	  gic_close();
    printf("TTC closed.\n");
}
