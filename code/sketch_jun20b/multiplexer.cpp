#include "multiplexer.hpp"

void set_pins(const uint8_t value)
{
  // Set direct multiplexer controll pins
  digitalWrite(MTP_BIN_PIN_1, bool(value & (0b00000001)));
  digitalWrite(MTP_BIN_PIN_2, bool(value & (0b00000010)));
  digitalWrite(MTP_BIN_PIN_3, bool(value & (0b00000100)));
  digitalWrite(MTP_BIN_PIN_4, bool(value & (0b00001000)));
}

#ifdef DIRECT_MULTIPLEXER_PIN_ENABLE_MODE 
// Sets the active multiplexer
void set_multiplexer(const uint8_t value)
{
  static unsigned int active_mtp = 100;

  if (active_mtp == +value / 16u) // integer division
  {
  }
  else
  {
    active_mtp = +value/16u;
    digitalWrite(MTP_1_ENABLE_PIN, !(0 == active_mtp));
    digitalWrite(MTP_2_ENABLE_PIN, !(1 == active_mtp));
    digitalWrite(MTP_3_ENABLE_PIN, !(2 == active_mtp));
    delayMicroseconds(50);
  }
}

void setup_pins()
{
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


