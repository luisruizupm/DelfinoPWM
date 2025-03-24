//#############################################################################
//
// FILE:   lab_main.c
//
// TITLE:  ePWM/eCAP Lab exercise
//
// C2K ACADEMY URL: https://dev.ti.com/tirex/local?id=source_c2000_academy_labs_control_peripherals_c2000_lab_pwm&packageId=C2000-ACADEMY
//
// This lab configures ePWM at 1kHz, and eCAP to capture the events
// to measure pwm duty cycle
//
//#############################################################################
//
//
// $Copyright:
// Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################


//
// Included Files
//
#include "adc.h"
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include <string.h>

uint32_t ePwm_TimeBase;
uint32_t ePwm_MinDuty;
uint32_t ePwm_MaxDuty;
uint32_t ePwm_curDuty;

uint16_t AdcBuf[50];            // Buffer to store ADC samples.
uint16_t AdcBufB[50];           // Buffer to store B ADC samples
uint16_t *AdcBufPtr = AdcBuf;   // Pointer to ADC buffer samples.
uint16_t *AdcBufPtrB = AdcBufB;
uint16_t LedCtr = 0;            // Counter to slow down LED toggle in ADC ISR.
uint16_t DutyModOn = 0;         // Flag to turn on/off duty cycle modulation.
uint16_t DutyModDir = 0;        // Flag to control duty mod direction up/down.
uint16_t DutyModCtr = 0;        // Counter to slow down rate of modulation.
int32_t eCapPwmDuty;            // Percent = (eCapPwmDuty/eCapPwmPeriod)*100.
int32_t eCapPwmPeriod;          // Frequency = DEVICE_SYSCLK_FREQ/eCapPwmPeriod.

// Definición de variables globales
#define BUFFER_SIZE 256  // Tamaño máximo del buffer

char rxBuffer[BUFFER_SIZE];  // Buffer para almacenar la cadena recibida
volatile uint16_t rxIndex = 0; // Índice del buffer
volatile uint8_t flagRxComplete = 0; // Flag para indicar que se recibió un mensaje completo

// Interrupt SCI RX
__interrupt void INT_mySCIA_RX_ISR(void) {
    
    
    char receivedChar = SCI_readCharBlockingFIFO(mySCIA_BASE);

    // Depuración: enviar el carácter recibido de vuelta
    SCI_writeCharBlockingNonFIFO(mySCIA_BASE, receivedChar);

    if (receivedChar == '\n' || receivedChar == '\r') {  
        rxBuffer[rxIndex] = '\0';  // Terminar la cadena
        flagRxComplete = 1;  // Activar la flag de mensaje completo
        SCI_writeCharArray(mySCIA_BASE, (uint16_t*)rxBuffer, rxIndex);
        rxIndex = 0;  // Reiniciar el índice
        
    } else {
        if (rxIndex < BUFFER_SIZE - 1) {  
            rxBuffer[rxIndex++] = receivedChar;
        } else {
            // Error: Buffer lleno, reiniciar
            rxIndex = 0;
        }
    }

    // Limpiar flags de interrupción
    SCI_clearInterruptStatus(mySCIA_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INT_mySCIA_RX_INTERRUPT_ACK_GROUP);
}

__interrupt void adcA1ISR(void)
{
    // Clear interrupt flags.
    Interrupt_clearACKGroup(INT_myADC0_1_INTERRUPT_ACK_GROUP);
    ADC_clearInterruptStatus(myADC0_BASE, ADC_INT_NUMBER1);

    //Interrupt_clearACKGroup(INT_myADC1_1_INTERRUPT_ACK_GROUP);
    ADC_clearInterruptStatus(myADC1_BASE, ADC_INT_NUMBER1);
    // Write contents of the ADC register to a circular buffer.
    *AdcBufPtr = ADC_readResult(myADC0_RESULT_BASE, myADC0_SOC0);
    *AdcBufPtrB = ADC_readResult(myADC1_RESULT_BASE, myADC1_SOC0);
    if (AdcBufPtr == (AdcBuf + 49))
    {
        // Force buffer to wrap around.
        AdcBufPtr = AdcBuf;
        AdcBufPtrB = AdcBufB;
    } else {
        AdcBufPtr += 1;
        AdcBufPtrB += 1;
    }
    if (LedCtr >= 49999) {
        // Divide 50kHz sample rate by 50e3 to toggle LED at a rate of 1Hz.
        GPIO_togglePin(myBoardLED0_GPIO);
        LedCtr = 0;
    } else {
        LedCtr += 1;
    }
    if (DutyModOn) {
        // Divide 50kHz sample rate by 16 to slow down duty modulation.
        if (DutyModCtr >= 15) {
            if (DutyModDir == 0) {
                // Increment State => Decrease Duty Cycle.
                if (ePwm_curDuty >= ePwm_MinDuty) {
                    DutyModDir = 1;
                } else {
                    ePwm_curDuty += 1;
                }
            } else {
                // Decrement State => Increase Duty Cycle.
                if (ePwm_curDuty <= ePwm_MaxDuty) {
                    DutyModDir = 0;
                } else {
                    ePwm_curDuty -= 1;
                }
            }
            DutyModCtr = 0;
        } else {
            DutyModCtr += 1;
        }
    }
    // Set the counter compare value.
    EPWM_setCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A, ePwm_curDuty);
}


