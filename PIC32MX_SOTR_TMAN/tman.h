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

// Define return codes
#define TMAN_SUCCESS 0
#define TMAN_FAIL -1

// Define Attribute codes
#define TMAN_ATTR_PERIOD 0
#define TMAN_ATTR_DEADLINE 1
#define TMAN_ATTR_PHASE 2
#define TMAN_ATTR_CONSTR 3

// Define prototypes (public interface)
int TMAN_INIT();
int TMAN_CLOSE();
int TMAN_TASK_ADD(char* task_id, void* code, void* args);
int TMAN_TASK_REGISTER_ATTRIBUTES(char* task_id, int attr, int value);
int TMAN_TASK_WAIT_PERIOD();
int TMAN_TASK_STATS(char* task_id);

#endif
