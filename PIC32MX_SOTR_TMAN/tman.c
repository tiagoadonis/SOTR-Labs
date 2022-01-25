#include "../PIC32MX_SOTR_TMAN/tman.h"

// Global Variables
static TMAN_TASK_HANDLER tman_handlers[16];
static TMAN_CONTROL tman_control;

/*
 * Control Tasks
 */
void tman_printer(void *pvParam){  
    char log[80];
    for(;;) {
        if (xQueueReceive(tman_control.print_queue, (void *)log, portMAX_DELAY) == pdTRUE) {
            printf(log);
        }
    }
}

/*
 * Private Definitions
 */
int get_handler_index(char* id){
    int rv = TMAN_FAIL;
    for (int i = 0; i < tman_control.new_task_index; i++){
        if (!strcmp(tman_handlers[i].task_id, id))
            rv = i;
    }
    return rv;
}

/*
 * Public Definitions
 */
int TMAN_INIT(){
    tman_control.new_task_index = 0;
    tman_control.print_queue = NULL;
    
    xTaskCreate(tman_printer, NULL, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    tman_control.print_queue = xQueueCreate(6, sizeof(char)*80);
    
    return TMAN_SUCCESS;
}

int TMAN_CLOSE(){
    vTaskStartScheduler();
    return TMAN_FAIL;
}

int TMAN_TASK_ADD(char* task_id, TaskFunction_t code, void* args){
    int index = tman_control.new_task_index;
    
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    
    xReturned = xTaskCreate(code, (const signed char* const) task_id, configMINIMAL_STACK_SIZE, (void*) args, tskIDLE_PRIORITY, &xHandle);    

    if( xReturned != pdPASS )
        return TMAN_FAIL;
    
    memcpy(&tman_handlers[index].task_id, task_id, 2);
    tman_handlers[index].last_activation_time = xTaskGetTickCount();
    tman_handlers[index].period = 0;
    tman_handlers[index].deadline = 0;
    tman_handlers[index].phase = 0;
    tman_handlers[index].activations = 0;
    tman_control.new_task_index++;
    
    return TMAN_SUCCESS;
}

int TMAN_TASK_REGISTER_ATTRIBUTES(char* task_id, int attr, int value){
    int index = get_handler_index(task_id);
    if (index == TMAN_FAIL)
        return index;
    
    switch (attr){
        case TMAN_ATTR_PERIOD:
            tman_handlers[index].period = pdMS_TO_TICKS(value * TMAN_TICK_PERIOD);
            break;

        case TMAN_ATTR_DEADLINE:
            tman_handlers[index].deadline = pdMS_TO_TICKS(value * TMAN_TICK_PERIOD);
            break;
        
        case TMAN_ATTR_PHASE:
            tman_handlers[index].phase = pdMS_TO_TICKS(value * TMAN_TICK_PERIOD);
            break;
        
        case TMAN_ATTR_CONSTR:
            // TODO
            break;
        default:
            // default statements
            break;
    }
    return TMAN_SUCCESS;
}

int TMAN_TASK_WAIT_PERIOD(char* task_id){
    TickType_t period, phase, deadline;
    TickType_t* last_time;
    int activations; 
    
    int index = get_handler_index(task_id);
    
    if (index == TMAN_FAIL)
        return index;
    
    last_time = &tman_handlers[index].last_activation_time;
    period = tman_handlers[index].period;
    phase = tman_handlers[index].phase;
    deadline = tman_handlers[index].deadline;
    activations = tman_handlers[index].activations;
    
    if(!activations){
        vTaskDelayUntil(last_time, period + phase);
    }
    else
        vTaskDelayUntil(last_time, period);
    
    tman_handlers[index].activations++;
    return TMAN_SUCCESS;
}

int TMAN_TASK_STATS(char* task_id, TMAN_TASK_STATUS* status_handler){
    int index = get_handler_index(task_id);
    if (index == TMAN_FAIL)
        return index;
    
    memcpy(&status_handler->task_id, &tman_handlers[index].task_id, 2);
    status_handler->activation_time = tman_handlers[index].last_activation_time / TMAN_TICK_PERIOD;
            
    return TMAN_SUCCESS;
}

int TMAN_PRINT(char* msg){ 
    if(xQueueSend(tman_control.print_queue, (void *) msg, (TickType_t )0) != pdTRUE)
        return TMAN_FAIL; 
    return TMAN_SUCCESS;
}