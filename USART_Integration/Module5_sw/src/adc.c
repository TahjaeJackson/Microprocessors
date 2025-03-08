/*
 * adc.h -- The ADC module interface
 */

#include "adc.h"


static XAdcPs XadcInstance;
/*
 * initialize the adc module
 */
void adc_init(void){

	int Status;
	XAdcPs_Config *configPtr;


	configPtr = XAdcPs_LookupConfig(XPAR_XADCPS_0_DEVICE_ID);
	if (configPtr == NULL ) {
			printf("initialization failed!\n");
		}

	Status = XAdcPs_CfgInitialize(&XadcInstance, configPtr,configPtr->BaseAddress); // might need to change the adder
	if (Status != XST_SUCCESS) {
		printf("initialization failed!\n");
	}

	Status = XAdcPs_SelfTest(&XadcInstance);
	if (Status != XST_SUCCESS) {
			printf("initialization failed!\n");
		}

	XAdcPs_SetSequencerMode(&XadcInstance, XADCPS_SEQ_MODE_SAFE); // Ensure safe state

	XAdcPs_SetAlarmEnables(&XadcInstance, 0x0000); // from git repo to disable all alarms

	//enabling channels for Temp Voltage and Potentiometer
	Status = XAdcPs_SetSeqChEnables(&XadcInstance, ( XADCPS_SEQ_CH_AUX14 | XADCPS_SEQ_CH_VCCINT | XADCPS_SEQ_CH_TEMP ));
	if (Status != XST_SUCCESS) {
			printf("initialization failed! - here\n");
		}

	XAdcPs_SetSequencerMode(&XadcInstance, XADCPS_SEQ_MODE_CONTINPASS);

}

/*
 * get the internal temperature in degree's centigrade
 */
u16 adc_val;
float adc_get_temp(void){

	adc_val = XAdcPs_GetAdcData(&XadcInstance, XADCPS_CH_TEMP);
	float temp = XAdcPs_RawToTemperature(adc_val);
	return temp;
}

/*
 * get the internal vcc voltage (should be ~1.0v)
 */
float adc_get_vccint(void){

	adc_val = XAdcPs_GetAdcData(&XadcInstance, XADCPS_CH_VCCINT);
	float voltage = XAdcPs_RawToVoltage(adc_val);
	return voltage;
}

/*
 * get the **corrected** potentiometer voltage (should be between 0 and 1v)
 */
float adc_get_pot(void){

	adc_val = XAdcPs_GetAdcData(&XadcInstance, XADCPS_CH_AUX_MIN + 14);
	float VPot = XAdcPs_RawToVoltage(adc_val);
	return VPot;
}

