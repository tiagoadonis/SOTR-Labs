#include "../UART/uart.h"
#include "../PIC32MX_SOTR_TMAN/tman.h"

void task_code(void *pvParam){
    int i, j, r = 0;
    
    TMAN_TASK_WAIT_PERIOD();
    printf("task id: %s ticks: %d", "placeholder", 0);
    
    for(i = 0; i < 10; i++)
        for(j = 0; j < 10; j++)
            r += i + j;
}

int tman_test(){
    char taskA_id[2] = "A";
    char taskB_id[2] = "B";
    char taskC_id[2] = "C";
    char taskD_id[2] = "D";
    char taskE_id[2] = "E";
    char taskF_id[2] = "F";
    
     // Set RA3 (LD4) as output
    TRISAbits.TRISA3 = 0;
    PORTAbits.RA3 = 0;
    
    // Initialize UART
    if(UartInit(configPERIPHERAL_CLOCK_HZ, 115200) != UART_SUCCESS) {
        PORTAbits.RA3 = 1;
        while(1);
    }
    
    /* Redirect stdin/stdout/stderr to UART1*/
     __XC_UART = 1;
    
    TMAN_INIT();
    TMAN_TASK_ADD(taskA_id, task_code, NULL);
    TMAN_TASK_ADD(taskB_id, task_code, NULL);
    TMAN_TASK_ADD(taskC_id, task_code, NULL);
    TMAN_TASK_ADD(taskD_id, task_code, NULL);
    TMAN_TASK_ADD(taskE_id, task_code, NULL);
    TMAN_TASK_ADD(taskF_id, task_code, NULL);
    
    TMAN_TASK_REGISTER_ATTRIBUTES(taskA_id, TMAN_ATTR_PERIOD, 1000);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskB_id, TMAN_ATTR_PERIOD, 1000);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskC_id, TMAN_ATTR_PERIOD, 1000);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskD_id, TMAN_ATTR_PERIOD, 1000);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskE_id, TMAN_ATTR_PERIOD, 1000);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskF_id, TMAN_ATTR_PERIOD, 1000);
    
    TMAN_CLOSE();
}