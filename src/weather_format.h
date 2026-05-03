#pragma once

#include <stddef.h>
#include <stdint.h>

void weather_format_phrase(char *out, size_t out_len, uint8_t lang, int16_t code, int16_t temp_f);
