#include "../PIC32MX_SOTR_TMAN/tman.h"

// Global Variables
static TMAN_TASK_HANDLER handlers[16];
static TMAN_STATUS status;

/*
 * Private Definitions
 */
int get_handler_index(char* id){
    int rv = TMAN_FAIL;
    for (int i = 0; i < status.new_task_index; i++){
        if (!strcmp(handlers[i].task_id, id))
            rv = i;
    }
    return rv;
}

/*
 * Public Definitions
 */
int TMAN_INIT(){
    status.new_task_index = 0;
    return TMAN_SUCCESS;
}

int TMAN_CLOSE(){
    vTaskStartScheduler();
    return TMAN_FAIL;
}

int TMAN_TASK_ADD(char* task_id, TaskFunction_t code, void* args){
    int index = status.new_task_index;
    
    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    
    xReturned = xTaskCreate(code, (const signed char* const) task_id, configMINIMAL_STACK_SIZE, (void*) args, tskIDLE_PRIORITY, &xHandle);    

    if( xReturned != pdPASS )
        return TMAN_FAIL;
    
    memcpy(&handlers[index].task_id, task_id, 2);
    handlers[index].last_activation_time = xTaskGetTickCount();
    status.new_task_index++;
    
    return TMAN_SUCCESS;
}

int TMAN_TASK_REGISTER_ATTRIBUTES(char* task_id, int attr, int value){
    int index = get_handler_index(task_id);
    if (index == TMAN_FAIL)
        return index;
    
    switch (attr){
        case TMAN_ATTR_PERIOD:
            handlers[index].period = pdMS_TO_TICKS(value);
            break;

        case TMAN_ATTR_DEADLINE:
            // TODO
            break;
        
        case TMAN_ATTR_PHASE:
            // TODO
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
    TickType_t period;
    TickType_t* last_time;
    int index = get_handler_index(task_id);
    
    if (index == TMAN_FAIL)
        return index;
    
    last_time = &handlers[index].last_activation_time;
    period = handlers[index].period;
    
    vTaskDelayUntil(last_time, period);
            
    return TMAN_SUCCESS;
}

int TMAN_TASK_STATS(TMAN_TASK_STATUS* status_handler, char* task_id){
    int index = get_handler_index(task_id);
    if (index == TMAN_FAIL)
        return index;
    
    memcpy(&status_handler->task_id, &handlers[index].task_id, 2);
    status_handler->activation_time = handlers[index].last_activation_time;
            
    return TMAN_SUCCESS;
}