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
  
  
  key_vals[0].keycode[0] = HID_KEY_A; // Layer 1
  key_vals[1].keycode[0] = HID_KEY_B;  
  key_vals[2].keycode[0] = HID_KEY_C;
  key_vals[3].keycode[0] = HID_KEY_D;
  key_vals[4].keycode[0] = HID_KEY_E;
  key_vals[5].keycode[0] = HID_KEY_F;
  key_vals[6].keycode[0] = HID_KEY_G;
  key_vals[7].keycode[0] = HID_KEY_H;
  key_vals[8].keycode[0] = HID_KEY_I;
  key_vals[9].keycode[0] = HID_KEY_J;
  // key_vals[5].keycode[0] = HID_KEY_GUI_LEFT;
  // key_vals[5].key_type[0] = KeyTypes::standard_actuation;
  // key_vals[6].keycode[0] = HID_KEY_CONTROL_LEFT;
  // key_vals[6].key_type[0] = KeyTypes::standard_actuation;
  // key_vals[7].keycode[0] = HID_KEY_SHIFT_LEFT;
  // key_vals[8].keycode[0] = HID_KEY_Z;
  // key_vals[9].keycode[0] = HID_KEY_X;

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

  unsigned long inner_start = micros();

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
      // keys[i].real = analogRead(ADC1);
      
      // set_multiplexer(i+1);      
      // set_pins(i+1);
// 
      // delayMicroseconds(5);
    // }
    // set_pins(0);
    // set_multiplexer(0);

    long int timeDifference = 1000 - micros() + inner_start;
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


  */
  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
    Serial.print(" | Pin ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(keys[i].real);
    Serial.print(" : ");
    Serial.print(keys[i].normalised);
    // Serial.print(keys[i].real - temparr[i]);
    // Serial.print(keys[i].real);
    // temparr[i] = keys[i].real;
    Serial.print("\t");
  }
  Serial.print(" | HZ = ");
  Serial.println(hz);


  end = micros();
  // micros() is in micro seconds or E-6 of 1 second


  // hz = 1.0f / (float(end - start) / (1000000.0F * float(cycles)));
  // ~~ Speeds notes : ~~ standard clock speed
  // when just doing the 2 analog reads it runs at : 116,813.56 hz
  // When printing each analog value to serial : 9,000 hz

  // Serial.print("\t| HZ = ");
  // Serial.println(hz);

}

