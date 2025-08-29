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
long start = 0;
long end = 100; // just in case div0 happens
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
  key_vals[2].keycode[0] = HID_KEY_EQUAL;
  key_vals[3].keycode[0] = HID_KEY_MINUS;
  key_vals[4].keycode[0] = HID_KEY_ALT_LEFT;
  key_vals[5].keycode[0] = HID_KEY_GUI_LEFT;
  key_vals[6].keycode[0] = HID_KEY_CONTROL_LEFT;
  key_vals[7].keycode[0] = HID_KEY_SHIFT_LEFT;
  key_vals[8].keycode[0] = HID_KEY_Z;
  key_vals[9].keycode[0] = HID_KEY_X;
  key_vals[10].keycode[0] = HID_KEY_C;
  key_vals[11].keycode[0] = HID_KEY_V;
  key_vals[12].keycode[0] = HID_KEY_B;
  key_vals[13].keycode[0] = HID_KEY_CAPS_LOCK;
  key_vals[14].keycode[0] = HID_KEY_HOME;
  key_vals[15].keycode[0] = HID_KEY_PAGE_DOWN;
  key_vals[16].keycode[0] = HID_KEY_G;
  key_vals[17].keycode[0] = HID_KEY_F;
  key_vals[18].keycode[0] = HID_KEY_D;
  key_vals[19].keycode[0] = HID_KEY_S;
  key_vals[20].keycode[0] = HID_KEY_A;
  key_vals[21].keycode[0] = HID_KEY_0;
  key_vals[22].keycode[0] = HID_KEY_TAB;
  key_vals[23].keycode[0] = HID_KEY_Q;
  key_vals[24].keycode[0] = HID_KEY_W;
  key_vals[25].keycode[0] = HID_KEY_E;
  key_vals[26].keycode[0] = HID_KEY_R;
  key_vals[27].keycode[0] = HID_KEY_T;
  key_vals[28].keycode[0] = HID_KEY_PAGE_UP;
  key_vals[29].keycode[0] = HID_KEY_5;
  key_vals[30].keycode[0] = HID_KEY_4;
  key_vals[31].keycode[0] = HID_KEY_3;
  key_vals[32].keycode[0] = HID_KEY_2;
  key_vals[33].keycode[0] = HID_KEY_1;
  key_vals[34].keycode[0] = HID_KEY_ESCAPE;

  // Layer 2 : ~~~~~~~~~~~~~~~~~~~~~~~~~~
  key_vals[2].keycode[1] = HID_USAGE_CONSUMER_VOLUME_INCREMENT;
  key_vals[3].keycode[1] = HID_USAGE_CONSUMER_VOLUME_DECREMENT;

  key_vals[33].keycode[1] = HID_KEY_F1;
  key_vals[32].keycode[1] = HID_KEY_F2;
  key_vals[31].keycode[1] = HID_KEY_F3;
  key_vals[30].keycode[1] = HID_KEY_F4;
  key_vals[29].keycode[1] = HID_KEY_F5;

  Serial.print("Keytype layer 0 = ");
  Serial.print(key_vals[0].key_type[0]);

  Serial.print(" : Keytype layer 1 = ");
  Serial.println(key_vals[0].key_type[1]);
  
  // Set min vals :
  for (int i = 0; i < KEY_COUNT; i++) {
    keys[i].real = analogRead(ADC1);
    keys[i].min_real = keys[i].real;  // set the min value to the value at startup
  }
  // need to implement them being saved ofc

}  // end of setup


const int cycles = 100;

int temparr[35] {0} ;//temporary

void loop()
{
  start = micros(); // get time

  Serial.print("Keytype layer 0 = ");
  Serial.print(key_vals[1].key_type[0]);

  Serial.print(" : Keytype layer 1 = ");
  Serial.println(key_vals[1].key_type[1]);

  unsigned int inner_start = micros();

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

    parse_keys_and_send_usb();

    // for ( int i = 0 ; i < KEY_COUNT ; i ++)
    // {
    //   keys[i].real = analogRead(ADC1);
      
    //   set_multiplexer(i+1);      
    //   set_pins(i+1);

    //   delayMicroseconds(20);
    // }
    // set_pins(0);
    // set_multiplexer(0);

    int timeDifference = 1000 - micros() + inner_start;
    if (timeDifference > 0)
      delayMicroseconds(timeDifference); 
  }

  // neopixel_update();
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
    // Serial.print(" | Pin ");
    // Serial.print(i);
    // Serial.print(" = ");
    // Serial.print(keys[i].real);
    // Serial.print(" : ");
    // Serial.print(keys[i].normalised);
    // Serial.print(keys[i].real - temparr[i]);
    Serial.print(keys[i].real);
    temparr[i] = keys[i].real;
    Serial.print("\t");
  }
  Serial.print(" | HZ = ");
  Serial.println(hz);


  end = micros();
  // micros() is in micro seconds or E-6 of 1 second

  hz = 1.0f / (float(end - start) / (1000000.0F * float(cycles)));
  // ~~ Speeds notes : ~~ standard clock speed
  // when just doing the 2 analog reads it runs at : 116,813.56 hz
  // When printing each analog value to serial : 9,000 hz

  // Serial.print("\t| HZ = ");
  // Serial.println(hz);
  */

}

