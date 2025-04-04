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
#include "epwm.h"
//#include <cstdint>
#include <stdint.h> 
#include <string.h>

extern uint32_t _RamfuncsLoadStart, _RamfuncsLoadEnd, _RamfuncsRunStart;

uint32_t ePwm_TimeBase;
uint32_t ePwm_MinDuty;
uint32_t ePwm_MaxDuty;
uint32_t ePwm_curDuty;

uint16_t rxStatus;

uint16_t AdcBuf[50];            // Buffer to store ADC samples.
uint16_t AdcBufB[50];           // Buffer to store B ADC samples
uint16_t *AdcBufPtr = AdcBuf;   // Pointer to ADC buffer samples.
uint16_t *AdcBufPtrB = AdcBufB;
uint16_t LedCtr = 0;            // Counter to slow down LED toggle in ADC ISR.
uint16_t DutyModOn = 0;         // Flag to turn on/off duty cycle modulation.
uint16_t DutyModDir = 0;        // Flag to control duty mod direction up/down.
uint16_t DutyModCtr = 0;        // Counter to slow down rate of modulation.

// Definición de variables globales
#define BUFFER_SIZE 256  // Tamaño máximo del buffer
#define DELAY_CNT 5

char rxBuffer[BUFFER_SIZE];  // Buffer para almacenar la cadena recibida
volatile uint16_t rxIndex = 0; // Índice del buffer
volatile uint8_t flagRxComplete = 0; // Flag para indicar que se recibió un mensaje completo


#pragma CODE_SECTION(INT_mySCIA_RX_ISR, ".TI.ramfunc")
#pragma CODE_SECTION(EPWM_setTimeBasePeriod, ".TI.ramfunc")
#pragma CODE_SECTION(EPWM_setCounterCompareValue, ".TI.ramfunc")
#pragma CODE_SECTION(EPWM_setPhaseShift, ".TI.ramfunc")
#pragma CODE_SECTION(extractFrequency, ".TI.ramfunc")
#pragma CODE_SECTION(extractDelay, ".TI.ramfunc")
#pragma CODE_SECTION(extractDeadp1, ".TI.ramfunc")
#pragma CODE_SECTION(extractDeadp2, ".TI.ramfunc")
#pragma CODE_SECTION(extractDeads1, ".TI.ramfunc")
#pragma CODE_SECTION(extractDeads2, ".TI.ramfunc")
#pragma CODE_SECTION(extractSync, ".TI.ramfunc")
// Interrupt SCI RX

