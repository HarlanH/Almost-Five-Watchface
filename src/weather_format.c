#include "weather_format.h"

#include <stdio.h>
#include <stdint.h>

#include "generated/weather_i18n_gen.h"
#include "num2words.h"

static int round_fahrenheit_to_celsius(int16_t temp_f)
{
	int32_t x = ((int32_t)temp_f - 32) * 5;
	if (x >= 0) {
		return (int)((x + 4) / 9);
	}
	return (int)((x - 4) / 9);
}

static const char *qual_for_ones(const WeatherLocale *L, int ones)
{
	if (ones <= 2) {
		return L->q_low;
	}
	if (ones <= 6) {
		return L->q_mid;
	}
	return L->q_high;
}

static const char *decade_for_tens(const WeatherLocale *L, int tens_digit)
{
	switch (tens_digit) {
		case 1:
			return L->d_teens;
		case 2:
			return L->d_twenties;
		case 3:
			return L->d_thirties;
		case 4:
			return L->d_forties;
		case 5:
			return L->d_fifties;
		case 6:
			return L->d_sixties;
		case 7:
			return L->d_seventies;
		case 8:
			return L->d_eighties;
		case 9:
			return L->d_nineties;
		default:
			return L->d_twenties;
	}
}

static void format_temperature_fahrenheit(char *buf, size_t len, const WeatherLocale *L, int16_t temp_f)
{
	int n = (int)temp_f;

	if (n < -20) {
		snprintf(buf, len, "%s", L->f_danger_cold);
		return;
	}
	if (n < 0) {
		snprintf(buf, len, "%s", L->f_below_zero);
		return;
	}
	if (n < 10) {
		snprintf(buf, len, "%s", L->f_single_digits);
		return;
	}
	if (n < 20) {
		snprintf(buf, len, "%s %s", qual_for_ones(L, n % 10), L->d_teens);
		return;
	}
	int tens_digit = n / 10;
	int ones = n % 10;
	if (tens_digit >= 10) {
		snprintf(buf, len, "%s", L->f_steamy);
		return;
	}
	snprintf(buf, len, "%s %s", qual_for_ones(L, ones), decade_for_tens(L, tens_digit));
}

static void format_temperature_celsius(char *buf, size_t len, const WeatherLocale *L, int n)
{
	if (n < -25) {
		snprintf(buf, len, "%s", L->c_danger_cold);
		return;
	}
	if (n < -10) {
		snprintf(buf, len, "%s", L->c_frigid);
		return;
	}
	if (n < 0) {
		snprintf(buf, len, "%s", L->c_below_freezing);
		return;
	}
	if (n <= 2) {
		snprintf(buf, len, "%s", L->c_around_freezing);
		return;
	}
	if (n <= 6) {
		snprintf(buf, len, "%s", L->c_cold);
		return;
	}
	if (n <= 9) {
		snprintf(buf, len, "%s", L->c_cool);
		return;
	}
	if (n < 20) {
		snprintf(buf, len, "%s %s", qual_for_ones(L, n % 10), L->d_teens);
		return;
	}
	int tens_digit = n / 10;
	int ones = n % 10;
	if (tens_digit >= 5) {
		snprintf(buf, len, "%s", L->c_very_hot);
		return;
	}
	snprintf(buf, len, "%s %s", qual_for_ones(L, ones), decade_for_tens(L, tens_digit));
}

void weather_format_phrase(char *out, size_t out_len, uint8_t lang, int16_t code, int16_t temp_f)
{
	const WeatherLocale *L = weather_locale_for_lang(lang);
	const char *sky = weather_sky_for_code(L, code);
	char band[40];

	if (lang == LANG_EN) {
		format_temperature_fahrenheit(band, sizeof(band), L, temp_f);
	} else {
		int c = round_fahrenheit_to_celsius(temp_f);
		format_temperature_celsius(band, sizeof(band), L, c);
	}

	snprintf(out, out_len, "%s, %s", sky, band);
}
