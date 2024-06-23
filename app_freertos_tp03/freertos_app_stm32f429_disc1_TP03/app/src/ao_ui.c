/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao_ui.h"
#include "ao_led.h"

/********************** macros and definitions *******************************/
#define QUEUE_LENGTH_            (10)
#define QUEUE_ITEM_SIZE_         (sizeof(ao_ui_message_t))
/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
ao_led_handle_t ao_led;
/********************** external data definition *****************************/

extern TaskHandle_t ao_led_h;
/********************** internal functions definition ************************/
static void task_ao_ui_(void *argument) {
	ao_ui_handle_t *hao = (ao_ui_handle_t*) argument;

	//se crea y destruye la tarea para asignar el handler
	ao_led_init(&ao_led);
	vTaskDelete(ao_led_h);
	vQueueDelete(ao_led.hqueue);

	while (true) {
		ao_ui_message_t msg_ui;
		if (pdPASS == xQueueReceive(hao->hqueue, &msg_ui,portMAX_DELAY)){

			//Chequeamos si el objeto AO_LED existe (sino se crea)
			if(eDeleted == eTaskGetState(ao_led_h)){
				LOGGER_INFO("AO_UI: Se creo el objeto AO_LED");
				ao_led_init(&ao_led);  //crear tarea y cola
			} else{
				LOGGER_INFO("AO_UI: ya existe AO_LED");
			}

			ao_led_msg_led_t msg_led;
			switch (msg_ui) {
				case AO_UI_TYPE_PULSE:
					LOGGER_INFO("AO_UI: button pulse");
					msg_led.msg = AO_LED_TYPE_PULSE;
					msg_led.prio = AO_LED_PRIO_1;
					ao_led_send(&ao_led, msg_led);
				break;

			case AO_UI_TYPE_SHORT:
				LOGGER_INFO("AO_UI: button short");
				msg_led.msg = AO_LED_TYPE_SHORT;
				msg_led.prio = AO_LED_PRIO_2;
				ao_led_send(&ao_led, msg_led);
				break;

			case AO_UI_TYPE_LONG:
				LOGGER_INFO("AO_UI: button long");
				msg_led.msg = AO_LED_TYPE_LONG;
				msg_led.prio = AO_LED_PRIO_3;
				ao_led_send(&ao_led, msg_led);
				break;

			default:
				LOGGER_INFO("AO_UI: button error");
				break;
			}

		}

	}
}

/********************** external functions definition ************************/
void ao_ui_init(ao_ui_handle_t *hao) {

	//Creacion de la cola
	hao->hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while (NULL == hao->hqueue) {
		// error
	}

	//Creacion de la tarea
	BaseType_t status;
	status = xTaskCreate(task_ao_ui_, "task_ao_ui", 128, (void* const ) hao,
			tskIDLE_PRIORITY + 1UL, NULL);
	while (pdPASS != status) {
		// error
	}

}

bool ao_ui_send(ao_ui_handle_t *hao, ao_ui_message_t msg) {
	return (pdPASS == xQueueSend(hao->hqueue, (void* )&msg, 0));
}

