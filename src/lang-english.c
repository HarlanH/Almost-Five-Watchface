#include "num2words.h"

/* Informal American English (default LANG_EN) */

const Language LANG_ENGLISH = {
  .hours = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine",
    "ten",
    "eleven",
    "twelve"
  },

#ifdef SCREEN_WIDE
  .phrases = {
    "*$1 o'clock ",
    "five after *$1 ",
    "ten after *$1 ",
    "quarter after *$1 ",
    "twenty after *$1 ",
    "twenty five after *$1 ",
    "*$1 thirty",
    "*$1 thirty five",
    "twenty til *$2 ",
    "quarter til *$2 ",
    "ten til *$2 ",
    "almost *$2 "
  },

  .greetings = {
    "Good morning ",
    "Good afternoon ",
    "Good evening ",
    "Good night "
  },
#else
  .phrases = {
    "*$1 o'clock ",
    "five  after *$1 ",
    "ten  after *$1 ",
    "quarter after *$1 ",
    "twenty after *$1 ",
    "twenty five  after *$1 ",
    "half  past *$1 ",
    "twenty five till *$2 ",
    "twenty  till *$2 ",
    "quarter till *$2 ",
    "ten till *$2 ",
    "almost *$2 "
  },

  .greetings = {
    "Good  mor-  ning ",
    "Good after- noon ",
    "Good even-  ing ",
    "Good night "
  },
#endif

  .connection_lost = "Where's your phone? ",
  .day_of_month_format = "the %d",
  .meeting_now = "Meeting Now!",
  .meeting_soon = "Meeting Soon!",
  .battery_status_format = "BAT %d%%",

  .number_of_exceptions = 0
};
