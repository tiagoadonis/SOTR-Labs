/* ************************************************************
* Xenomai - creates a periodic task
*	
* David Rocha nº 84807
* Tiago Adonis nº 88896
* Out/2020: Upgraded from Xenomai V2.5 to V3.1    
* 
************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <semaphore.h>

#include <sys/mman.h> // For mlockall

// Xenomai API (former Native API)
#include <alchemy/task.h>
#include <alchemy/timer.h>

#define MS_2_NS(ms)(ms*1000*1000) /* Convert ms to ns */

/* *****************************************************
 * Define task structure for setting input arguments
 * *****************************************************/
 struct taskArgsStruct {
	 RTIME taskPeriod_ns;
	 int some_other_arg;
 };

/* *******************
 * Task attributes 
 * *******************/ 
#define TASK_MODE 0  	// No flags
#define TASK_STKSZ 0 	// Default stack size

#define TASK_A_PRIO 25 	// RT priority [0..99]
#define TASK_A_PERIOD_NS MS_2_NS(1000)

RT_TASK task_1_desc; // Task decriptor

/* Add two other tasks */
RT_TASK task_2_desc; // Task1 decriptor
RT_TASK task_3_desc; // Task2 decriptor

/* *********************
* Function prototypes
* **********************/
void catch_signal(int sig); 	/* Catches CTRL + C to allow a controlled termination of the application */
void wait_for_ctrl_c(void);
void Heavy_Work(void);      	/* Load task */
void task_code(void *args); 	/* Task body */

/* ******************
* Main function
* *******************/ 
int main(int argc, char *argv[]) {
	int task1, task2, task3; 
	struct taskArgsStruct task1Args, task2Args, task3Args;
	cpu_set_t mask;
	
	/* Lock memory to prevent paging */
	mlockall(MCL_CURRENT|MCL_FUTURE); 

	/* Create RT task */
	/* Args: descriptor, name, stack size, priority [0..99] and mode (flags for CPU, FPU, joinable ...) */
	task1 = rt_task_create(&task_1_desc, "1", TASK_STKSZ, TASK_A_PRIO, TASK_MODE);
	if(task1) {
		printf("Error creating task 1 (error code = %d)\n",task1);
		return task1;
	} else 
		printf("Task 1 created successfully\n");
	
	/* A2 - Add two other tasks */
	/* Task 2 -> priority: 5 */
	task2 = rt_task_create(&task_2_desc, "2", TASK_STKSZ, 5, TASK_MODE);
	if(task2) {
		printf("Error creating task 2 (error code = %d)\n", task2);
		return task2;
	} else 
		printf("Task 2 created successfully\n");
	
	/* Task 3 -> priority: 50 */
	task3 = rt_task_create(&task_3_desc, "3", TASK_STKSZ, 50, TASK_MODE);
	if(task3) {
		printf("Error creating task 3 (error code = %d)\n", task3);
		return task3;
	} else 
		printf("Task 3 created successfully\n");
	
	/* A2 - Force these tasks to share the same CPU core */
	CPU_ZERO(&mask);  		// clear all CPUs 
	CPU_SET(1, &mask);    	// select CPU 1 

	rt_task_set_affinity(&task_2_desc, &mask);
	rt_task_set_affinity(&task_3_desc, &mask);

	/* Start RT task 1 */
	/* Args: task decriptor, address of function/implementation and argument*/
	task1Args.taskPeriod_ns = TASK_A_PERIOD_NS; 	
    rt_task_start(&task_1_desc, &task_code, (void *)&task1Args);
    
	/* A2 - Start RT task 3 */
	task2Args.taskPeriod_ns = TASK_A_PERIOD_NS;
	rt_task_start(&task_2_desc, &task_code, (void *)&task2Args);

	/* A2 - Start RT task 3 */
	task3Args.taskPeriod_ns = TASK_A_PERIOD_NS;
	rt_task_start(&task_3_desc, &task_code, (void *)&task3Args);

	/* A2 - wait for termination signal */	
	wait_for_ctrl_c();

	return 0;	
}

/* ***********************************
* Task body implementation
* *************************************/
void task_code(void *args) {
	RT_TASK *curtask;
	RT_TASK_INFO curtaskinfo;
	struct taskArgsStruct *taskArgs;

	RTIME ta = 0, ta_old = 0;
	unsigned long overruns;
	int err;

	int activations = 0;
	// Hold the minium/maximum observed inter arrival time
	uint64_t min_iat = UINT64_MAX, max_iat = 0;
	
	/* Get task information */
	curtask=rt_task_self();
	rt_task_inquire(curtask,&curtaskinfo);
	taskArgs=(struct taskArgsStruct *)args;
	printf("Task %s init, period:%llu\n", curtaskinfo.name, taskArgs->taskPeriod_ns);
		
	/* Set task as periodic */
	err = rt_task_set_periodic(NULL, TM_NOW, taskArgs->taskPeriod_ns);
	for(;;) {
		err = rt_task_wait_period(&overruns);
		if(err) {
			printf("task %s overrun!!!\n", curtaskinfo.name);
			break;
		}
		
		activations++;
		ta = rt_timer_read();

		if(activations > 1){
			if (abs(ta - ta_old) < min_iat){
				min_iat = abs(ta - ta_old);
			}
			if (abs(ta - ta_old) > max_iat){
				max_iat = abs(ta - ta_old);
			}
			printf("Task %s activation at time %llu min: %lu | max: %lu\n", curtaskinfo.name, ta, min_iat, max_iat);
		}
		else{
			printf("Task %s activation at time %llu\n", curtaskinfo.name, ta);
		}

		/* Task "load" */
		Heavy_Work();
		ta_old = ta;
	}
	return;
}


/* **************************************************************************
 *  Catch control+c to allow a controlled termination
 * **************************************************************************/
void catch_signal(int sig)
{
	return;
}

void wait_for_ctrl_c(void) {
	signal(SIGTERM, catch_signal); //catch_signal is called if SIGTERM received
	signal(SIGINT, catch_signal);  //catch_signal is called if SIGINT received

	// Wait for CTRL+C or sigterm
	pause();
	
	// Will terminate
	printf("Terminating ...\n");
}

/* **************************************************************************
 *  Task load implementation. In the case integrates numerically a function
 * **************************************************************************/
#define f(x) 1/(1+pow(x,2)) /* Define function to integrate*/
void Heavy_Work(void)
{
	float lower, upper, integration=0.0, stepSize, k;
	int i, subInterval;
	
	RTIME ts, // Function start time
		  tf; // Function finish time
			
	static int first = 0; // Flag to signal first execution		
	
	/* Get start time */
	ts=rt_timer_read();
	
	/* Integration parameters */
	/*These values can be tunned to cause a desired load*/
	lower=0;
	upper=100;
	subInterval=1000000;

	/* Calculation */
	/* Finding step size */
	stepSize = (upper - lower)/subInterval;

	/* Finding Integration Value */
	integration = f(lower) + f(upper);
	for(i=1; i<= subInterval-1; i++){
		k = lower + i*stepSize;
		integration = integration + 2 * f(k);
 	}
	integration = integration * stepSize/2;
 	
 	/* Get finish time and show results */
 	if (!first) {
		tf=rt_timer_read();
		tf-=ts;  // Compute time difference form start to finish
 	
		printf("Integration value is: %.3f. It took %9llu ns to compute.\n", integration, tf);
		
		first = 1;
	}
}