__interrupt void ecap1ISR(void)
{
    Interrupt_clearACKGroup(INT_myECAP0_INTERRUPT_ACK_GROUP);
    ECAP_clearGlobalInterrupt(myECAP0_BASE);
    ECAP_clearInterrupt(myECAP0_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_3);
    eCapPwmDuty = (int32_t)ECAP_getEventTimeStamp(myECAP0_BASE, ECAP_EVENT_2) -
                  (int32_t)ECAP_getEventTimeStamp(myECAP0_BASE, ECAP_EVENT_1);
    eCapPwmPeriod = (int32_t)ECAP_getEventTimeStamp(myECAP0_BASE, ECAP_EVENT_3) -
                    (int32_t)ECAP_getEventTimeStamp(myECAP0_BASE, ECAP_EVENT_1);
}

int extractFrequency(const char *str, int *frequency) {
    const char *prefix = "PER:";
    int i = 0;
    for (i = 0; i < 4; ++i) {
        if (str[i] != prefix[i]) {
            return 0;
        }
    }

    int num = 0;
    for (i = 4; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') { // Verificar si es un dígito
            num = num * 10 + (str[i] - '0');   // Convertir a entero
        } else {
            break; // Si encontramos un carácter no numérico, salimos
        }
    }
    *frequency = num;
    return  1;

}

//
// Main
//
void main(void)
{
    Device_init();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    Board_init();

    // Initialize variables for ePWM Duty Cycle
    ePwm_TimeBase = EPWM_getTimeBasePeriod(myEPWM0_BASE);
    ePwm_MinDuty = (uint32_t)(0.85f * (float)ePwm_TimeBase);
    ePwm_MaxDuty = (uint32_t)(0.15f * (float)ePwm_TimeBase);
    ePwm_curDuty = EPWM_getCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A);

    EINT;
    ERTM;

    // Define local variables
    //
    char* msg;                // Message sent through terminal window
    //uint16_t rxStatus = 0U;   // Variable used to store the status of the SCI RX Register

    //
    // Send starting message.
    //
    msg = "\r\n\n\nHello World! Enter a number 0-9 to change the LED blink rate, or led.\0";
    SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 75);

    int ledstatus = 1;
    GPIO_writePin(DEVICE_GPIO_PIN_LED2, ledstatus);

    msg = "\r\nInput: \0";
    SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 11);

    int frequency;


    for (;;) {
        // Comprobar errores
        //rxStatus = SCI_getRxStatus(mySCIA_BASE);
        //if ((rxStatus & SCI_RXSTATUS_ERROR) != 0) {
        //    ESTOP0; // Detener ejecución si hay un error
        //}

        // Verificar si la interrupción ha recibido una cadena completa
        if (flagRxComplete) {
            flagRxComplete = 0;  // Resetear la flag
            msg = "\r\nReceived: ";
            SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 13);
            
            SCI_writeCharArray(mySCIA_BASE, (uint16_t*)rxBuffer, strlen(rxBuffer));


            if (strcmp(rxBuffer, "led") == 0) {
                ledstatus = !ledstatus;
                GPIO_writePin(DEVICE_GPIO_PIN_LED1, ledstatus);

                if (ledstatus) {
                    msg = "\r\nblue LED ON \0";
                } else {
                    msg = "\r\nblue LED OFF \0";
                }
                SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 17);

                
            }

            else if (extractFrequency(rxBuffer, &frequency)) {

                EPWM_setTimeBasePeriod(myEPWM0_BASE, frequency);

                ePwm_TimeBase = EPWM_getTimeBasePeriod(myEPWM0_BASE);
                
                EPWM_setCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A, (ePwm_TimeBase + 1 ) / 2 - 1);	

                ePwm_curDuty = EPWM_getCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A);                

            }

            msg = "\r\nInput: \0";
            SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 11);
        }
    }
    
}

//
// End of File
//

