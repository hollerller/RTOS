/********************** inclusions *******************************************/

#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "task_button.h"
#include "ao_ui.h"
#include "ao_led.h"

/********************** macros and definitions *******************************/
#define NUMBER_LEDS (sizeof(ao_leds)/sizeof(ao_leds[0]))
/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
ao_ui_handle_t ao_ui;


//ao_led_handle_t ao_leds[] = {
//		{ "ao_led_red", NULL, LED_A_PORT, LED_A_PIN, 1000 },	//led 1 (red)
//		{ "ao_led_green", NULL, LED_B_PORT, LED_B_PIN, 1000 },	//led 2 (green)
//		{ "ao_led_blue", NULL, LED_C_PORT, LED_C_PIN, 1000 },	//led 3 (blue)
//		};

/********************** external data declaration *****************************/

/********************** external functions definition ************************/
void app_init(void) {
	//const char *msg;
	//inicializamos el objeto activo ui
	ao_ui_init(&ao_ui);
	LOGGER_LOG("Se Creo el Objeto Activo ao_ui\n\r");

	//inicializamos los objetos activos de los leds
//	for (uint32_t i = 0; i < NUMBER_LEDS; ++i) {
//		ao_led_init(&ao_leds[i]);
//		msg = ao_leds[i].ao_name;
//		LOGGER_LOG("Se Creo el Objeto Activo %s\n\r", msg);
//	}

	BaseType_t status;

	status = xTaskCreate(task_button, "task_button", 128, NULL,
			tskIDLE_PRIORITY + 1UL, NULL);
	while (pdPASS != status) {
		// error
	}
	LOGGER_LOG("tasks init\n\r");
	cycle_counter_init();
}

/********************** end of file ******************************************/
