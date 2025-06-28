#include "Adafruit_TinyUSB.h"
// Number of keyboard keys
#define KEY_COUNT 60

// Analogue pins
#define ADC1 A0
#define ADC2 A1
typedef uint8_t NORMALISED_ADC_VAL;
#define MAX_NORMALISED_ADC_VAL 255

#define MAX_ANALOG_VALUE 4096
#define MIN_ANALOG_VALUE 0

#define MTP_BIN_PIN_1 D0 // These directly controll each multiplexer
#define MTP_BIN_PIN_2 D1
#define MTP_BIN_PIN_3 D2
#define MTP_BIN_PIN_4 D3
#define MTP_BIN_PIN_5 D4 // These enable the multiplexer disable pins
#define MTP_BIN_PIN_6 D5
#define MTP_BIN_PIN_7 D6

uint8_t const desc_hid_report[] = {TUD_HID_REPORT_DESC_KEYBOARD()};

// HID STUFF :
Adafruit_USBD_HID usb_hid;

long start = 0;
long end = 100; // just in case div0 happens
float hz = 0;




struct key {
  uint8_t normalised;     // number between 0-255 where 0 is unpressed and 255 is fully depressed
  uint16_t real;          // the value from ADC
  float min_real;         // minimum value read from ADC
  float max_real;         // value from ADC analog to digital conversion
  float factor;           // a number to map the un normalised values to 0-255 num
  bool active_state;      // The state that was last sent over usb
  uint8_t keycode;        // the HID_KEY value of the specific switch
  int has_value_changed;  // If the value hasnt changed for a few frames and the key isnt pressed we can reset the min value

  key()
      : normalised(0), real(0), min_real(MAX_ANALOG_VALUE),
        max_real(MIN_ANALOG_VALUE), factor(0.0F), active_state(0), keycode(0)
  {
  }
};

key keys[KEY_COUNT];

void setup()
{
  Serial.begin(9600);

  if (!TinyUSBDevice.isInitialized())
  {
    TinyUSBDevice.begin(0);
  }

  // Setup HID
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(1);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("TinyUSB Keyboard");

  // Set up output report (on control endpoint) for Capslock indicator
  usb_hid.setReportCallback(NULL, hid_report_callback);

  usb_hid.begin();

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // led pin
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif

  // set binary multiplexer output pins

  pinMode(MTP_BIN_PIN_1, OUTPUT);
  pinMode(MTP_BIN_PIN_2, OUTPUT);
  pinMode(MTP_BIN_PIN_3, OUTPUT);
  pinMode(MTP_BIN_PIN_4, OUTPUT);
  pinMode(MTP_BIN_PIN_5, OUTPUT);
  pinMode(MTP_BIN_PIN_6, OUTPUT);
  pinMode(MTP_BIN_PIN_7, OUTPUT);

  // set keycode values ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // need to actually make the custom keymap

  // for ( int i = 0 ; i < KEY_COUNT ; i++)
  // {
  // keys[i].keycode = keymap.key[]
  // }

  analogReadResolution(12);

  keys[0].keycode = HID_KEY_D;
  keys[1].keycode = HID_KEY_F;


  // Set min vals :
  for (int i = 0; i < KEY_COUNT; i++) {
    keys[i].real = analogRead(ADC1);
    keys[i].min_real = keys[i].real;  // set the min value to the value at startup
  }
  // need to implement them being saved ofc

}  // end of setup


const int cycles = 1;

void loop()
{
  start = micros(); // get time

  for (int i = 0; i < cycles; i++)
  {

    delay(1);

#ifdef TINYUSB_NEED_POLLING_TASK
    // Manual call tud_task since it isn't called by Core's background
    TinyUSBDevice.task();
#endif
    // not enumerated()/mounted() yet: nothing to do
    if (!TinyUSBDevice.mounted())
    {
      return;
    }
    // int val = analogRead(ADC1);
    // val = analogRead(ADC2);

    process_hid();
  }
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
  Serial.print(keys[1].has_value_changed);
*/

  for (uint8_t i = 0; i < KEY_COUNT; i++)
  {
    Serial.print(" | Pin ");
    Serial.print(i);
    Serial.print(" = ");
    Serial.print(keys[i].real);
  }
  Serial.print("\t| HZ = ");
  Serial.println(hz);
  end = micros();
  // micros() is in micro seconds or E-6 of 1 second

  hz = 1.0f / (float(end - start) / (1000000.0F * float(cycles)));
  // ~~ Speeds notes : ~~ standard clock speed
  // when just doing the 2 analog reads it runs at : 116,813.56 hz
  // When printing each analog value to serial : 9,000 hz

  // Serial.print("\t| HZ = ");
  // Serial.println(hz);
}

