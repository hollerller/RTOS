#ifndef INC_AO_LED_H_
#define INC_AO_LED_H_
/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef enum
{
  AO_LED_TYPE_PULSE,
  AO_LED_TYPE_SHORT,
  AO_LED_TYPE_LONG,
  AO_LED_TYPE_N,
} ao_led_message_t;

typedef enum
{
	AO_LED_PRIO_1,
	AO_LED_PRIO_2,
	AO_LED_PRIO_3,
} ao_led_msg_priority_t;

typedef struct
{
	ao_led_message_t msg;
	ao_led_msg_priority_t prio;
} ao_led_msg_led_t;

typedef struct
{
	QueueHandle_t hqueue;
} ao_led_handle_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
bool ao_led_send(ao_led_handle_t* hao, ao_led_msg_led_t msg);

void ao_led_init(ao_led_handle_t* hao);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif





#endif /* INC_AO_LED_H_ */
