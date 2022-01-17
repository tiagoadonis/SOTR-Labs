#include "../UART/uart.h"
#include "../PIC32MX_SOTR_TMAN/tman.h"

void task_code(void *pvParam){
    TMAN_TASK_STATUS status_handler;
    int i, j, r = 0;
    char* id = (char *) pvParam;
    
    TMAN_TASK_WAIT_PERIOD(id);
    TMAN_TASK_STATS(&status_handler, id);
    
    printf("ID: %s Activations: %d\n\r", status_handler.task_id, status_handler.activations);
    
    for(i = 0; i < 3; i++)
        for(j = 0; j < 3; j++)
            r += i + j;
}

int tman_test(){
    char taskA_id[2] = "A";
    char taskB_id[2] = "B";
    char taskC_id[2] = "C";
    char taskD_id[2] = "D";
    char taskE_id[2] = "E";
    char taskF_id[2] = "F";
    
    /* Set RA3 (LD4) as output */
    TRISAbits.TRISA3 = 0;
    PORTAbits.RA3 = 0;
    
    /* Initialize UART */
    if(UartInit(configPERIPHERAL_CLOCK_HZ, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 1;
        while(1);
    }
    
    /* Redirect stdin/stdout/stderr to UART1 */
     __XC_UART = 1;
     
    /* Welcome message*/
    printf("*************************************\n\r");
    printf("Starting SOTR TMAN Demo\n\r");
    printf("*************************************\n\r");
    
    /* Initialize TMAN Framework */ 
    TMAN_INIT();
    printf("Initialized TMAN Successfully\n\r");
    
    /* Create TMAN Tasks */
    if(TMAN_TASK_ADD(taskA_id, task_code, (void*)taskA_id))
        printf("Error creating task %s\n\r", taskA_id);
    
    if(TMAN_TASK_ADD(taskB_id, task_code, (void*)taskB_id))
        printf("Error creating task %s\n\r", taskB_id);
    
    if(TMAN_TASK_ADD(taskC_id, task_code, (void*)taskC_id))
        printf("Error creating task %s\n\r", taskC_id);
    
    if(TMAN_TASK_ADD(taskD_id, task_code, (void*)taskD_id))
        printf("Error creating task %s\n\r", taskD_id);
    
    if(TMAN_TASK_ADD(taskE_id, task_code, (void*)taskE_id))
        printf("Error creating task %s\n\r", taskE_id);
    
    if(TMAN_TASK_ADD(taskF_id, task_code, (void*)taskF_id))
        printf("Error creating task %s\n\r", taskF_id);
    
    printf("Created TMAN Tasks Successfully\n\r");
    
    /* Set TMAN Tasks Period*/
    if(TMAN_TASK_REGISTER_ATTRIBUTES(taskA_id, TMAN_ATTR_PERIOD, 1000))
        printf("Error attributing period task %s\n\r", taskA_id);
    
    if(TMAN_TASK_REGISTER_ATTRIBUTES(taskB_id, TMAN_ATTR_PERIOD, 900))
        printf("Error attributing period task %s\n\r", taskB_id);
    
    if(TMAN_TASK_REGISTER_ATTRIBUTES(taskC_id, TMAN_ATTR_PERIOD, 800))
        printf("Error attributing period task %s\n\r", taskC_id);
    
    if(TMAN_TASK_REGISTER_ATTRIBUTES(taskD_id, TMAN_ATTR_PERIOD, 700))
        printf("Error attributing period task %s\n\r", taskD_id);
    
    if(TMAN_TASK_REGISTER_ATTRIBUTES(taskE_id, TMAN_ATTR_PERIOD, 600))
        printf("Error attributing period task %s\n\r", taskE_id);
    
    if(TMAN_TASK_REGISTER_ATTRIBUTES(taskF_id, TMAN_ATTR_PERIOD, 500))
        printf("Error attributing period task %s\n\r", taskF_id);
    
    printf("Registered TMAN Period Successfully\n\r");
    
    TMAN_CLOSE();
    printf("Closed TMAN Successfully\n\r");
    
    return 0;
}