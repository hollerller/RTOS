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
#define QUEUE_LENGTH_			(1)
#define QUEUE_ITEM_SIZE_		(sizeof(ao_led_message_t))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/
extern ao_led_handle_t ao_leds[];
/********************** internal functions definition ************************/
static void deleteTask(void){
	LOGGER_INFO("se elimino la tarea\n\r");
	vTaskDelete(NULL);
}
static void task_ao_led_(void *argument) {
	ao_led_handle_t *hao = (ao_led_handle_t*) argument;

	while (true) {
		ao_led_message_t msg;
		if (pdPASS == xQueueReceive(hao->hqueue, &msg, portMAX_DELAY)) {
			//parte nueva
			switch (msg) {
			case AO_LED_TYPE_RED:
				LOGGER_INFO("Encender led ROJO (1)\n\r");
				HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_ON);
				vTaskDelay(pdMS_TO_TICKS(hao->time_on));
				HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_OFF);
				LOGGER_INFO("Apagar led ROJO (1)\n\r");
				LOGGER_INFO("...........................\n\r");
				break;

			case AO_LED_TYPE_GREEN:
				LOGGER_INFO("Encender led VERDE (2)\n\r");
				HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_ON);
				vTaskDelay(pdMS_TO_TICKS(hao->time_on));
				HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_OFF);
				LOGGER_INFO("Apagar led VERDE (2)\n\r");
				LOGGER_INFO("...........................\n\r");
				break;

			case AO_LED_TYPE_BLUE:
				LOGGER_INFO("Encender led AZUL (3)\n\r");
				HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_ON);
				vTaskDelay(pdMS_TO_TICKS(hao->time_on));
				HAL_GPIO_WritePin(hao->ldx_gpio_port, hao->ldx_pin, LED_OFF);
				LOGGER_INFO("Apagar led AZUL (3)\n\r");
				LOGGER_INFO("...........................\n\r");
				break;

			default:
				LOGGER_INFO("error");
				break;
			}

			deleteTask();
		}
	}
}

/********************** external functions definition ************************/
void ao_led_init(ao_led_handle_t *hao) {

	//Creacion de la tarea
	BaseType_t status;
	status = xTaskCreate(task_ao_led_, hao->ao_name, 128, (void* const ) hao,
			tskIDLE_PRIORITY + 1UL, NULL);
	while (pdPASS != status) {
		// error
	}
	LOGGER_INFO("Se creo la tarea del Led %s\n\r",hao->ao_name);
}

bool ao_led_send(ao_led_handle_t *hao, ao_led_message_t msg) {
	if(pdPASS == xQueueSend(hao->hqueue, (void* )&msg, 0)){
		return 1;
	}
	else{
		return 0;
	}
}

