#ifndef INC_AO_UI_H_
#define INC_AO_UI_H_
/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef enum
{
  AO_UI_TYPE_PULSE,
  AO_UI_TYPE_SHORT,
  AO_UI_TYPE_LONG,
  AO_UI_TYPE_N,
} ao_ui_message_t;


typedef struct
{
    QueueHandle_t hqueue;
} ao_ui_handle_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

bool ao_ui_send(ao_ui_handle_t* hao, ao_ui_message_t msg);

void ao_ui_init(ao_ui_handle_t* hao);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif



#endif /* INC_AO_UI_H_ */
