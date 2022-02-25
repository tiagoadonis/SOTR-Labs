## [tutorial01](https://github.com/tiagoadonis/SOTR-Labs/tree/master/tutorial01)

- The first tutorial is about Real Time Services on Linux.
- Every tutorial has a litle report associated, where some questions are answered. The report of tutorial 01 is [here](https://github.com/tiagoadonis/SOTR-Labs/blob/master/tutorial01/report.pdf).

## [tutorial02](https://github.com/tiagoadonis/SOTR-Labs/tree/master/tutorial02)

- The second tutorial is an brief introduction to Xenomai, an development structure in real time which cooperates with linux kernel.
- The report of tutorial 02 is [here](https://github.com/tiagoadonis/SOTR-Labs/blob/master/tutorial02/report.pdf).

## [tutorial03](https://github.com/tiagoadonis/SOTR-Labs/tree/master/tutorial03)

- The third and last turorial is an brie introduction to FreeRTOS, an kernel of an operational system in real time for devices embarked which was ported to 35 microcontrollers plataforms.
- The report of tutorial 03 is [here](https://github.com/tiagoadonis/SOTR-Labs/blob/master/tutorial03/report.pdf).

## [final_project](https://github.com/tiagoadonis/SOTR-Labs/tree/master/final_project)

The objective of the work is developing a framework (Task Manager – TMan) that allows registering a set of FreeRTOS tasks, associate each task with a set of attributes (e.g. period, deadline, phase, precedence constraints) and activate those tasks at the appropriate instants. 
<br />
The following methods should be provided:
- **TMAN_Init:** initialization of the framework
- **TMAN_Close:** terminate the framework
- **TMAN_TaskAdd:** Add a task to the framework
- **TMAN_TaskRegisterAttributes:** Register attributes (e.g. period, phase, deadline, precedence constraints) for a task already added to the framework
- **TMAN_TaskWaitPeriod:** Called by a task to signal the termination of an instance and wait for the next activation
- **TMAN_TaskStats:** returns statistical information about a task. Provided information must include at least the number of activations, but additional info (e.g. number of deadline misses) will be valued. 

**NOTE:** For the project properly work, it's necessary to modify the kernel header file, line 878. Replace configTASK_NOTIFICATION_ARRAY_ENTRIES macro with value 3.
