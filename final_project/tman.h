#ifndef __TMAN_H__
#define __TMAN_H__

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <xc.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Define return codes
#define TMAN_SUCCESS 0
#define TMAN_FAIL -1

// Define attribute codes
#define TMAN_ATTR_PERIOD 1
#define TMAN_ATTR_DEADLINE 2
#define TMAN_ATTR_PHASE 3
#define TMAN_ATTR_CONSTR 4

// Define framework related codes
#define TMAN_MAX_TASKS 16
#define TMAN_TICK_PERIOD 1000
#define TMAN_TASK_ID_SIZE 2
#define TMAN_MAX_CONSTR_TAKE 2
#define TMAN_MAX_CONSTR_GIVE 2

typedef struct TMAN_TASK_HANDLER {
    char task_id[TMAN_TASK_ID_SIZE];
    int activations;
    char constr_take[TMAN_MAX_CONSTR_TAKE][TMAN_TASK_ID_SIZE];
    char constr_give[TMAN_MAX_CONSTR_GIVE][TMAN_TASK_ID_SIZE];
    int constr_give_not_index[TMAN_MAX_CONSTR_GIVE];
    int num_constrs_take;
    int num_constrs_give;
    TaskHandle_t task_handle;
    TickType_t period;
    TickType_t last_activation_time;
    TickType_t deadline;
    TickType_t phase;
} TMAN_TASK_HANDLER;

typedef struct TMAN_TASK_STATUS {
    char task_id[2];
    TickType_t activation_time;
} TMAN_TASK_STATUS;

typedef struct TMAN_CONTROL {
    int new_task_index;
    TickType_t ticks;
    QueueHandle_t print_queue;
} TMAN_CONTROL;

// Define prototypes (public interface)
int TMAN_INIT();
int TMAN_CLOSE();
int TMAN_TASK_ADD(char* task_id, TaskFunction_t code, void* args, int priority);
int TMAN_TASK_REGISTER_ATTRIBUTES(char* task_id, int attr, void* value);
int TMAN_TASK_WAIT_PERIOD(char* task_id);
int TMAN_TASK_STATS(char* task_id, TMAN_TASK_STATUS* handler);
int TMAN_PRINT(char* msg);

#endif