// Sets the 4 channel digital output to controll active multiplexer
void setpins(const uint8_t value)
{
  // Set direct multiplexer controll pins
  digitalWrite(MTP_BIN_PIN_1, bool(value & (0b00000001)));
  digitalWrite(MTP_BIN_PIN_2, bool(value & (0b00000010)));
  digitalWrite(MTP_BIN_PIN_3, bool(value & (0b00000100)));
  digitalWrite(MTP_BIN_PIN_4, bool(value & (0b00001000)));
}
// Sets the active multiplexer
void set_multiplexer(const uint8_t value)
{
  static unsigned int active_mtp = 0;

  if (active_mtp == +value / 16) // integer division
  {
  }
  else
  {
    digitalWrite(MTP_BIN_PIN_5, (0 == active_mtp));
    digitalWrite(MTP_BIN_PIN_6, (1 == active_mtp));
    digitalWrite(MTP_BIN_PIN_7, (2 == active_mtp));
  }

  // set multiplexer disable pins
  digitalWrite(
      MTP_BIN_PIN_5,
      !bool(value & (0b11110000))); // 000 the value when value = false
  digitalWrite(
      MTP_BIN_PIN_6,
      bool((value & (0b00010000) & ~(value & 0b11100000)))); // 001
  digitalWrite(
      MTP_BIN_PIN_7,
      bool((value & (0b00100000) & ~(value & 0b11010000)))); // 010

  Serial.print("Int val = ");
  Serial.print(+value);
  if (!bool(value & (0b11110000)))
    Serial.print("  Enabled MTP  ");
  else
    Serial.print("  Disabled MTP  ");
  if (bool((value & (0b00010000) & ~(value & 0b11100000))))
    Serial.print("Enabled MTP  ");
  else
    Serial.print("Disabled MTP  ");
  if (bool((value & (0b00100000) & ~(value & 0b11010000))))
    Serial.println("Enabled MTP  ");
  else
    Serial.println("Disabled MTP  ");
}

