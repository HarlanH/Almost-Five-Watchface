#include "num2words.h"
#include "string.h"
#include <stdio.h>

#include "lang-swedish.h"
#include "lang-english.h"
#include "lang-english-uk.h"
#include "lang-norwegian.h"
#include "lang-dutch.h"
#include "lang-italian.h"
#include "lang-spanish.h"
#include "lang-german-eastern.h"
#include "lang-german-western.h"
#include "lang-french.h"
#include "lang-japanese.h"

static const Language* language = &LANG_ENGLISH;

static void get_english_day_of_month_ordinal(int day, char* message, size_t length) {
  static const char *ordinals[] = {
    "",
    "the first", "the second", "the third", "the fourth", "the fifth",
    "the sixth", "the seventh", "the eighth", "the ninth", "the tenth",
    "the eleventh", "the twelfth", "the thirteenth", "the fourteenth", "the fifteenth",
    "the sixteenth", "the seventeenth", "the eighteenth", "the nineteenth", "the twentieth",
    "the twenty-first", "the twenty-second", "the twenty-third", "the twenty-fourth", "the twenty-fifth",
    "the twenty-sixth", "the twenty-seventh", "the twenty-eighth", "the twenty-ninth", "the thirtieth",
    "the thirty-first"
  };

  if (day < 1 || day > 31) {
    message[0] = '\0';
    return;
  }

  snprintf(message, length, "%s", ordinals[day]);
}

void set_language(uint8_t lang) {
  switch (lang) {
    case LANG_EN:
      language = &LANG_ENGLISH;
      break;

    case LANG_EN_GB:
      language = &LANG_ENGLISH_UK;
      break;

    case LANG_SE:
      language = &LANG_SWEDISH;
      break;

    case LANG_NO:
      language = &LANG_NORWEGIAN;
      break;

    case LANG_NL:
      language = &LANG_DUTCH;
      break;

    case LANG_IT:
      language = &LANG_ITALIAN;
      break;

    case LANG_ES:
      language = &LANG_SPANISH;
      break;

    case LANG_GE:
      language = &LANG_GERMAN_E;
      break;

    case LANG_GW:
      language = &LANG_GERMAN_W;
      break;

    case LANG_FR:
      language = &LANG_FRENCH;
      break;

    case LANG_JA:
      language = &LANG_JAPANESE;
      break;

    default:
      language = &LANG_ENGLISH;
  }
}

const char* getHourWord(int hour) {
  int pos = (hour + 11) % 12; // + 11 instead of - 1 to avoid negative result
  return language->hours[pos];
}

const char* getFiveMinutePhrase(int fiveMinutePeriod) {
  return language->phrases[fiveMinutePeriod];
}


void check_exceptions(int hours, int pentaminutes, char* words, size_t length) {
  int i;
  for (i = 0; i < language->number_of_exceptions; i++) {
    if (hours == language->exceptions[i].hours) {
      if (pentaminutes == language->exceptions[i].pentaminutes) {
        strncpy(words, language->exceptions[i].phrase, length);
        return;
      }
    }
  }
}

void time_to_words(int hours, int minutes, char* words, size_t length) {

  memset(words, 0, length);

  // Fuzzy time
  int fiveMinutePeriod = (minutes / 5) % 12;

  check_exceptions(hours, fiveMinutePeriod, words, length);

  if (*words == '\0') {
    // No exception
    char phrase[length]; 
    strcpy(phrase, getFiveMinutePhrase(fiveMinutePeriod));
   
    char *variable = NULL;
    const char *hour;

    if ((variable = strstr(phrase, "$1")) != NULL) {
      hour = getHourWord(hours);
    }
    else if ((variable = strstr(phrase, "$2")) != NULL) {
      hour = getHourWord(hours + 1);
    }

    if (variable != NULL) {
      // Substitute '$x' with '%s'
      *variable = '%';
      *(variable + 1) = 's';
      snprintf(words, length, phrase, hour);
    } else {
      strncpy(words, phrase, length);
    }
  }
}

void time_to_greeting(int hour, char* greeting)
{
  int pos;

  if (hour < 6) {
    pos = 3; // night greeting
  } else if (hour < 12) {
    pos = 0; // morning greeting
  } else if (hour < 18) {
    pos = 1; // day greeting
  } else {
    pos = 2; // evening greeting
  }

  strcpy(greeting, language->greetings[pos]);
}

void get_connection_lost_message(char* message)
{
  strcpy(message, language->connection_lost);
}

void get_day_of_month_message(int day, char* message, size_t length)
{
  if (day < 1 || day > 31) {
    message[0] = '\0';
    return;
  }

  if (language == &LANG_ENGLISH || language == &LANG_ENGLISH_UK) {
    get_english_day_of_month_ordinal(day, message, length);
    return;
  }

  snprintf(message, length, language->day_of_month_format, day);
}

void get_meeting_now_message(char* message, size_t length)
{
  snprintf(message, length, "%s", language->meeting_now);
}

void get_meeting_soon_message(char* message, size_t length)
{
  snprintf(message, length, "%s", language->meeting_soon);
}

const char* get_battery_status_format(void)
{
  return language->battery_status_format;
}
