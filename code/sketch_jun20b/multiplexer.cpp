#include "multiplexer.hpp"

void set_pins(const uint8_t value)
{
  // Set direct multiplexer controll pins
  digitalWrite(MTP_BIN_PIN_4, bool(value & (0b00001000)));
  digitalWrite(MTP_BIN_PIN_3, bool(value & (0b00000100)));
  digitalWrite(MTP_BIN_PIN_2, bool(value & (0b00000010)));
  digitalWrite(MTP_BIN_PIN_1, bool(value & (0b00000001)));
}

#ifdef DIRECT_MULTIPLEXER_PIN_ENABLE_MODE 


int get_active_adc_pin(const uint8_t value)
{
  switch (value / 16u) // integer division to split into pins 0-15, 16-31 32-34
  {
  case 0:
    return A0;
  case 1:
    return A1;
  case 2:
    return A2;
  default:
  break;
  }
  Serial.println("SOMETHING WENT REALLY WRONG READING FORM A0 SO IT DOESNT CRASH BUT THIS IS BAD");
  return A0;
}

//does nothing i just cba to delete all its references
void set_multiplexer(const uint8_t value)
{
  digitalWrite(MTP_1_ENABLE_PIN, LOW);
  digitalWrite(MTP_2_ENABLE_PIN, LOW);
  digitalWrite(MTP_3_ENABLE_PIN, LOW);
  set_pins(value);
}
// Sets the active multiplexer
void set_multiplexer_old(const uint8_t value)
{
  static unsigned int active_mtp = 100;

  if (active_mtp == +value / 16u) // integer division
  {
    set_pins(value);
  }
  else
  {
    digitalWrite(MTP_1_ENABLE_PIN, !(0));
    digitalWrite(MTP_2_ENABLE_PIN, !(0));
    digitalWrite(MTP_3_ENABLE_PIN, !(0));
    set_pins(value);
    active_mtp = +value/16u;
    digitalWrite(MTP_1_ENABLE_PIN, !(0 == active_mtp));
    digitalWrite(MTP_2_ENABLE_PIN, !(1 == active_mtp));
    digitalWrite(MTP_3_ENABLE_PIN, !(2 == active_mtp));
    delayMicroseconds(30);
  }
}

void setup_pins()
{
  pinMode(ADC1, INPUT_PULLUP);
  pinMode(MTP_BIN_PIN_1, OUTPUT);
  pinMode(MTP_BIN_PIN_2, OUTPUT);
  pinMode(MTP_BIN_PIN_3, OUTPUT);
  pinMode(MTP_BIN_PIN_4, OUTPUT);
  pinMode(MTP_1_ENABLE_PIN, OUTPUT);
  pinMode(MTP_2_ENABLE_PIN, OUTPUT);
  pinMode(MTP_3_ENABLE_PIN, OUTPUT);
  pinMode(MTP_4_ENABLE_PIN, OUTPUT);
  pinMode(MTP_5_ENABLE_PIN, OUTPUT);
  pinMode(MTP_6_ENABLE_PIN, OUTPUT);
}
#endif

#ifdef MULTIPLEXER_CONTROL_MULTIPLEXER 
void set_multiplexer(const uint8_t value)
{
  static unsigned int active_mtp = 100;

  if (active_mtp == +value / 16u) // integer division
  {
  }
  else
  {
    active_mtp = +value/16u;
    digitalWrite(MTP_CONTROLL_BIN_1, bool(active_mtp & (0b00000001)));
    digitalWrite(MTP_CONTROLL_BIN_2, bool(active_mtp & (0b00000010)));
    digitalWrite(MTP_CONTROLL_BIN_3, bool(active_mtp & (0b00000100)));
    digitalWrite(MTP_CONTROLL_BIN_4, bool(active_mtp & (0b00001000)));
  }
}

void setup_pins()
{
  pinMode(MTP_BIN_PIN_1, OUTPUT);
  pinMode(MTP_BIN_PIN_2, OUTPUT);
  pinMode(MTP_BIN_PIN_3, OUTPUT);
  pinMode(MTP_BIN_PIN_4, OUTPUT);
  pinMode(MTP_CONTROLL_BIN_1, OUTPUT);
  pinMode(MTP_CONTROLL_BIN_2, OUTPUT);
  pinMode(MTP_CONTROLL_BIN_3, OUTPUT);
  pinMode(MTP_CONTROLL_BIN_4, OUTPUT);
}
#endif


