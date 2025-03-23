/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "board.h"

//*****************************************************************************
//
// Board Configurations
// Initializes the rest of the modules. 
// Call this function in your application if you wish to do all module 
// initialization.
// If you wish to not use some of the initializations, instead of the 
// Board_init use the individual Module_inits
//
//*****************************************************************************
void Board_init()
{
	EALLOW;

	PinMux_init();
	INPUTXBAR_init();
	SYNC_init();
	ADC_init();
	ECAP_init();
	EPWM_init();
	GPIO_init();
	SCI_init();
	INTERRUPT_init();

	EDIS;
}

//*****************************************************************************
//
// PINMUX Configurations
//
//*****************************************************************************
void PinMux_init()
{
	//
	// PinMux for modules assigned to CPU1
	//
	
	//
	// EPWM1 -> myEPWM0 Pinmux
	//
	GPIO_setPinConfig(myEPWM0_EPWMA_PIN_CONFIG);
	GPIO_setPadConfig(myEPWM0_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myEPWM0_EPWMA_GPIO, GPIO_QUAL_SYNC);

	GPIO_setPinConfig(myEPWM0_EPWMB_PIN_CONFIG);
	GPIO_setPadConfig(myEPWM0_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myEPWM0_EPWMB_GPIO, GPIO_QUAL_SYNC);

	//
	// EPWM2 -> myEPWM2 Pinmux
	//
	GPIO_setPinConfig(myEPWM2_EPWMA_PIN_CONFIG);
	GPIO_setPadConfig(myEPWM2_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myEPWM2_EPWMA_GPIO, GPIO_QUAL_SYNC);

	GPIO_setPinConfig(myEPWM2_EPWMB_PIN_CONFIG);
	GPIO_setPadConfig(myEPWM2_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myEPWM2_EPWMB_GPIO, GPIO_QUAL_SYNC);

	// GPIO34 -> myBoardLED0_GPIO Pinmux
	GPIO_setPinConfig(GPIO_34_GPIO34);
	// GPIO31 -> myBoardLED1_GPIO Pinmux
	GPIO_setPinConfig(GPIO_31_GPIO31);
	//
	// SCIA -> mySCIA Pinmux
	//
	GPIO_setPinConfig(mySCIA_SCIRX_PIN_CONFIG);
	GPIO_setPadConfig(mySCIA_SCIRX_GPIO, GPIO_PIN_TYPE_STD | GPIO_PIN_TYPE_PULLUP);
	GPIO_setQualificationMode(mySCIA_SCIRX_GPIO, GPIO_QUAL_ASYNC);

	GPIO_setPinConfig(mySCIA_SCITX_PIN_CONFIG);
	GPIO_setPadConfig(mySCIA_SCITX_GPIO, GPIO_PIN_TYPE_STD | GPIO_PIN_TYPE_PULLUP);
	GPIO_setQualificationMode(mySCIA_SCITX_GPIO, GPIO_QUAL_ASYNC);


}

//*****************************************************************************
//
// ADC Configurations
//
//*****************************************************************************
void ADC_init(){
	myADC0_init();
	myADC1_init();
}

