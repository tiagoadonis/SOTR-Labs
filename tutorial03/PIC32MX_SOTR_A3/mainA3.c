/*
 * David Rocha nº 84807
 * Tiago Adónis nº xxxxx
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
int last_value;
int average_value;

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
        
        // Read Value from ADC0
        last_value = 0;
        
        // Print value
        sprintf(mesg, "Acq value read: %d \n\r", last_value);
        PrintStr(mesg);    
        
        xTaskNotifyGive(xTaskProc);
    }
}

void pvProc(void *pvParam){
    uint8_t mesg[80];
    
    for(;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // Calculate average
        average_value = 0;
        
        // Print average
        sprintf(mesg, "Proc average: %d \n\r", average_value);   
        PrintStr(mesg); 
        
        xTaskNotifyGive(xTaskOut);
    }
}

void pvOut(void *pvParam){
    int temperature;
    uint8_t mesg[80];
    
    for(;;) {     
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        
        // Convert temperature to decimal
        temperature = average_value * 0;
        
        // Print average in decimal
        sprintf(mesg, "Out temperature: %d \n\r", temperature);   
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
