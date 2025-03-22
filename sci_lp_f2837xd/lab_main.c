//############################################################################
//
// FILE: lab_main.c
//
// TITLE: Lab - SCI communication
//
// C2K ACADEMY URL: https://dev.ti.com/tirex/local?id=source_c2000_academy_labs_communications_lab_c2000_lab_sci&packageId=C2000-ACADEMY
//
//! \addtogroup academy_lab_list
//! <h1> Lab solution on Using Communication Peripherals </h1>
//!
//! The objective of this lab is to become familiar with the on-board SCI
//! (Serial Communication Interface) by sending and receiving data between a
//! C2000 device and a computer. We will use the computer to change the
//! frequency of the blinking LED and then the board will echo this value back
//! to the computer. This will allow us to demonstrate both means of
//! communication. Additionally, Code Composer Studio's terminal feature will
//! be explored and will be used to interact with the device.
//!
//! \b External \b Connections \n
//!  - None.
//!
//! \b Watch \b Variables \n
//!  - None.
//!
//############################################################################
// $Copyright:
// Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com
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
//############################################################################

#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "gpio.h"
#include "sci.h"
#include <string.h>
#include <stdlib.h>

// Definición de variables globales
#define BUFFER_SIZE 256  // Tamaño máximo del buffer
#define SUPUTAMADRE 1

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

const char* numbers[] = {
    "0\n", "1\n", "2\n", "3\n", "4\n", "5\n", "6\n", "7\n", "8\n", "9\n",
    "10\n", "11\n", "12\n", "13\n", "14\n", "15\n", "16\n"
};

__interrupt void INT_myCPUTIMER0_ISR(void)
{
    int randomNumber = rand() % 17;
    const char* numberStr = numbers[randomNumber];

    SCI_writeCharArray(mySCIA_BASE, (uint16_t*)numberStr, strlen(numberStr));
    SCI_writeCharArray(mySCI1_BASE, (uint16_t*)numberStr, strlen(numberStr));

    Interrupt_clearACKGroup(INT_myCPUTIMER0_INTERRUPT_ACK_GROUP);
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
    SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 65);

    int ledstatus = 1;
    GPIO_writePin(DEVICE_GPIO_PIN_LED2, ledstatus);

    msg = "\r\nInput: \0";
    SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 24);

    CPUTimer_startTimer(myCPUTIMER0_BASE);

    for (;;){
        if (flagRxComplete) {
            flagRxComplete = 0;  // Resetear la flag
            msg = "\r\nReceived: ";
            SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 12);
            
            SCI_writeCharArray(mySCIA_BASE, (uint16_t*)rxBuffer, strlen(rxBuffer));
            if (strcmp(rxBuffer, "LED") == 0) {
                ledstatus = !ledstatus;
                GPIO_writePin(DEVICE_GPIO_PIN_LED1, ledstatus);

                if (ledstatus) {
                    msg = "\r\nblue LED ON \0";
                } else {
                    msg = "\r\nblue LED OFF \0";
                }
                SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 25);

                
            }

            msg = "\r\nInput: \0";
            SCI_writeCharArray(mySCIA_BASE, (uint16_t*)msg, 24);
        }
    }

}
