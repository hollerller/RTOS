#ifndef INC_AO_LED_H_
#define INC_AO_LED_H_
/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef enum
{
  AO_LED_TYPE_SIGNAL,
  AO_LED_TYPE_N,
} ao_led_message_t;


typedef struct
{
    const char*		ao_name;			//Nombre del objeto activo
    QueueHandle_t 	hqueue;				//cola para las notificaciones
	GPIO_TypeDef *	ldx_gpio_port;		//port del led a usar
	uint16_t		ldx_pin;			//pin del led a usar
	uint16_t		time_on;			//tiempo que permanece encendido
} ao_led_handle_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
bool ao_led_send(ao_led_handle_t* hao, ao_led_message_t msg);

void ao_led_init(ao_led_handle_t* hao);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif





#endif /* INC_AO_LED_H_ */