void myADC0_init(){
	//
	// Configures the analog-to-digital converter module prescaler.
	//
	ADC_setPrescaler(myADC0_BASE, ADC_CLK_DIV_4_0);
	//
	// Configures the analog-to-digital converter resolution and signal mode.
	//
	ADC_setMode(myADC0_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
	//
	// Sets the timing of the end-of-conversion pulse
	//
	ADC_setInterruptPulseMode(myADC0_BASE, ADC_PULSE_END_OF_CONV);
	//
	// Powers up the analog-to-digital converter core.
	//
	ADC_enableConverter(myADC0_BASE);
	//
	// Delay for 1ms to allow ADC time to power up
	//
	DEVICE_DELAY_US(500);
	//
	// SOC Configuration: Setup ADC EPWM channel and trigger settings
	//
	// Disables SOC burst mode.
	//
	ADC_disableBurstMode(myADC0_BASE);
	//
	// Sets the priority mode of the SOCs.
	//
	ADC_setSOCPriority(myADC0_BASE, ADC_PRI_ALL_ROUND_ROBIN);
	//
	// Start of Conversion 0 Configuration
	//
	//
	// Configures a start-of-conversion (SOC) in the ADC and its interrupt SOC trigger.
	// 	  	SOC number		: 0
	//	  	Trigger			: ADC_TRIGGER_EPWM2_SOCA
	//	  	Channel			: ADC_CH_ADCIN0
	//	 	Sample Window	: 8 SYSCLK cycles
	//		Interrupt Trigger: ADC_INT_SOC_TRIGGER_NONE
	//
	ADC_setupSOC(myADC0_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN0, 8U);
	ADC_setInterruptSOCTrigger(myADC0_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
	//
	// ADC Interrupt 1 Configuration
	// 		Source	: ADC_SOC_NUMBER0
	// 		Interrupt Source: enabled
	//		Continuous Mode	: enabled
	//
	//
	ADC_setInterruptSource(myADC0_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
	ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);
	ADC_enableContinuousMode(myADC0_BASE, ADC_INT_NUMBER1);
	ADC_enableInterrupt(myADC0_BASE, ADC_INT_NUMBER1);
}

void myADC1_init(){
	//
	// Configures the analog-to-digital converter module prescaler.
	//
	ADC_setPrescaler(myADC1_BASE, ADC_CLK_DIV_4_0);
	//
	// Configures the analog-to-digital converter resolution and signal mode.
	//
	ADC_setMode(myADC1_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);
	//
	// Sets the timing of the end-of-conversion pulse
	//
	ADC_setInterruptPulseMode(myADC1_BASE, ADC_PULSE_END_OF_ACQ_WIN);
	//
	// Powers up the analog-to-digital converter core.
	//
	ADC_enableConverter(myADC1_BASE);
	//
	// Delay for 1ms to allow ADC time to power up
	//
	DEVICE_DELAY_US(500);
	//
	// SOC Configuration: Setup ADC EPWM channel and trigger settings
	//
	// Disables SOC burst mode.
	//
	ADC_disableBurstMode(myADC1_BASE);
	//
	// Sets the priority mode of the SOCs.
	//
	ADC_setSOCPriority(myADC1_BASE, ADC_PRI_ALL_ROUND_ROBIN);
	//
	// Start of Conversion 0 Configuration
	//
	//
	// Configures a start-of-conversion (SOC) in the ADC and its interrupt SOC trigger.
	// 	  	SOC number		: 0
	//	  	Trigger			: ADC_TRIGGER_EPWM2_SOCA
	//	  	Channel			: ADC_CH_ADCIN2
	//	 	Sample Window	: 8 SYSCLK cycles
	//		Interrupt Trigger: ADC_INT_SOC_TRIGGER_NONE
	//
	ADC_setupSOC(myADC1_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM2_SOCA, ADC_CH_ADCIN2, 8U);
	ADC_setInterruptSOCTrigger(myADC1_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
	//
	// ADC Interrupt 1 Configuration
	// 		Source	: ADC_SOC_NUMBER0
	// 		Interrupt Source: enabled
	//		Continuous Mode	: enabled
	//
	//
	ADC_setInterruptSource(myADC1_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
	ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER1);
	ADC_enableContinuousMode(myADC1_BASE, ADC_INT_NUMBER1);
	ADC_enableInterrupt(myADC1_BASE, ADC_INT_NUMBER1);
}



//*****************************************************************************
//
// ECAP Configurations
//
//*****************************************************************************
void ECAP_init(){
	myECAP0_init();
}

void myECAP0_init(){
	//
	// Disable ,clear all capture flags and interrupts
	//
	ECAP_disableInterrupt(myECAP0_BASE,
		(ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
		ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
		ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
		ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
		ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
		ECAP_ISR_SOURCE_COUNTER_PERIOD   |
		ECAP_ISR_SOURCE_COUNTER_COMPARE));
	ECAP_clearInterrupt(myECAP0_BASE,
		(ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
		ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
		ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
		ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
		ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
		ECAP_ISR_SOURCE_COUNTER_PERIOD   |
		ECAP_ISR_SOURCE_COUNTER_COMPARE));
	//
	// Disables time stamp capture.
	//
	ECAP_disableTimeStampCapture(myECAP0_BASE);
	//
	// Stops Time stamp counter.
	//
	ECAP_stopCounter(myECAP0_BASE);
	//
	// Sets eCAP in Capture mode.
	//
	ECAP_enableCaptureMode(myECAP0_BASE);
	//
	// Sets the capture mode.
	//
	ECAP_setCaptureMode(myECAP0_BASE,ECAP_CONTINUOUS_CAPTURE_MODE,ECAP_EVENT_4);
	//
	// Sets the Capture event prescaler.
	//
	ECAP_setEventPrescaler(myECAP0_BASE, 0U);
	//
	// Sets the Capture event polarity.
	//
	ECAP_setEventPolarity(myECAP0_BASE,ECAP_EVENT_1,ECAP_EVNT_RISING_EDGE);
	ECAP_setEventPolarity(myECAP0_BASE,ECAP_EVENT_2,ECAP_EVNT_FALLING_EDGE);
	ECAP_setEventPolarity(myECAP0_BASE,ECAP_EVENT_3,ECAP_EVNT_RISING_EDGE);
	ECAP_setEventPolarity(myECAP0_BASE,ECAP_EVENT_4,ECAP_EVNT_FALLING_EDGE);
	//
	// Configure counter reset on events
	//
	ECAP_disableCounterResetOnEvent(myECAP0_BASE,ECAP_EVENT_1);
	ECAP_disableCounterResetOnEvent(myECAP0_BASE,ECAP_EVENT_2);
	ECAP_disableCounterResetOnEvent(myECAP0_BASE,ECAP_EVENT_3);
	ECAP_disableCounterResetOnEvent(myECAP0_BASE,ECAP_EVENT_4);
	//
	// Sets a phase shift value count.
	//
	ECAP_setPhaseShiftCount(myECAP0_BASE,0U);
	//
	// Disable counter loading with phase shift value.
	//
	ECAP_disableLoadCounter(myECAP0_BASE);
	//
	// Configures Sync out signal mode.
	//
	ECAP_setSyncOutMode(myECAP0_BASE,ECAP_SYNC_OUT_SYNCI);
	//
	// Configures emulation mode.
	//
	ECAP_setEmulationMode(myECAP0_BASE,ECAP_EMULATION_STOP);
	//
	// Starts Time stamp counter for myECAP0.
	//
	ECAP_startCounter(myECAP0_BASE);
	//
	// Enables time stamp capture for myECAP0.
	//
	ECAP_enableTimeStampCapture(myECAP0_BASE);
	//
	// Re-arms the eCAP module for myECAP0.
	//
	ECAP_reArm(myECAP0_BASE);
	//
	// Enables interrupt source for myECAP0.
	//
	ECAP_enableInterrupt(myECAP0_BASE,(ECAP_ISR_SOURCE_CAPTURE_EVENT_3));

    //-----------------Signal Monitoring--------------------//
}

//*****************************************************************************
//
// EPWM Configurations
//
//*****************************************************************************
void EPWM_init(){
    EPWM_setClockPrescaler(myEPWM0_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_2);	
    EPWM_setTimeBasePeriod(myEPWM0_BASE, 2500);	
    EPWM_setTimeBaseCounter(myEPWM0_BASE, 0);	
    EPWM_setTimeBaseCounterMode(myEPWM0_BASE, EPWM_COUNTER_MODE_UP_DOWN);	
    EPWM_disablePhaseShiftLoad(myEPWM0_BASE);	
    EPWM_setPhaseShift(myEPWM0_BASE, 0);	
    EPWM_setCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A, 1875);	
    EPWM_setCounterCompareShadowLoadMode(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);	
    EPWM_setCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_B, 0);	
    EPWM_setCounterCompareShadowLoadMode(myEPWM0_BASE, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_HIGH, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_LOW, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);	
    EPWM_setActionQualifierAction(myEPWM0_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);	
    EPWM_setDeadBandDelayPolarity(myEPWM0_BASE, EPWM_DB_FED, EPWM_DB_POLARITY_ACTIVE_LOW);	
    EPWM_setDeadBandDelayMode(myEPWM0_BASE, EPWM_DB_RED, true);	
    EPWM_setRisingEdgeDelayCountShadowLoadMode(myEPWM0_BASE, EPWM_RED_LOAD_ON_CNTR_ZERO);	
    EPWM_disableRisingEdgeDelayCountShadowLoadMode(myEPWM0_BASE);	
    EPWM_setRisingEdgeDelayCount(myEPWM0_BASE, 100);	
    EPWM_setDeadBandDelayMode(myEPWM0_BASE, EPWM_DB_FED, true);	
    EPWM_setFallingEdgeDelayCountShadowLoadMode(myEPWM0_BASE, EPWM_FED_LOAD_ON_CNTR_ZERO);	
    EPWM_disableFallingEdgeDelayCountShadowLoadMode(myEPWM0_BASE);	
    EPWM_setFallingEdgeDelayCount(myEPWM0_BASE, 80);	
    EPWM_setClockPrescaler(myEPWM2_BASE, EPWM_CLOCK_DIVIDER_1, EPWM_HSCLOCK_DIVIDER_2);	
    EPWM_setTimeBasePeriod(myEPWM2_BASE, 199);	
    EPWM_setTimeBaseCounter(myEPWM2_BASE, 0);	
    EPWM_setTimeBaseCounterMode(myEPWM2_BASE, EPWM_COUNTER_MODE_UP);	
    EPWM_disablePhaseShiftLoad(myEPWM2_BASE);	
    EPWM_setPhaseShift(myEPWM2_BASE, 0);	
    EPWM_setCounterCompareValue(myEPWM2_BASE, EPWM_COUNTER_COMPARE_A, 0);	
    EPWM_setCounterCompareShadowLoadMode(myEPWM2_BASE, EPWM_COUNTER_COMPARE_A, EPWM_COMP_LOAD_ON_CNTR_ZERO);	
    EPWM_setCounterCompareValue(myEPWM2_BASE, EPWM_COUNTER_COMPARE_B, 0);	
    EPWM_setCounterCompareShadowLoadMode(myEPWM2_BASE, EPWM_COUNTER_COMPARE_B, EPWM_COMP_LOAD_ON_CNTR_ZERO);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_A, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_ZERO);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPA);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPA);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_UP_CMPB);	
    EPWM_setActionQualifierAction(myEPWM2_BASE, EPWM_AQ_OUTPUT_B, EPWM_AQ_OUTPUT_NO_CHANGE, EPWM_AQ_OUTPUT_ON_TIMEBASE_DOWN_CMPB);	
    EPWM_setRisingEdgeDelayCountShadowLoadMode(myEPWM2_BASE, EPWM_RED_LOAD_ON_CNTR_ZERO);	
    EPWM_disableRisingEdgeDelayCountShadowLoadMode(myEPWM2_BASE);	
    EPWM_setFallingEdgeDelayCountShadowLoadMode(myEPWM2_BASE, EPWM_FED_LOAD_ON_CNTR_ZERO);	
    EPWM_disableFallingEdgeDelayCountShadowLoadMode(myEPWM2_BASE);	
    EPWM_enableADCTrigger(myEPWM2_BASE, EPWM_SOC_A);	
    EPWM_setADCTriggerSource(myEPWM2_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_PERIOD);	
    EPWM_setADCTriggerEventPrescale(myEPWM2_BASE, EPWM_SOC_A, 1);	
    EPWM_enableADCTrigger(myEPWM2_BASE, EPWM_SOC_B);	
    EPWM_setADCTriggerSource(myEPWM2_BASE, EPWM_SOC_B, EPWM_SOC_TBCTR_PERIOD);	
    EPWM_setADCTriggerEventPrescale(myEPWM2_BASE, EPWM_SOC_B, 1);	
}

