/*
 * David Rocha nº 84807
 * Tiago Adónis nº 88896
 * 
 * FREERTOS demo for ChipKit MAX32 board
 * - Creates two periodic tasks
 * - One toggles Led LD4, other is a long (interfering)task that 
 *      activates LD5 when executing 
 * - When the interfering task has higher priority interference becomes visible
 *      - LD4 does not blink at the right rate
 *
 * Environment:
 * - MPLAB X IDE v5.45
 * - XC32 V2.50
 * - FreeRTOS V202107.00
 *
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>
#include <xc.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* App includes */
#include "../UART/uart.h"

// Shared Memory
float last_value;
float average_value;

// Synchronization
static TaskHandle_t xTaskProc = NULL, xTaskOut = NULL;

/*
 * Prototypes and tasks
 */
void pvAcq(void *pvParam){
    TickType_t xLastWakeTime, xMaxBlockTime = pdMS_TO_TICKS(100);
    uint8_t mesg[80];
    
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    for(;;) {
        vTaskDelayUntil(&xLastWakeTime, xMaxBlockTime);
        
        // Blink LD4
        PORTAbits.RA3 = !PORTAbits.RA3;
        
        // Get one sample from ADC0
        IFS1bits.AD1IF = 0;          // Reset interrupt flag
        AD1CON1bits.ASAM = 1;        // Start conversion
        while (IFS1bits.AD1IF == 0); // Wait fo EOC

        // Convert to 0..3.3V 
        last_value = (ADC1BUF0 * 3.3) / 1023;
        
        // Print value
        sprintf(mesg, "Acq value read: %.2f \n\r", last_value);
        PrintStr(mesg);    
        
        xTaskNotifyGive(xTaskProc);
    }
}

void pvProc(void *pvParam){
    uint8_t mesg[80];
    float values[5] = {0, 0, 0, 0, 0};
    float sum;
    int index, activations = 0;
    
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        activations++;
        
        // Calculate average
        index  = activations % 4;
        values[index] =  last_value;
        
        sum = 0;
        for (int i = 0; i < 5; i++) {
            sum += values[i];
        }
        average_value = sum/5;
        
        // Print average
        sprintf(mesg, "Proc average: %.2f \n\r", average_value);   
        PrintStr(mesg); 
        
        xTaskNotifyGive(xTaskOut);
    }
}

void pvOut(void *pvParam){
    uint8_t mesg[80];
    
    for(;;) {     
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // Convert to temperature
        average_value = average_value * 100/3.3;
        
        // Print average in decimal
        sprintf(mesg, "Out temperature: %.2f\n\r", average_value);   
        PrintStr(mesg); 
    }
}

/*
 * Create the demo tasks then start the scheduler.
 */
int mainA3( void ){
    // Set RA3 (LD4) as output
    TRISAbits.TRISA3 = 0;
    PORTAbits.RA3 = 0;
    
    // Disable JTAG interface as it uses a few ADC ports
    DDPCONbits.JTAGEN = 0;
    
    // Initialize ADC module
    // Polling mode, AN0 as input
    // Generic part
    AD1CON1bits.SSRC = 7;       // Internal counter ends sampling and starts conversion
    AD1CON1bits.CLRASAM = 1;    //Stop conversion when 1st A/D converter interrupt is generated and clears ASAM bit automatically
    AD1CON1bits.FORM = 0;       // Integer 16 bit output format
    AD1CON2bits.VCFG = 0;       // VR+=AVdd; VR-=AVss
    AD1CON2bits.SMPI = 0;       // Number (+1) of consecutive conversions, stored in ADC1BUF0...ADCBUF{SMPI}
    AD1CON3bits.ADRC = 1;       // ADC uses internal RC clock
    AD1CON3bits.SAMC = 16;      // Sample time is 16TAD ( TAD = 100ns)
    // Set AN0 as input
    AD1CHSbits.CH0SA = 0;       // Select AN0 as input for A/D converter
    TRISBbits.TRISB0 = 1;       // Set AN0 to input mode
    AD1PCFGbits.PCFG0 = 0;      // Set AN0 to analog mode
    // Enable module
    AD1CON1bits.ON = 1;         // Enable A/D module (This must be the ***last instruction of configuration phase***)

	// Init UART and redirect stdin/stdot/stderr to UART
    if(UartInit(configPERIPHERAL_CLOCK_HZ, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 1; // If Led active error initializing UART
        while(1);
    }

     __XC_UART = 1; /* Redirect stdin/stdout/stderr to UART1*/
    
    /* Welcome message*/
    printf("\n\n ********************************\n\r");
    printf("Starting SOTR FreeRTOS Demo - A3\n\r");
    printf("*************************************\n\r");
    
    /* Create the tasks defined within this file. */
	xTaskCreate(pvAcq, (const signed char * const) "Acq", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate(pvProc,(const signed char * const) "Proc", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTaskProc );
    xTaskCreate(pvOut, (const signed char * const) "Out", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &xTaskOut );
    
    /* Finally start the scheduler. */
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}
