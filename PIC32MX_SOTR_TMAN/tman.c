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

void tman_scheduler(void *pvParam){  
    TickType_t block_time = pdMS_TO_TICKS(TMAN_TICK_PERIOD);
    TickType_t last_time = 0;
    
    for(;;) {
        for (int i = 0; i < tman_control.new_task_index; i++){
            TMAN_TASK_HANDLER temp_handler = tman_handlers[i];
            int is_ready = tman_control.ticks % temp_handler.period == temp_handler.phase;
            is_ready = is_ready && (temp_handler.activations * temp_handler.deadline <= tman_control.ticks + temp_handler.phase);
            if (is_ready)
                xTaskNotifyGiveIndexed(tman_handlers[i].task_handle, 0);
        }
        vTaskDelayUntil(&last_time, block_time);
        tman_control.ticks++;
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

int notify_constr(char* task_id){
    int constr_index;
    int index = get_handler_index(task_id);
    
    if (index == TMAN_FAIL)
        return index;
    
    for(int i = 0; i < tman_handlers[index].num_constrs_give; i++){
        constr_index = get_handler_index(tman_handlers[index].constr_give[i]);
        xTaskNotifyGiveIndexed(tman_handlers[constr_index].task_handle, tman_handlers[index].constr_give_not_index[i]);
    }
    
    return TMAN_SUCCESS;
}

/*
 * Public Definitions
 */
int TMAN_INIT(){
    tman_control.new_task_index = 0;
    tman_control.print_queue = NULL;
    tman_control.ticks = 0;
    
    xTaskCreate(tman_scheduler, NULL, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(tman_printer, NULL, configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
    tman_control.print_queue = xQueueCreate(6, sizeof(char)*80);
    
    return TMAN_SUCCESS;
}

int TMAN_CLOSE(){
    vTaskStartScheduler();
    return TMAN_FAIL;
}

int TMAN_TASK_ADD(char* task_id, TaskFunction_t code, void* args, int priority){
    int index = tman_control.new_task_index;
    
    BaseType_t xReturned;
    
    xReturned = xTaskCreate(code, (const signed char* const) task_id, configMINIMAL_STACK_SIZE, (void*) args, tskIDLE_PRIORITY + priority, &tman_handlers[index].task_handle);    

    if( xReturned != pdPASS )
        return TMAN_FAIL;
    
    memcpy(&tman_handlers[index].task_id, task_id, 2);
    tman_handlers[index].last_activation_time = 0;
    tman_handlers[index].period = 0;
    tman_handlers[index].deadline = 0;
    tman_handlers[index].phase = 0;
    tman_handlers[index].activations = 0;
    tman_handlers[index].num_constrs_take = 0;
    tman_handlers[index].num_constrs_give = 0;
    tman_control.new_task_index++;
    
    return TMAN_SUCCESS;
}

int TMAN_TASK_REGISTER_ATTRIBUTES(char* task_id, int attr, void* value){
    int constr_index;
    int index = get_handler_index(task_id);
    if (index == TMAN_FAIL)
        return index;
    
    switch (attr){
        case TMAN_ATTR_PERIOD:
            tman_handlers[index].period = *((int *) value);
            break;

        case TMAN_ATTR_DEADLINE:
            tman_handlers[index].deadline = *((int *) value);
            break;
        
        case TMAN_ATTR_PHASE:
            tman_handlers[index].phase = *((int *) value);
            break;
        
        case TMAN_ATTR_CONSTR:
            constr_index = get_handler_index((char *) value);
            
            memcpy(tman_handlers[constr_index].constr_give[tman_handlers[constr_index].num_constrs_give], task_id, 2);
            tman_handlers[constr_index].constr_give_not_index[tman_handlers[constr_index].num_constrs_give] = tman_handlers[index].num_constrs_take+1;
            tman_handlers[constr_index].num_constrs_give++;
                    
            memcpy(tman_handlers[index].constr_take[tman_handlers[index].num_constrs_take], (char *) value, 2);
            tman_handlers[index].num_constrs_take++;
            break;
        default:
            break;
    }
    return TMAN_SUCCESS;
}

int TMAN_TASK_WAIT_PERIOD(char* task_id){
    int index = get_handler_index(task_id);
    
    if (index == TMAN_FAIL)
        return index;
    
    if(tman_handlers[index].activations)
        notify_constr(task_id);
    
    ulTaskNotifyTakeIndexed(0, pdTRUE, portMAX_DELAY);
    if(tman_handlers[index].num_constrs_take){
        for(int i = 0; i < tman_handlers[index].num_constrs_take; i++){
            ulTaskNotifyTakeIndexed(i+1, pdTRUE, portMAX_DELAY);
        }
    }
    
    tman_handlers[index].last_activation_time = xTaskGetTickCount();
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