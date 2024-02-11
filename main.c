/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/trace.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#define CCM_RAM __attribute__((section(".ccmram")))



/*-----------------------------------------------------------*/
// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace-impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

//----------------------------------------------------------------------

TickType_t Period4 = pdMS_TO_TICKS(100);
#define number_of_slots (10)
QueueHandle_t queue;
SemaphoreHandle_t semaA;
SemaphoreHandle_t semaB;
SemaphoreHandle_t semaC;
SemaphoreHandle_t semaD;

TimerHandle_t timer1;
TimerHandle_t timer2;
TimerHandle_t timer3;
TimerHandle_t timer4;

static int blocked_counter_task1 =0;
static int transemeted_counter_task1 =0;
static int blocked_counter_task2 =0;
static int transemeted_counter_task2 =0;
static int blocked_counter_task3 =0;
static int transemeted_counter_task3 =0;
static int reciever_counter_task4 =0;
static int reciever_fails =0;

int timer1_calls =0;
int timer2_calls =0;
int timer3_calls =0;

int timer1_period =0;
int timer2_period =0;
int timer3_period =0;


int uniform_distribution(int rangeLow, int rangeHigh);

static int reset_counter = 0;
const int array1[6] = {50, 80, 110, 140, 170, 200};
const int array2[6] = {150, 200, 250, 300, 350, 400};

int m ;
void reset1()
{
	    transemeted_counter_task1 = 0;
	    transemeted_counter_task2 = 0;
	    transemeted_counter_task3 = 0;
	    blocked_counter_task1 = 0;
	    blocked_counter_task2 = 0;
	    blocked_counter_task3 = 0;
	    reciever_counter_task4 = 0;

	    xQueueReset(queue);

	}
static int i = 0;
void reset2()
{
	i++;
	reset_counter++;




	int total_Transmited = transemeted_counter_task1 + transemeted_counter_task2 + transemeted_counter_task3;
	int total_bocked = blocked_counter_task1 + blocked_counter_task2 + blocked_counter_task3;
	int total_sent = total_Transmited + total_bocked;
	printf("\n \n \nTotal Sent Messages: %d\n", total_sent);
    printf("Total Successfully Transmited Messages: %d\n", total_Transmited);
    printf("Total Blocked Messages: %d\n\n", total_bocked );

	//printf("Total recieved Messages: %d\n", reciever_counter_task4 );
    printf("Sender_1 - Sent Messages: %d\n", transemeted_counter_task1);
    printf("Sender_2 - Sent Messages: %d\n", transemeted_counter_task2);
    printf("Sender_3 - Sent Messages: %d\n", transemeted_counter_task3);
    printf("Sender_1 - Blocked Messages: %d\n", blocked_counter_task1);
    printf("Sender_2 - Blocked Messages: %d\n", blocked_counter_task2);
    printf("Sender_3 - Blocked Messages: %d\n", blocked_counter_task3);

/*
    double avg1 = ((double)timer1_period/timer1_calls);
    double avg2 = ((double)timer2_period/timer2_calls);
    double avg3 = ((double)timer3_period/timer3_calls);
    double total_avg = (avg1+ avg2 +avg3)/3 ;

    printf(" avg1 : %f \n" , avg1 );
    printf(" avg2 : %f \n" , avg2 );
    printf(" avg3 : %f \n" , avg3 );
    printf(" total_avg : %f \n" , total_avg );
*/
    printf("#Reset: %d \n", (reset_counter));

    reset1();

    if (reset_counter == 6)
    	{
    	trace_puts("Game Over !!! \n");
    	exit(0);
    	}
}

int uniform_distribution(int rangeLow, int rangeHigh)
{
    int myRand = rand();
    int range = rangeHigh - rangeLow + 1;
    int timer_period_1 = (myRand % range) + rangeLow;
    return timer_period_1;
}
/*
static int uniform_distribution(int rangeLow, int rangeHigh)
{
    int myRand = rand();
    int range = rangeHigh - rangeLow + 1;
    int timer_period_1 =( ((myRand / RAND_MAX)*range )+ rangeLow);
    return timer_period_1;
}
*/

static void sender_task_1(void *pvparameters)
{
	//code of task1

	BaseType_t xstatus;
	while(1)
	{
		xSemaphoreTake(semaA, portMAX_DELAY);

		char message[20];

		snprintf(message,19, "Time is %lu", xTaskGetTickCount());
		xstatus=xQueueSend(queue,&message,0);
		if(xstatus==pdPASS)
		{

			transemeted_counter_task1++;
		}
		else
		{

			blocked_counter_task1++;
		}

	}
}

