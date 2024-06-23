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
#define QUEUE_LENGTH_       (10)
#define QUEUE_ITEM_SIZE_    (sizeof(ao_led_msg_led_t))

#define TIME				(5000)
#define BUFFERSIZE			(sizeof(buffer)/sizeof(buffer[0]))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
TaskHandle_t ao_led_h;
/********************** external data definition *****************************/

/********************** internal functions definition ************************/
// Función para intercambiar dos elementos
static void swap(ao_led_message_t *xp, ao_led_message_t *yp) {
	ao_led_message_t temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Función para realizar el algoritmo de la burbuja
static void bubbleSort(ao_led_message_t arr[], int n) {
    int i, j;
    for (i = 0; i < n-1; i++) {
        // Últimos i elementos ya están ordenados
        for (j = 0; j < n-i-1; j++) {
            // Cambiar el signo de comparación para ordenar de mayor a menor
            if (arr[j] < arr[j+1]) {
                swap(&arr[j], &arr[j+1]);
            }
        }
    }
}


//imprime por consola el valores de un vector (solo para debug)
static void printArray(ao_led_message_t arr[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

static void apagarLeds(void){
	HAL_GPIO_WritePin(LED_A_PORT, LED_A_PIN, LED_OFF);
	HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, LED_OFF);
	HAL_GPIO_WritePin(LED_C_PORT, LED_C_PIN, LED_OFF);
}

static void task_ao_led_(void *argument) {
	ao_led_handle_t *hao = (ao_led_handle_t*) argument;

	ao_led_message_t buffer[10]; 				//Buffer para guardar los valore a procesar
	ao_led_message_t preValue = AO_LED_TYPE_N;	//Valor anterior, si es igual al atual no se apaga el led

	//Limpio el buffer (asigno el valor AO_LED_TYPE_N a todos los valores
	for (uint8_t i = 0; i < BUFFERSIZE; ++i) {
		buffer[i] = AO_LED_TYPE_N;
	}

	while (true) {

		ao_led_msg_led_t msg;

		// se chequea la cola, se guardan los valores en un buffer y se ordenan
		while (0 != uxQueueMessagesWaiting(hao->hqueue)) {
			if (pdPASS == xQueueReceive(hao->hqueue, &msg, portMAX_DELAY)) {
				LOGGER_INFO("AO_LED: Se recibio un mensaje desde AO_UI");

				//Aca guardanos el valor en el buffer en un lugar libre
				for (uint8_t i = 0; i < BUFFERSIZE; ++i) {
					if (AO_LED_TYPE_N == buffer[i]) {
						buffer[i] = msg.msg;
						break;
					}
				}
				//odenamos los valores de mayor a menor
				bubbleSort(buffer, BUFFERSIZE);
			}
		} // fin While while (0 != uxQueueMessagesWaiting(hao->hqueue))

		LOGGER_INFO("AO_LED: El buffer ordenado es:");
		printArray(buffer, BUFFERSIZE);

		//Desde Aqui se consumen los valore ordenados en el buffer hasta terminar
		uint8_t count = 0; //Variable auxiliar para determinar si el buffer esta limpio.
		for(uint8_t i = 0; i < BUFFERSIZE; ++i){

			switch (buffer[i]) {
			case AO_LED_TYPE_PULSE:
				if(AO_LED_TYPE_PULSE != preValue){
					apagarLeds();
					LOGGER_INFO("AO_LED: Se apago el LED");
					preValue = AO_LED_TYPE_PULSE;
				}else{
					preValue = AO_LED_TYPE_PULSE;
				}
				LOGGER_INFO("AO_LED: Se encendio el led ROJO");
				HAL_GPIO_WritePin(LED_A_PORT, LED_A_PIN, LED_ON);
				vTaskDelay(pdMS_TO_TICKS(TIME));
				buffer[i] = AO_LED_TYPE_N;
				break;

			case AO_LED_TYPE_SHORT:
				if(AO_LED_TYPE_SHORT != preValue){
					apagarLeds();
					LOGGER_INFO("AO_LED: Se apago el LED");
					preValue = AO_LED_TYPE_SHORT;
				}else{
					preValue = AO_LED_TYPE_SHORT;
				}
				LOGGER_INFO("AO_LED: Se encendio el led VERDE");
				HAL_GPIO_WritePin(LED_B_PORT, LED_B_PIN, LED_ON);
				vTaskDelay(pdMS_TO_TICKS(TIME));
				buffer[i] = AO_LED_TYPE_N;
				break;

			case AO_LED_TYPE_LONG:

				if(AO_LED_TYPE_LONG != preValue){
					apagarLeds();
					LOGGER_INFO("AO_LED: Se apago el LED");
					preValue = AO_LED_TYPE_LONG;
				}else{
					preValue = AO_LED_TYPE_LONG;
				}
				LOGGER_INFO("AO_LED: Se encendio el led AZUL");
				HAL_GPIO_WritePin(LED_C_PORT, LED_C_PIN, LED_ON);
				vTaskDelay(pdMS_TO_TICKS(TIME));
				LOGGER_INFO("AO_LED: Se proceso un LONG");
				buffer[i] = AO_LED_TYPE_N;
				break;

			case AO_LED_TYPE_N:
				if(count == BUFFERSIZE-1){  //solo se ejecuta 1 vez
					apagarLeds();
					LOGGER_INFO("AO_LED: Se apago el LED");
				}
				count++;
				break;

			default:
				LOGGER_INFO("AO_LED: Error Case");
				break;
			}


			if((BUFFERSIZE-1) < count){
				LOGGER_INFO("AO_LED: Buffer limpio - se elimina AO_LED (Queue + Task)\n");
				vQueueDelete(hao->hqueue);
				vTaskDelete(NULL);
			}
		} //fin for(uint8_t i = 0; i < sizeof(buffer); ++i)

	}
}

/********************** external functions definition ************************/
void ao_led_init(ao_led_handle_t *hao) {

	//Creacion de la cola
	hao->hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
	while (NULL == hao->hqueue) {
		// error
	}

	//Creacion de la tarea
	BaseType_t status;
	status = xTaskCreate(task_ao_led_,"AO_led", 128, (void* const ) hao,
			tskIDLE_PRIORITY + 1UL, &ao_led_h);
	while (pdPASS != status) {
		// error
	}
}

bool ao_led_send(ao_led_handle_t *hao, ao_led_msg_led_t msg) {
	return (pdPASS == xQueueSend(hao->hqueue, (void* )&msg, 0));
}

