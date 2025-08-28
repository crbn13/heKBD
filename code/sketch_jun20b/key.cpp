#include "key.hpp"

Key::Key()
      : normalised(0), real(0), min_real(MAX_ANALOG_VALUE),
        max_real(MIN_ANALOG_VALUE), factor(0.0F), active_state(0), has_value_changed(0)
  {
  }
Key keys[KEY_COUNT];
KeyValue key_vals[KEY_COUNT]; 