char receivedChar;
__interrupt void INT_mySCIA_RX_ISR(void) {
    
    
    receivedChar = SCI_readCharBlockingFIFO(mySCIA_BASE);

    rxStatus = SCI_getRxStatus(mySCIA_BASE);
            if((rxStatus & SCI_RXSTATUS_ERROR) != 0)
            {
                //
                //If Execution stops here there is some error
                //Analyze SCI_getRxStatus() API return value
                //
                ESTOP0;
            }

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


void delayCycles(uint32_t cycles) {
    while(cycles--) {
        __asm(" NOP");
    }
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

int extractDelay(const char *str, int *delay) {
    const char *prefix = "PS:";
    int i = 0;
    
    // Check prefix (only 3 characters for "PS:")
    for (i = 0; i < 3; ++i) {
        if (str[i] != prefix[i]) {
            return 0;
        }
    }

    int num = 0;
    // Start extracting from index 3 (right after "PS:")
    for (i = 3; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') {
            num = num * 10 + (str[i] - '0');
        } else {
            break;
        }
    }
    *delay = num;
    return 1;
}

int extractDeadp1(const char *str, int *dead) {
    const char *prefix = "DP1:";
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
    *dead = num;
    return  1;
}

int extractDeadp2(const char *str, int *dead) {
    const char *prefix = "DP2:";
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
    *dead = num;
    return  1;
}

int extractDeads1(const char *str, int *dead) {
    const char *prefix = "DS1:";
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
    *dead = num;
    return  1;
}

int extractDeads2(const char *str, int *dead) {
    const char *prefix = "DS2:";
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
    *dead = num;
    return  1;
}

int extractSync(const char *str, int *sync) {
    const char *prefix = "SYNC:";
    int i = 0;
    for (i = 0; i < 5; ++i) {
        if (str[i] != prefix[i]) {
            return 0;
        }
    }

    int num = 0;
    for (i = 5; str[i] != '\0'; i++) {
        if (str[i] >= '0' && str[i] <= '9') { // Verificar si es un dígito
            num = num * 10 + (str[i] - '0');   // Convertir a entero
        } else {
            break; // Si encontramos un carácter no numérico, salimos
        }
    }
    *sync = num;
    return  1;

}


void setPWMOutputState(uint32_t base, bool enable) {
    if (enable) {
        // Reactivar el PWM deshabilitando la condición de Trip Zone
        EPWM_clearTripZoneFlag(base, EPWM_TZ_INTERRUPT);  // Limpiar banderas
        EPWM_clearTripZoneFlag(base, EPWM_TZ_FLAG_OST);   // Limpiar evento de One-Shot
    } else {
        // Configurar Trip Zone para forzar ambas salidas a LOW
        EPWM_setTripZoneAction(base, EPWM_TZ_ACTION_EVENT_TZA, EPWM_TZ_ACTION_LOW);
        EPWM_setTripZoneAction(base, EPWM_TZ_ACTION_EVENT_TZB, EPWM_TZ_ACTION_LOW);

        // Forzar un evento de Trip Zone
        EPWM_forceTripZoneEvent(base, EPWM_TZ_FORCE_EVENT_OST); // One-Shot Trip
    }
}

//
// Main
//
void main(void)
{
    memcpy(&_RamfuncsRunStart, &_RamfuncsLoadStart, (size_t)&_RamfuncsLoadEnd - (size_t)&_RamfuncsLoadStart);
    Device_init();
    Interrupt_initModule();
    Interrupt_initVectorTable();

    Board_init();

    // Initialize variables for ePWM Duty Cycle
    ePwm_TimeBase = EPWM_getTimeBasePeriod(myEPWM0_BASE);
    int ePwm_TimeBase_init = EPWM_getTimeBasePeriod(myEPWM0_BASE);
    ePwm_MinDuty = (uint32_t)(0.85f * (float)ePwm_TimeBase);
    ePwm_MaxDuty = (uint32_t)(0.15f * (float)ePwm_TimeBase);
    ePwm_curDuty = EPWM_getCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A);

    EPWM_setPhaseShift(myEPWM2_BASE, 499);
    EPWM_setPhaseShift(myEPWM3_BASE, 499);
    EPWM_setPhaseShift(myEPWM4_BASE, 499);

    setPWMOutputState(myEPWM0_BASE, false);
    setPWMOutputState(myEPWM1_BASE, false);
    setPWMOutputState(myEPWM2_BASE, false);
    setPWMOutputState(myEPWM3_BASE, false);



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
    int delay = 0;
    int dead = 100;
    int sync_rect = 499;

    uint32_t numerador;
    uint32_t denominador;
    uint16_t phaseshift;
    uint16_t sync_delay;


    for (;;) {


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
                uint32_t previousPeriod = ePwm_TimeBase; // Guardar el periodo anterior
                
                // Determinar el orden de configuración según si el nuevo periodo es mayor o menor
                if(frequency > previousPeriod) {
                    // Nuevo periodo mayor: primero TimeBase, luego Compare
                    EPWM_setTimeBasePeriod(myEPWM0_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM1_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM2_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM3_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM4_BASE, frequency);
                    
                    ePwm_TimeBase = frequency; // Actualizar el valor actual
                    
                    uint32_t newCompareValue = (ePwm_TimeBase + 1) / 2 - 1;
                    EPWM_setCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM1_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM2_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM3_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM4_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                } else {
                    // Nuevo periodo menor: primero Compare, luego TimeBase
                    uint32_t newCompareValue = (frequency + 1) / 2 - 1;
                    EPWM_setCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM1_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM2_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM3_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    EPWM_setCounterCompareValue(myEPWM4_BASE, EPWM_COUNTER_COMPARE_A, newCompareValue);
                    
                    EPWM_setTimeBasePeriod(myEPWM0_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM1_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM2_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM3_BASE, frequency);
                    EPWM_setTimeBasePeriod(myEPWM4_BASE, frequency);
                    
                    ePwm_TimeBase = frequency; // Actualizar el valor actual
                }
                
                ePwm_curDuty = EPWM_getCounterCompareValue(myEPWM0_BASE, EPWM_COUNTER_COMPARE_A);   

                // Configuración de los phase shifts (igual que antes)
                numerador = (uint32_t)delay * ePwm_TimeBase * 2;
                denominador = ePwm_TimeBase_init;
                phaseshift = (uint16_t)(numerador / denominador);
                EPWM_setPhaseShift(myEPWM1_BASE, -phaseshift);    

                numerador = (uint32_t)(sync_rect + 1) * (ePwm_TimeBase + 1);
                denominador = (ePwm_TimeBase_init + 1);
                sync_delay = (uint16_t)(numerador / denominador) - 1;

                EPWM_setPhaseShift(myEPWM2_BASE, sync_delay);
                EPWM_setPhaseShift(myEPWM3_BASE, sync_delay);
                EPWM_setPhaseShift(myEPWM4_BASE, sync_delay); 
                
                EPWM_forceSyncPulse(myEPWM0_BASE);      

            }

            else if (extractDelay(rxBuffer, &delay)){
                ePwm_TimeBase = EPWM_getTimeBasePeriod(myEPWM0_BASE);
                numerador = (uint32_t)((delay) * (ePwm_TimeBase + 1)) * 2;  // Evita overflow
                denominador = ePwm_TimeBase_init;                   // Divisor en un solo paso
                phaseshift = (uint16_t)(numerador / denominador);  // División segura
                EPWM_setPhaseShift(myEPWM1_BASE, - phaseshift);  
            }

            else if (extractDeadp1(rxBuffer, &dead)) {
                EPWM_setRisingEdgeDelayCount(myEPWM0_BASE, (uint16_t) dead);	

                EPWM_setRisingEdgeDelayCount(myEPWM1_BASE, (uint16_t) dead);	
            }

            else if (extractDeadp2(rxBuffer, &dead)) {	
                EPWM_setFallingEdgeDelayCount(myEPWM0_BASE, (uint16_t) dead);	

                EPWM_setFallingEdgeDelayCount(myEPWM1_BASE, (uint16_t) dead);	
            }

            else if (extractDeads1(rxBuffer, &dead)) {
                EPWM_setRisingEdgeDelayCount(myEPWM2_BASE, (uint16_t) dead);	

                EPWM_setRisingEdgeDelayCount(myEPWM3_BASE, (uint16_t) dead);

                EPWM_setRisingEdgeDelayCount(myEPWM4_BASE, (uint16_t) dead);	
            }

            else if (extractDeads2(rxBuffer, &dead)) {	
                EPWM_setFallingEdgeDelayCount(myEPWM2_BASE, (uint16_t) dead);	

                EPWM_setFallingEdgeDelayCount(myEPWM3_BASE, (uint16_t) dead);

                EPWM_setFallingEdgeDelayCount(myEPWM4_BASE, (uint16_t) dead);	
            }

            else if (extractSync(rxBuffer, &sync_rect)) {
                numerador = (uint32_t)(sync_rect + 1) * (ePwm_TimeBase + 1);
                denominador = (ePwm_TimeBase_init + 1);
                sync_delay = (uint16_t)(numerador / denominador) - 1;

                EPWM_setPhaseShift(myEPWM2_BASE, sync_delay);
                EPWM_setPhaseShift(myEPWM3_BASE, sync_delay); 
                EPWM_setPhaseShift(myEPWM4_BASE, sync_delay);       

            
            }

            // Enable/disable PWM outputs:
            else if (strcmp(rxBuffer, "pwmpon") == 0) {
                setPWMOutputState(myEPWM0_BASE, true);
                setPWMOutputState(myEPWM1_BASE, true);
                msg = "\r\nPWM outputs ENABLED (SW force disabled)\0";
                SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 43);
            }
            else if (strcmp(rxBuffer, "pwmpoff") == 0) {
                setPWMOutputState(myEPWM0_BASE, false);
                setPWMOutputState(myEPWM1_BASE, false);
                msg = "\r\nPWM outputs DISABLED (forced LOW)\0";
                SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 37);
            }
            else if (strcmp(rxBuffer, "pwmson") == 0) {
                setPWMOutputState(myEPWM2_BASE, true);
                setPWMOutputState(myEPWM3_BASE, true);
                msg = "\r\nPWM outputs ENABLED (SW force disabled)\0";
                SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 43);
            }
            else if (strcmp(rxBuffer, "pwmsoff") == 0) {
                setPWMOutputState(myEPWM2_BASE, false);
                setPWMOutputState(myEPWM3_BASE, false);
                msg = "\r\nPWM outputs DISABLED (forced LOW)\0";
                SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 37);
            }

            msg = "\r\nInput: \0";
            SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 11);
        }
    }
    
}

//
// End of File
//

