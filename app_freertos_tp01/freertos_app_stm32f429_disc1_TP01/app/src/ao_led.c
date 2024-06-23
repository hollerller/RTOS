//********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao_led.h"

/********************** macros and definitions *******************************/
#define QUEUE_LENGTH_            (1)
#define QUEUE_ITEM_SIZE_         (sizeof(ao_led_message_t))


/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/
static void task_ao_led_(void *argument){
	ao_led_handle_t* hao = (ao_led_handle_t*)argument;

	  while (true){
			ao_led_message_t msg;
		  if (pdPASS == xQueueReceive(hao->hqueue, &msg, portMAX_DELAY)){
		  	  if(AO_LED_TYPE_SIGNAL == msg){
		  		HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_ON);
		  		vTaskDelay(pdMS_TO_TICKS(hao->time_on));
		  		HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_OFF);
		  	  }
		  }
	  }
}

/********************** external functions definition ************************/
void ao_led_init(ao_led_handle_t* hao){


	//Creacion de la cola
	  hao->hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	  while(NULL == hao->hqueue)
	  {
	    // error
	  }

	//Creacion de la tarea
	  BaseType_t status;
	  status = xTaskCreate(task_ao_led_, hao->ao_name, 128, (void* const)hao, tskIDLE_PRIORITY +1UL, NULL);
	  while (pdPASS != status)
	  {
	    // error
	  }
}

bool ao_led_send(ao_led_handle_t* hao, ao_led_message_t msg){
	return (pdPASS == xQueueSend(hao->hqueue, (void*)&msg, 0));
}