static void sender_task_2(void * pvparameters)
{
	//code of task1
	BaseType_t xstatus;
	while(1)
	{
		xSemaphoreTake(semaB, portMAX_DELAY);

		char message[20];

		snprintf(message,19, "Time is %lu", xTaskGetTickCount());
		xstatus=xQueueSend(queue,&message,0);
		if(xstatus==pdPASS)
		{

			transemeted_counter_task2++;
		}
		else
		{

			blocked_counter_task2++;
		}

	}
}
static void sender_task_3(void * pvparameters)
{
	//code of task1

	BaseType_t xstatus;
	while(1)
	{
		xSemaphoreTake(semaC, portMAX_DELAY);

		char message[20];

		snprintf(message,19, "Time is %lu", xTaskGetTickCount());
		xstatus=xQueueSend(queue,&message,0);
		if(xstatus==pdPASS)
		{

			transemeted_counter_task3++;
		}
		else
		{

			blocked_counter_task3++;
		}

	}
}

static void reciever_task(void * parameters)
{
	BaseType_t xstatus;
	while(1)
	{
		//task sleep

		xSemaphoreTake(semaD,portMAX_DELAY);
		char message[20];
		xstatus=xQueueReceive(queue,message,0);
		if(xstatus==pdPASS)
		{

			reciever_counter_task4++;
			if (reciever_counter_task4 == 1000)
							{
				                trace_puts("REST!");
								reset2();

							}
		}
		else {
			reciever_fails++;
		}
	}
}

static void timer_1_callback(TimerHandle_t xTimer)
{
	//timer1_calls++;
	//timer1_period += uniform_distribution( array1[i], array2[i]);
	xTimerChangePeriod(timer1, pdMS_TO_TICKS( uniform_distribution( array1[i], array2[i] ) )  , 0 );
	xSemaphoreGive(semaA);
}
static void timer_2_callback(TimerHandle_t xTimer)
{
	//timer2_calls++;
	//timer2_period+= uniform_distribution( array1[i], array2[i]);
	xTimerChangePeriod(timer2, pdMS_TO_TICKS( uniform_distribution( array1[i], array2[i] ) )  , 0 );
	xSemaphoreGive(semaB);
}
static void timer_3_callback(TimerHandle_t xTimer)
{
	//timer3_calls++;
	//timer3_period+= uniform_distribution( array1[i], array2[i]);
	xTimerChangePeriod(timer3, pdMS_TO_TICKS( uniform_distribution( array1[i], array2[i] ) )  , 0 );
	xSemaphoreGive(semaC);
}
static void timer_4_callback(TimerHandle_t xTimer)
{
	xSemaphoreGive(semaD);
}



int main(int argc, char* argv[])

{
	srand( time(NULL) );
	trace_puts("MAIN_Started");
	// Add your code here.

	BaseType_t status,status1,status2,status3,status4;
	    semaA=xSemaphoreCreateBinary();
	    semaB=xSemaphoreCreateBinary();
		semaC=xSemaphoreCreateBinary();
	    semaD=xSemaphoreCreateBinary();

	    xSemaphoreGive(semaA);
	    xSemaphoreGive(semaB);
	    xSemaphoreGive(semaC);
	    xSemaphoreGive(semaD);


		queue=xQueueCreate(number_of_slots,sizeof(char[20]));
		if (queue != NULL)
		{ printf("Queue_Created \n");
		}


	   status= xTaskCreate(sender_task_1,"task_1",1000,(void*) 0,1,NULL);
	   status= xTaskCreate(sender_task_2,"task_2",1000,(void*) 0,1,NULL);
	   status= xTaskCreate(sender_task_3,"task_3",1000,(void*) 0,2,NULL);
	   status= xTaskCreate(reciever_task,"task_4",1000,NULL,3,NULL);

	   m = uniform_distribution(50,150);

		timer1=xTimerCreate("timer_1",( pdMS_TO_TICKS(uniform_distribution(50,150)) ),pdTRUE,0,timer_1_callback);
		timer2=xTimerCreate("timer_2",( pdMS_TO_TICKS(uniform_distribution(50,150)) ),pdTRUE,0,timer_2_callback);
		timer3=xTimerCreate("timer_3",( pdMS_TO_TICKS(uniform_distribution(50,150)) ),pdTRUE,0,timer_3_callback);
		timer4=xTimerCreate("timer_4",Period4,pdTRUE,0,timer_4_callback);

		if(timer1!=NULL && timer2!=NULL && timer3!=NULL && timer4!=NULL  )
		{
			status1=xTimerStart(timer1,0);
			status2=xTimerStart(timer2,0);
			status3=xTimerStart(timer3,0);
			status4=xTimerStart(timer4,0);

			if(status1==pdPASS  && status2==pdPASS && status3==pdPASS && status4==pdPASS)
			{
				reset1();


				vTaskStartScheduler();

			}
			else
			{
				printf("timers are not started ");
			}

		}
		return 0;

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------




void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

void vApplicationTickHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
  state will be stored. */
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

  /* Pass out the array that will be used as the Idle task's stack. */
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;

  /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configMINIMAL_STACK_SIZE is specified in words, not bytes. */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