void process_hid()
{
  // used to avoid send multiple consecutive zero report for keyboard
  static bool keyPressedPreviously = false;

  uint8_t count = 0;         // the number of keys being pressed
  uint8_t keycodes[6] = {0}; // array of 6 keys that are being pressed
  bool modifier_changed = false;

  // check active keys and assign to keycode
  for (int i = 0; i < KEY_COUNT; i++)
  {

    // Multiplexer code to set value would be here but it doesnt exist yet

    setpins(i);
    keys[i].real = analogRead(ADC1);

    modifier_changed = false;

    if (keys[i].max_real < keys[i].real)
    {
      keys[i].max_real = keys[i].real;
      modifier_changed = true;
    }
    if (keys[i].min_real > keys[i].real)
    {
      keys[i].min_real = keys[i].real;
      modifier_changed = true;
    }

    auto change_modifier = [&]() {
      if (keys[i].max_real != keys[i].min_real && keys[i].max_real - keys[i].min_real > 50)  // only set modifier if
        keys[i].factor = (float)(NORMALISED_ADC_VAL)(-1) / (keys[i].max_real - keys[i].min_real);
      modifier_changed = false;
    };

    if (modifier_changed)
    {
      change_modifier();
    }

    decltype(keys[i].normalised) previous = +keys[i].normalised;

    keys[i].normalised =
        +int(abs((+keys[i].real - keys[i].min_real) * keys[i].factor));

    if (+int(abs((+keys[i].real - keys[i].min_real) * keys[i].factor)) >
        MAX_NORMALISED_ADC_VAL) // no cheeky buffer overflows
      keys[i].normalised = MAX_NORMALISED_ADC_VAL;

    // rapid trigger is by standard :

    const int bounds_checker = 10; // needs renaming, it accounts for the random
                                   // variance in the analogue input
    const int change_buffer = 2;

    //  MOTHER OPTIMSATION
    // divide by the bounds checker and ignore the remainder which has the ish
    // effect as the bounds checker but... if its near the bound value it
    // wouldnt work

    // 0 IS NOT DEPRESSED, 255 is FULLY DEPRESSED
    if (keys[i].active_state)
    {
      keys[i].has_value_changed = 0;
      if (keys[i].normalised < bounds_checker &&
          keys[i].normalised < +previous) // check if unpressed and key at top
      {
        keys[i].active_state = false;
        change_modifier();
      }
      else if (
          keys[i].normalised < +previous - change_buffer &&
          keys[i].normalised <
              MAX_NORMALISED_ADC_VAL -
                  bounds_checker) // check if direction of keystroke changed in
                                  // middle of stroke
      {
        keys[i].active_state = false;
        keycodes[count++] = keys[i].keycode;
      }
      else if (
          keys[i].normalised >=
          MAX_NORMALISED_ADC_VAL - bounds_checker) // if its at the 255 ranges
      {
        keycodes[count++] = keys[i].keycode;

      } else if (keys[i].normalised > previous)  // check if key travelling downwards
      {
        keycodes[count++] = keys[i].keycode;
      }
      else // if neither then set value to previous and switch still pressed
      {
        keycodes[count++] = keys[i].keycode;
        keys[i].normalised = previous;
      }
    }
    else // if key not already pressed
    {
      if (keys[i].normalised > +previous + change_buffer &&
          keys[i].normalised > bounds_checker) // check that it has moved enough
                                               // to achuate a press downwards
      {
        keys[i].has_value_changed = 0;
        keys[i].active_state = true;
        keycodes[count++] = keys[i].keycode;
      } else if (keys[i].normalised > previous)  // if the keystroke is going downwards then set the value to previous so that the distance gets bigger if it keeps going down
      {
        keys[i].normalised = previous;
      }
      else // when keys.normalised is < previous the key is travelling upwards
      {
        if (keys[i].normalised == previous)
        {
          keys[i].has_value_changed++;
        }
        else
        {
          keys[i].has_value_changed = 0; // this means that it just doesnt work?
        }
      }
    }

    if (keys[i].has_value_changed > 200)
    {
      keys[i].min_real = keys[i].real;
      keys[i].has_value_changed = 0;
      change_modifier();
    }

    if (count > 5) // usb hid has a max report of 6 keys at a time :(
      break;       // break out of loop
  }

  if (TinyUSBDevice.suspended() && count)
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  // skip if hid is not ready e.g still transferring previous report
  if (!usb_hid.ready())
    return;

  if (count)
  {
    // Send report if there is key pressed
    uint8_t const report_id = 0;
    uint8_t const modifier = 0;  // modifier keys stored in array of 1 bit numbers

    keyPressedPreviously = true;
    usb_hid.keyboardReport(report_id, modifier, keycodes);
  }
  else
  {
    // Send All-zero report to indicate there is no keys pressed
    // Most of the time, it is, though we don't need to send zero report
    // every loop(), only a key is pressed in previous loop()
    if (keyPressedPreviously)
    {
      keyPressedPreviously = false;
      usb_hid.keyboardRelease(0);
    }
  }
}

// Output report callback for LED indicator such as Caplocks
void hid_report_callback(
    uint8_t report_id,
    hid_report_type_t report_type,
    uint8_t const *buffer,
    uint16_t bufsize)
{
  (void)report_id;
  (void)bufsize;

  // LED indicator is output report with only 1 byte length
  if (report_type != HID_REPORT_TYPE_OUTPUT)
    return;

  // The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
  // Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
  uint8_t ledIndicator = buffer[0];

#ifdef LED_BUILTIN
  // turn on LED if capslock is set
  digitalWrite(LED_BUILTIN, ledIndicator & KEYBOARD_LED_CAPSLOCK);
#endif
}
