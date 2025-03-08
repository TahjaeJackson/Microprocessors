#include "servo.h"
#include "xtmrctr.h"
#include "xparameters.h"
#include <stdio.h>

#define TIMER_COUNTER_0 0
#define PWM_PERIOD 1000000  // 20ms period assuming 100MHz clock (calculation on sheet)

//Discussed with TA, could be a potential error handling implementation
#define MIN_DUTY_CYCLE 5.0  // 5%
#define MAX_DUTY_CYCLE 10.0 // 10%
#define TIMER_DEVICE_ID XPAR_AXI_TIMER_0_DEVICE_ID
#define PERIOD 0
#define PULSEWIDTH 1
#define MIN_DUTY 5.0   // 0.5ms pulse width
#define MAX_DUTY 10.0  // 2.5ms pulse width
#define STEP_SIZE 0.25 // Step increment/decrement

static XTmrCtr TimerInstance;

void servo_init(void) {
    int Status;

    // Initialize the timer
    Status = XTmrCtr_Initialize(&TimerInstance, TIMER_DEVICE_ID);

    //error handling
    if (Status != XST_SUCCESS) {
        printf("Servo initialization failed!\n");
        return;
    }
    XTmrCtr_Stop(&TimerInstance,PERIOD);
    XTmrCtr_Stop(&TimerInstance,PULSEWIDTH);


    u32 options = XTC_PWM_ENABLE_OPTION | XTC_DOWN_COUNT_OPTION | XTC_EXT_COMPARE_OPTION;

     // Set PWM mode options for pulsewidth
    XTmrCtr_SetOptions(&TimerInstance, PULSEWIDTH, options);

       // Set PWM mode options for period
    XTmrCtr_SetOptions(&TimerInstance, PERIOD, options);

    // Set initial duty cycle to 7.5%
    // Set the reset value for pulse 1 ms
     XTmrCtr_SetResetValue(&TimerInstance, PULSEWIDTH, 75000);

        // Set the reset value for period 20 ms
     XTmrCtr_SetResetValue(&TimerInstance, PERIOD, 1000000);



    // Start the timer
    XTmrCtr_Start(&TimerInstance, PULSEWIDTH);

       // Start the timer
    XTmrCtr_Start(&TimerInstance, PERIOD);

}

void servo_set(double dutycycle) {
    if (dutycycle < MIN_DUTY_CYCLE) {

    	dutycycle = MIN_DUTY_CYCLE;
    }
    if (dutycycle > MAX_DUTY_CYCLE){

    	dutycycle = MAX_DUTY_CYCLE;
    }

    if(dutycycle == MIN_DUTY_CYCLE){

    }
    if(dutycycle == MAX_DUTY_CYCLE){

    }

    // Update the current duty cycle
//    double current_duty_cycle = dutycycle;

    // Calculate pulse width based on duty cycle
    u32 pulse_width = (u32)((dutycycle / 100.0) * PWM_PERIOD);

    // Setting the reset value for PWM signal
    XTmrCtr_SetResetValue(&TimerInstance, PULSEWIDTH, pulse_width);


}
