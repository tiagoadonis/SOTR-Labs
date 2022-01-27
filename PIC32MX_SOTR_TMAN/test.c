#include "../UART/uart.h"
#include "../PIC32MX_SOTR_TMAN/tman.h"

void task_code(void *pvParam){
    TMAN_TASK_STATUS status_handler;
    int i, j, r = 0;
    char mesg[80];
    char* id = (char *) pvParam;
    
    for(;;) {
        TMAN_TASK_WAIT_PERIOD(id);
        TMAN_TASK_STATS(id, &status_handler);
        
        sprintf(mesg, "%s, %d\n\r", status_handler.task_id, status_handler.activation_time);
        TMAN_PRINT(mesg);
        
        for(i = 0; i < 100; i++)
            for(j = 0; j < 20; j++)
                r += i + j;
        
        TMAN_TASK_END(id);
    }
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
     
    /* Welcome message */
    printf("Starting SOTR TMAN Demo\n\r");
    printf("*************************************\n\r");
    
    /* Initialize TMAN Framework */ 
    TMAN_INIT();
    printf("Initialized TMAN.\n\r");
    
    /* Create TMAN Tasks */
    TMAN_TASK_ADD(taskA_id, task_code, (void*)taskA_id);
    TMAN_TASK_ADD(taskB_id, task_code, (void*)taskB_id);
    TMAN_TASK_ADD(taskC_id, task_code, (void*)taskC_id);
    TMAN_TASK_ADD(taskD_id, task_code, (void*)taskD_id);
    TMAN_TASK_ADD(taskE_id, task_code, (void*)taskE_id);
    TMAN_TASK_ADD(taskF_id, task_code, (void*)taskF_id);
    printf("Created TMAN Tasks.\n\r");
    
    /* Set TMAN Tasks Periods */
    int period_a = 2, period_b = 2;
    int period_c = 4;
    int period_d = 6, period_e = 6, period_f = 6;
    TMAN_TASK_REGISTER_ATTRIBUTES(taskA_id, TMAN_ATTR_PERIOD, &period_a);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskB_id, TMAN_ATTR_PERIOD, &period_b);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskC_id, TMAN_ATTR_PERIOD, &period_c);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskD_id, TMAN_ATTR_PERIOD, &period_d);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskE_id, TMAN_ATTR_PERIOD, &period_e);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskF_id, TMAN_ATTR_PERIOD, &period_f);
    printf("Registered TMAN Periods.\n\r");
    
    /* Set TMAN Tasks Phases */
    int phase_a = 0, phase_b = 1;
    int phase_c = 0;
    int phase_d = 0, phase_e = 0, phase_f = 0;
    TMAN_TASK_REGISTER_ATTRIBUTES(taskA_id, TMAN_ATTR_PHASE, &phase_a);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskB_id, TMAN_ATTR_PHASE, &phase_b);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskC_id, TMAN_ATTR_PHASE, &phase_c);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskD_id, TMAN_ATTR_PHASE, &phase_d);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskE_id, TMAN_ATTR_PHASE, &phase_e);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskF_id, TMAN_ATTR_PHASE, &phase_f);
    printf("Registered TMAN Phases.\n\r"); 
    
    /* Set TMAN Tasks Deadlines */
    int deadline_a = 4, deadline_b = 2;
    int deadline_c = 4;
    int deadline_d = 6, deadline_e = 6, deadline_f = 6;
    TMAN_TASK_REGISTER_ATTRIBUTES(taskA_id, TMAN_ATTR_DEADLINE, &deadline_a);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskB_id, TMAN_ATTR_DEADLINE, &deadline_b);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskC_id, TMAN_ATTR_DEADLINE, &deadline_c);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskD_id, TMAN_ATTR_DEADLINE, &deadline_d);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskE_id, TMAN_ATTR_DEADLINE, &deadline_e);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskF_id, TMAN_ATTR_DEADLINE, &deadline_f);
    printf("Registered TMAN Deadlines.\n\r");
    
    /* Set TMAN Tasks Constraints */
    char constraint_E[2] = "F";
    char constraint_E_2[2] = "D";
    TMAN_TASK_REGISTER_ATTRIBUTES(taskE_id, TMAN_ATTR_CONSTR, &constraint_E);
    TMAN_TASK_REGISTER_ATTRIBUTES(taskE_id, TMAN_ATTR_CONSTR, &constraint_E_2);
    printf("Registered TMAN Constraints.\n\r");
    
    return TMAN_CLOSE();
}