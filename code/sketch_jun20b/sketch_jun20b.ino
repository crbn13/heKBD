// Stuff to configure : ~~~~~~
#include "config.hpp"
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "Arduino.h"

#include "usb_hid.hpp"
#include "key.hpp"
#include "multiplexer.hpp"
#include "keystate_parser.hpp"
#include "neopixel.hpp"



// HZ controll stuff 
unsigned long start = 0;
unsigned long end = 100; // just in case div0 happens
float hz = 0;


void setup()
{
  Serial.begin(19200);

  setup_pins();
  setup_usb();
  neopixel_setup();

  // led pin
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  analogReadResolution(ANALOGUE_READ_RESOLUTION);

  // set binary multiplexer output pins


  // set keycode values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  for (int i = 0 ; i < KEY_COUNT; i++)
  {
    key_vals[i].keycode[0] = HID_KEY_Z;
  }
  
  
  key_vals[0].key_type[0] = KeyTypes::function_key; // function layer key
  key_vals[0].keycode[0] = 1; // Layer 1

  key_vals[1].keycode[0] = HID_KEY_SPACE;  
  key_vals[2].keycode[0] = HID_KEY_DELETE;
  key_vals[3].keycode[0] = HID_KEY_SLASH;
  key_vals[4].keycode[0] = HID_KEY_ARROW_LEFT;
  // key_vals[4].key_type[0] = KeyTypes::standard_actuation;
  key_vals[5].keycode[0] = HID_KEY_ARROW_DOWN;
  // key_vals[5].key_type[0] = KeyTypes::standard_actuation;
  key_vals[6].keycode[0] = HID_KEY_ARROW_RIGHT;
  // key_vals[6].key_type[0] = KeyTypes::standard_actuation;
  key_vals[7].keycode[0] = HID_KEY_SHIFT_LEFT;
  key_vals[7].key_type[0] = KeyTypes::standard_actuation;
  key_vals[8].keycode[0] = HID_KEY_ARROW_UP;
  key_vals[9].keycode[0] = HID_KEY_PERIOD;
  key_vals[10].keycode[0] = HID_KEY_COMMA;
  key_vals[11].keycode[0] = HID_KEY_M;
  key_vals[12].keycode[0] = HID_KEY_N;
  key_vals[13].keycode[0] = HID_KEY_BACKSPACE;
  key_vals[13].key_type[0] = KeyTypes::standard_actuation;
  key_vals[14].keycode[0] = HID_KEY_ENTER;
  key_vals[14].key_type[0] = KeyTypes::standard_actuation;
  key_vals[15].keycode[0] = HID_KEY_BRACKET_RIGHT;
  key_vals[16].keycode[0] = HID_KEY_H;
  key_vals[17].keycode[0] = HID_KEY_J;
  key_vals[18].keycode[0] = HID_KEY_K;
  key_vals[19].keycode[0] = HID_KEY_L;
  key_vals[20].keycode[0] = HID_KEY_SEMICOLON;
  key_vals[21].keycode[0] = HID_KEY_APOSTROPHE;
  key_vals[22].keycode[0] = HID_KEY_BACKSLASH;
  key_vals[23].keycode[0] = HID_KEY_P;
  key_vals[24].keycode[0] = HID_KEY_O;
  key_vals[25].keycode[0] = HID_KEY_I;
  key_vals[26].keycode[0] = HID_KEY_U;
  key_vals[27].keycode[0] = HID_KEY_Y;
  key_vals[28].key_type[0] = KeyTypes::function_key; // function layer key
  key_vals[28].keycode[0] = 1; // Layer 1
  // key_vals[28].keycode[0] = HID_KEY_PAGE_UP;
  key_vals[29].keycode[0] = HID_KEY_6;
  key_vals[30].keycode[0] = HID_KEY_7;
  key_vals[31].keycode[0] = HID_KEY_8;
  key_vals[32].keycode[0] = HID_KEY_9;
  key_vals[33].keycode[0] = HID_KEY_0;
  key_vals[34].keycode[0] = HID_KEY_BACKSPACE;
  // key_vals[34].key_type[0] = KeyTypes::standard_actuation;
  
  // Layer 2 : ~~~~~~~~~~~~~~~~~~~~~~~~~~
  key_vals[1].keycode[1] = HID_USAGE_CONSUMER_PLAY_PAUSE; // layer 1 : equals
  key_vals[2].keycode[1] = HID_KEY_VOLUME_UP; // layer 1 : equals
  key_vals[3].keycode[1] = HID_KEY_VOLUME_DOWN; // layer 1 : minus
  key_vals[13].keycode[1] = HID_KEY_PRINT_SCREEN; // layer 1 : capslock
  key_vals[17].keycode[1] = HID_KEY_PAGE_UP; // layer 1 : F
  key_vals[18].keycode[1] = HID_KEY_PAGE_DOWN; // layer 1 : D
  key_vals[34].keycode[1] = HID_KEY_GRAVE;
  key_vals[33].keycode[1] = HID_KEY_F6;
  key_vals[32].keycode[1] = HID_KEY_F7;
  key_vals[31].keycode[1] = HID_KEY_F8;
  key_vals[30].keycode[1] = HID_KEY_F9;
  key_vals[29].keycode[1] = HID_KEY_F10;
  key_vals[28].keycode[1] = HID_KEY_PAGE_DOWN;
  key_vals[15].keycode[1] = HID_KEY_F12; // layer1 : bracket
  key_vals[14].keycode[1] = HID_KEY_END;

  key_vals[16].keycode[1] = HID_KEY_ARROW_LEFT;
  key_vals[17].keycode[1] = HID_KEY_ARROW_DOWN;
  key_vals[18].keycode[1] = HID_KEY_ARROW_UP;
  key_vals[19].keycode[1] = HID_KEY_ARROW_RIGHT;
  
}  // end of setup