//*****************************************************************************
//
// GPIO Configurations
//
//*****************************************************************************
void GPIO_init(){
	myBoardLED0_GPIO_init();
	myBoardLED1_GPIO_init();
}

void myBoardLED0_GPIO_init(){
	GPIO_setPadConfig(myBoardLED0_GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myBoardLED0_GPIO, GPIO_QUAL_SYNC);
	GPIO_setDirectionMode(myBoardLED0_GPIO, GPIO_DIR_MODE_OUT);
	GPIO_setControllerCore(myBoardLED0_GPIO, GPIO_CORE_CPU1);
}
void myBoardLED1_GPIO_init(){
	GPIO_setPadConfig(myBoardLED1_GPIO, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(myBoardLED1_GPIO, GPIO_QUAL_SYNC);
	GPIO_setDirectionMode(myBoardLED1_GPIO, GPIO_DIR_MODE_OUT);
	GPIO_setControllerCore(myBoardLED1_GPIO, GPIO_CORE_CPU1);
}

//*****************************************************************************
//
// INPUTXBAR Configurations
//
//*****************************************************************************
void INPUTXBAR_init(){
	myINPUTXBARINPUT0_init();
}

void myINPUTXBARINPUT0_init(){
	XBAR_setInputPin(myINPUTXBARINPUT0_INPUT, myINPUTXBARINPUT0_SOURCE);
}

//*****************************************************************************
//
// INTERRUPT Configurations
//
//*****************************************************************************
void INTERRUPT_init(){
	
	// Interrupt Settings for INT_myADC0_1
	// ISR need to be defined for the registered interrupts
	Interrupt_register(INT_myADC0_1, &adcA1ISR);
	Interrupt_enable(INT_myADC0_1);
	
	// Interrupt Settings for INT_myECAP0
	// ISR need to be defined for the registered interrupts
	Interrupt_register(INT_myECAP0, &ecap1ISR);
	Interrupt_enable(INT_myECAP0);
	
	// Interrupt Settings for INT_mySCIA_RX
	// ISR need to be defined for the registered interrupts
	Interrupt_register(INT_mySCIA_RX, &INT_mySCIA_RX_ISR);
	Interrupt_enable(INT_mySCIA_RX);
}
//*****************************************************************************
//
// SCI Configurations
//
//*****************************************************************************
void SCI_init(){
	mySCIA_init();
}

void mySCIA_init(){
	SCI_clearInterruptStatus(mySCIA_BASE, SCI_INT_RXFF | SCI_INT_TXFF | SCI_INT_FE | SCI_INT_OE | SCI_INT_PE | SCI_INT_RXERR | SCI_INT_RXRDY_BRKDT | SCI_INT_TXRDY);
	SCI_clearOverflowStatus(mySCIA_BASE);
	SCI_resetTxFIFO(mySCIA_BASE);
	SCI_resetRxFIFO(mySCIA_BASE);
	SCI_resetChannels(mySCIA_BASE);
	SCI_setConfig(mySCIA_BASE, DEVICE_LSPCLK_FREQ, mySCIA_BAUDRATE, (SCI_CONFIG_WLEN_8|SCI_CONFIG_STOP_ONE|SCI_CONFIG_PAR_NONE));
	SCI_disableLoopback(mySCIA_BASE);
	SCI_performSoftwareReset(mySCIA_BASE);
	SCI_enableInterrupt(mySCIA_BASE, SCI_INT_RXFF);
	SCI_setFIFOInterruptLevel(mySCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
	SCI_enableFIFO(mySCIA_BASE);
	SCI_enableModule(mySCIA_BASE);
}

//*****************************************************************************
//
// SYNC Scheme Configurations
//
//*****************************************************************************
void SYNC_init(){
	SysCtl_setSyncOutputConfig(SYSCTL_SYNC_OUT_SRC_EPWM1SYNCOUT);
	//
	// For EPWM1, the sync input is: SYSCTL_SYNC_IN_SRC_EXTSYNCIN1
	//
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM7, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_EPWM10, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_ECAP1, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	SysCtl_setSyncInputConfig(SYSCTL_SYNC_IN_ECAP4, SYSCTL_SYNC_IN_SRC_EPWM1SYNCOUT);
	//
	// SOCA
	//
	SysCtl_enableExtADCSOCSource(0);
	//
	// SOCB
	//
	SysCtl_enableExtADCSOCSource(0);
}
