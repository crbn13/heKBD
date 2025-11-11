#ifndef GUARD_KEYSTATE_PARSER_HPP
#define GUARD_KEYSTATE_PARSER_HPP
#include "config.hpp"
#include "usb_hid.hpp"
#include "key.hpp"
#include "multiplexer.hpp"

bool parse_keys_and_send_usb();

extern uint8_t active_layer;


#endif // Include guard


