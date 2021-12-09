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
#include "queue.h"

/* App includes */
#include "../UART/uart.h"

// IPC
QueueHandle_t xProcQueue = NULL;
QueueHandle_t xOutQueue = NULL;

/*
 * Prototypes and tasks
 */
void pvAcq(void *pvParam){
    TickType_t xLastWakeTime, xMaxBlockTime = pdMS_TO_TICKS(100);
    uint8_t mesg[80];
    int last_value;
    
    // Initialize the xLastWakeTime variable with the current time.
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
        
        if (xQueueSend(xProcQueue, (void *)&last_value, (TickType_t )0) != pdTRUE) {
            PrintStr("Acq error");
        }
    }
}

void pvProc(void *pvParam){
    uint8_t mesg[80];
    int average_value, queue_value;
    
    for(;;) {
        if (xQueueReceive(xProcQueue, (void *)&queue_value, portMAX_DELAY) == pdTRUE) {
            // Calculate average
            average_value = queue_value * 0;

            // Print average
            sprintf(mesg, "Proc average: %d \n\r", average_value);   
            PrintStr(mesg); 

            if(xQueueSend(xOutQueue, (void *) &average_value, (TickType_t )0) != pdTRUE) {
                PrintStr("Proc error"); 
            }
        }
    }
}

void pvOut(void *pvParam){
    int temperature, queue_value;
    uint8_t mesg[80];
    
    for(;;) {     
        if (xQueueReceive(xOutQueue, (void *)&(queue_value), portMAX_DELAY) == pdTRUE) {
            // Convert temperature to decimal
            temperature = queue_value * 0;

            // Print average in decimal
            sprintf(mesg, "Out temperature: %d \n\r", temperature);   
            PrintStr(mesg); 
        }
    }
}

/*
 * Create the demo tasks then start the scheduler.
 */
int mainA4( void ){
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
    printf("Starting SOTR FreeRTOS Demo - A4\n\r");
    printf("*************************************\n\r");
    
    // Initialize Queues
    xProcQueue = xQueueCreate(2, sizeof(int));
    xOutQueue = xQueueCreate(2, sizeof(int));
    
    /* Create the tasks defined within this file. */
	xTaskCreate(pvAcq, (const signed char * const) "Acq", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate(pvProc,(const signed char * const) "Proc", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    xTaskCreate(pvOut, (const signed char * const) "Out", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
    
    /* Finally start the scheduler. */
	vTaskStartScheduler();

	/* Will only reach here if there is insufficient heap available to start
	the scheduler. */
	return 0;
}