const int cycles = 100;

int temparr[35] {0} ;//temporary
#define SLEEP_TIME 10000 // number of cycles of inactivity to activate sleep mode
#define FREQUENCY_AWAKE 1000 //number of microseconds per cycle
#define FREQUENCY_ASLEEP 100000// number of microseconds per cycle
void loop()
{
  start = micros(); // get time

  unsigned long inner_start = micros();

  // Used to set the sleep timer so that a sleep state is started when the keyboard has been inactive for an extended period of time
  static unsigned long sleep_timer  = 0;
  static bool sleep = false;
  static unsigned int frequency = FREQUENCY_AWAKE;

  for (int i = 0; i < cycles; i++)
  {
    inner_start = micros();

#ifdef TINYUSB_NEED_POLLING_TASK
    // Manual call tud_task since it isn't called by Core's background
    TinyUSBDevice.task();
#endif
    // not enumerated()/mounted() yet: nothing to do
    if (!TinyUSBDevice.mounted())
    {
      return;
    }

    while (!usb_keyboard.ready() || !usb_controller.ready()) { /* wait till its all done */ }

    if (parse_keys_and_send_usb())
    {
      sleep_timer = 0u;
      sleep = false;
      frequency = FREQUENCY_AWAKE;
    }
    else
    {
      if (sleep)
      {
        sleep_timer = SLEEP_TIME;
        frequency = FREQUENCY_ASLEEP;
      }
      else
      {
        sleep_timer++;
        if (sleep_timer >= SLEEP_TIME)
        {
          sleep     = true;
          frequency = FREQUENCY_ASLEEP;
        }
      }
    }


    // for ( int i = 0 ; i < KEY_COUNT ; i ++)
    // {
      // keys[i].real = analogRead(ADC1);
      
      // set_multiplexer(i+1);      
      // set_pins(i+1);
// 
      // delayMicroseconds(5);
    // }
    // set_pins(0);
    // set_multiplexer(0);

    long int timeDifference = frequency - micros() + inner_start;
    if (timeDifference > 0)
      delayMicroseconds(timeDifference); 
  }

  
  neopixel_update(active_layer);
  /*
 
  Serial.print(keys[0].real);
  Serial.print("  ");
  Serial.print(keys[0].normalised);
  Serial.print("  ");
  Serial.print(keys[0].factor);
  Serial.print("  ");
  Serial.print(keys[0].max_real);
  Serial.print("  ");
  Serial.print(keys[0].min_real);
  Serial.print("  ");
  Serial.print(keys[0].active_state);
  Serial.print("  ");
  Serial.print(keys[0].has_value_changed);
  Serial.print("\t  |  ");

  Serial.print(keys[1].real);
  Serial.print("  ");
  Serial.print(keys[1].normalised);
  Serial.print("  ");
  Serial.print(keys[1].factor);
  Serial.print("  ");
  Serial.print(keys[1].max_real);
  Serial.print("  ");
  Serial.print(keys[1].min_real);
  Serial.print("  ");
  Serial.print(keys[1].active_state);
  Serial.print("  ");
  Serial.println(keys[1].has_value_changed);


  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
    Serial.print(" | Pin ");
    Serial.print(i);
    Serial.print(" = ");
    // Serial.print(keys[i].real);
    // Serial.print(" : ");
    Serial.print(keys[i].normalised);
    // Serial.print(keys[i].real - temparr[i]);
    // Serial.print(keys[i].real);
    // temparr[i] = keys[i].real;
    Serial.print("\t");
    if (i % 7 == 0 && i > 0) 
      Serial.print("\n");
  }
  // Serial.print(" | HZ = ");
  // Serial.println(hz);

*/
  end = micros();
  // micros() is in micro seconds or E-6 of 1 second
  hz = 1.0f / (float(end - start) / (1000000.0F * float(cycles)));

  // ~~ Speeds notes : ~~ standard clock speed
  // when just doing the 2 analog reads it runs at : 116,813.56 hz
  // When printing each analog value to serial : 9,000 hz

  Serial.print("\t| HZ = ");
  Serial.println(hz);


}

