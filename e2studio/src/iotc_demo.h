/*
IoTConnect Demo Header

(C) 2024 Avnet, Inc.
Author: Eric Voirin

*/

#include <stdint.h>

/* This is all a bit hacky, but we need to tie the example project's components together so we can interact with them from the telemetry thread */

/* Defined in board_mon_thread_entry */
void set_led_frequency(uint16_t freq);