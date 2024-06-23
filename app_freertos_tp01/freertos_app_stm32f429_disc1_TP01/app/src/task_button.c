/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "task_button.h"
#include "ao_ui.h"

/********************** macros and definitions *******************************/
#define TASK_PERIOD_MS_           (50)

#define BUTTON_PERIOD_MS_         (TASK_PERIOD_MS_)
#define BUTTON_PULSE_TIMEOUT_     (200)
#define BUTTON_SHORT_TIMEOUT_     (1000)
#define BUTTON_LONG_TIMEOUT_      (2000)
/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/
extern ao_ui_handle_t ao_ui;
/********************** internal functions definition ************************/

/********************** external functions definition ************************/

typedef enum
{
  BUTTON_TYPE_NONE,
  BUTTON_TYPE_PULSE,
  BUTTON_TYPE_SHORT,
  BUTTON_TYPE_LONG,
  BUTTON_TYPE__N,
} button_type_t;

static struct
{
    uint32_t counter;
} button;

static void button_counter_init_(void)
{
  button.counter = 0;
}

static button_type_t button_process_state_(bool value)
{
  button_type_t ret = BUTTON_TYPE_NONE;
  if(value)
  {
    button.counter += BUTTON_PERIOD_MS_;
  }
  else
  {
    if(BUTTON_LONG_TIMEOUT_ <= button.counter)
    {
      ret = BUTTON_TYPE_LONG;
    }
    else if(BUTTON_SHORT_TIMEOUT_ <= button.counter)
    {
      ret = BUTTON_TYPE_SHORT;
    }
    else if(BUTTON_PULSE_TIMEOUT_ <= button.counter)
    {
      ret = BUTTON_TYPE_PULSE;
    }
    button.counter = 0;
  }
  return ret;
}

/********************** external functions definition ************************/

void task_button(void* argument)
{
  button_counter_init_();

  while(true)
  {
    GPIO_PinState button_state;
    button_state = HAL_GPIO_ReadPin(BUTTON_A_PORT, BUTTON_A_PIN);

    button_type_t button_type;
    button_type = button_process_state_(button_state);

    ao_ui_message_t msg;

    switch (button_type) {
      case BUTTON_TYPE_NONE:
        break;

      case BUTTON_TYPE_PULSE:
        //LOGGER_INFO("button pulse");
        msg = AO_UI_TYPE_PULSE;
        ao_ui_send(&ao_ui, msg);
        break;

      case BUTTON_TYPE_SHORT:
     //   LOGGER_INFO("button short");
        msg = AO_UI_TYPE_SHORT;
        ao_ui_send(&ao_ui, msg);
        break;

      case BUTTON_TYPE_LONG:
       // LOGGER_INFO("button long");
        msg = AO_UI_TYPE_LONG;
        ao_ui_send(&ao_ui, msg);
        break;
      default:
        LOGGER_INFO("button error");
        break;
    }

    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** end of file ******************************************/
