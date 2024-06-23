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
#define QUEUE_LENGTH_            (1)
#define QUEUE_ITEM_SIZE_         (sizeof(ao_ui_message_t))
/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/
extern ao_led_handle_t ao_leds[];
/********************** internal functions definition ************************/
static void task_ao_ui_(void *argument) {
	ao_ui_handle_t *hao = (ao_ui_handle_t*) argument;

	while (true) {
		ao_ui_message_t msg_ui;
		if (pdPASS == xQueueReceive(hao->hqueue, &msg_ui, portMAX_DELAY)) {
			ao_led_message_t msg_led;

			switch (msg_ui) {
			case AO_UI_TYPE_PULSE:
				LOGGER_INFO("button pulse");
				msg_led = AO_LED_TYPE_RED;

				//parte nueva
				ao_led_init(&ao_leds[0]);
				if(0 == ao_led_send(&ao_leds[0], msg_led)){
					LOGGER_INFO("se lleno la cola de AO_leds")
				}

				break;

			case AO_UI_TYPE_SHORT:
				LOGGER_INFO("button short");
				msg_led = AO_LED_TYPE_GREEN;
				ao_led_init(&ao_leds[1]);
				if(0 == ao_led_send(&ao_leds[1], msg_led)){
					LOGGER_INFO("se lleno la cola de AO_leds")
				}
				break;

			case AO_UI_TYPE_LONG:
				LOGGER_INFO("button long");
				msg_led = AO_LED_TYPE_BLUE;
				ao_led_init(&ao_leds[2]);
				if(0 == ao_led_send(&ao_leds[2], msg_led)){
					LOGGER_INFO("se lleno la cola de AO_leds")
				}
				break;

			default:
				LOGGER_INFO("button error");
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

