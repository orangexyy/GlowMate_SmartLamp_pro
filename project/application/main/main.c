#include "stm32f4xx.h" 

#include "system.h"
#include "app.h"
#include "driver.h"
#include "thread.h"

int main()
{
	SysTick_Init(168);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	driver_init();
    app_init();

    thread_run();    
	
}

