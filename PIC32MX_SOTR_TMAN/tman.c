#include "../PIC32MX_SOTR_TMAN/tman.h"

/*
 * Prototypes and tasks
 */
int TMAN_INIT(){
    
    return 0;
}

int TMAN_CLOSE(){
    
    return 0;
}

int TMAN_TASK_ADD(char* task_id, void* code, void* args){
    
    return 0;
}

int TMAN_TASK_REGISTER_ATTRIBUTES(char* task_id, int attr, int value){
    switch (attr){
        case TMAN_ATTR_PERIOD:
            // statements
            break;

        case TMAN_ATTR_DEADLINE:
            // statements
            break;
        
        case TMAN_ATTR_PHASE:
            // statements
            break;
        
        case TMAN_ATTR_CONSTR:
            // statements
            break;
        default:
            // default statements
            break;
    }
    return 0;
}

int TMAN_TASK_WAIT_PERIOD(){
    
    return 0;
}

int TMAN_TASK_STATS(char* task_id){
    
    return 0;
}