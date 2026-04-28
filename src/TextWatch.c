#include <pebble.h>
#ifdef PBL_COLOR
  #include "gcolor_definitions.h" // Allows the use of color
#endif

#include "num2words.h"
#include "TextWatch.h"
#include "config_message.h"

Window *window;
TextLayer *dayOfMonthLayer;
TextLayer *btStatusLayer;
TextLayer *meetingStatusLayer;
TextLayer *batteryStatusLayer;
char dayOfMonthText[32];
char btStatusText[12];
char meetingStatusText[20];
char batteryStatusText[20];
int lastDisplayedDay = -1;
AppTimer *backlightTimer = NULL;
bool btConnected = true;
int batteryPercent = 100;

Line lines[NUM_LINES];

int currentMinutes;
int currentNLines;

// Corrent color of normal text
GColor8 regularTextColor;
// Corrent color of bold text
GColor8 boldTextColor;

// Time in seconds to add to the current time before calculating which
// 5-minute period we should display
int timeOffset;

// Variale to keep track of the last minute that we updated the time
// Used to optimise so we only need to run time logic once per minute.
int lastMinute = -1;

// Time in seconds since epoch when a displayed message should be removed.
// Only set to non zero when a message is displaying.
time_t resetMessageTime = 0;

// Time in seconds since epoch when connection lost message will be displayed,
// if connection is still lost... (attempt to reduce false notifications)
time_t connectionLostTime = 0;

// Time in seconds to diplay the greeting messages at startup. 0 means disable
// greeting messages.
int messageTime = 3;

// Which gesture to activate date screen
// 0 = off
// 1 = Boxing move (X-axis)
// 2 = flick wrist (Y-axis)
// 3 = Shake up/down (Z-axis)
// 4 = Any shake
int dateGesture = GESTURE_ANY;

// Notify when BT connection is lost?
// 0 = off
// 1 = text and light only
// 2 = on
int bt_lost_notification = BT_NOTIFY_ON;
int meetingStatus = MEETING_STATUS_NONE;

// Screen resolution. Set in the init function.
int xres;
int yres;

static ConfigMessageContext config_message_context;
#define STATUS_BAR_HEIGHT 24
#define DAY_LINE_RESERVED_HEIGHT 30
#define TIME_BLOCK_Y_BIAS -2

void backlight_off_handler(void *context)
{
	(void)context;
	light_enable(false);
	backlightTimer = NULL;
}

void update_status_indicators(void)
{
	if (btConnected) {
		btStatusText[0] = '\0';
	} else {
		snprintf(btStatusText, sizeof(btStatusText), "BT!");
	}

	if (batteryPercent < 40) {
		snprintf(batteryStatusText, sizeof(batteryStatusText), get_battery_status_format(), batteryPercent);
	} else {
		batteryStatusText[0] = '\0';
	}

	if (meetingStatus == MEETING_STATUS_NOW) {
		text_layer_set_font(meetingStatusLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
		get_meeting_now_message(meetingStatusText, sizeof(meetingStatusText));
	} else if (meetingStatus == MEETING_STATUS_SOON) {
		text_layer_set_font(meetingStatusLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
		get_meeting_soon_message(meetingStatusText, sizeof(meetingStatusText));
	} else {
		text_layer_set_font(meetingStatusLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
		meetingStatusText[0] = '\0';
	}

	text_layer_set_text(btStatusLayer, btStatusText);
	text_layer_set_text(meetingStatusLayer, meetingStatusText);
	text_layer_set_text(batteryStatusLayer, batteryStatusText);
}

// UTF8 aware strlen() for a sequence of bytes
int strlenUtf8(char *start, char *end) 
{
	int span = end - start;
	int i = 0, j = 0;
	while (start[i] && i < span) 
	{
		if ((start[i] & 0xc0) != 0x80) j++;
		i++;
	}
	return j;
}

// Animation handler
void animationStoppedHandler(struct Animation *animation, bool finished, void *context)
{
	TextLayer *current = (TextLayer *)context;
	GRect rect = layer_get_frame((Layer *)current);
	rect.origin.x = xres;
	layer_set_frame((Layer *)current, rect);
}

// Animate line
void makeAnimationsForLayer(Line *line, int delay)
{
	TextLayer *current = line->currentLayer;
	TextLayer *next = line->nextLayer;

#ifdef PBL_PLATFORM_APLITE
	// Destroy old animations 
	if (line->animation1 != NULL)
	{
		 property_animation_destroy(line->animation1);
	}
	if (line->animation2 != NULL)
	{
		 property_animation_destroy(line->animation2);
	}
#endif

	// Configure animation for current layer to move out
	GRect rect = layer_get_frame((Layer *)current);
	rect.origin.x =  -xres;
	line->animation1 = property_animation_create_layer_frame((Layer *)current, NULL, &rect);
	Animation *animation = property_animation_get_animation(line->animation1);
	animation_set_duration(animation, ANIMATION_DURATION);
	animation_set_delay(animation, delay);
	animation_set_curve(animation, AnimationCurveEaseIn); // Accelerate

	// Configure animation for current layer to move in
	GRect rect2 = layer_get_frame((Layer *)next);
	rect2.origin.x = 0;
	line->animation2 = property_animation_create_layer_frame((Layer *)next, NULL, &rect2);
	animation = property_animation_get_animation(line->animation2);	
	animation_set_duration(animation, ANIMATION_DURATION);
	animation_set_delay(animation, delay + ANIMATION_OUT_IN_DELAY);
	animation_set_curve(animation, AnimationCurveEaseOut); // Deaccelerate

	// Set a handler to rearrange layers after animation is finished
	animation_set_handlers(animation, (AnimationHandlers) {
		.stopped = (AnimationStoppedHandler)animationStoppedHandler
	}, current);

	// Start the animations
	animation_schedule(property_animation_get_animation(line->animation1));
	animation_schedule(property_animation_get_animation(line->animation2));	
}

void updateLayerText(TextLayer* layer, char* text)
{
	const char* layerText = text_layer_get_text(layer);
	strcpy((char*)layerText, text);
	// To mark layer dirty
	text_layer_set_text(layer, layerText);
    //layer_mark_dirty(&layer->layer);
}

// Update line
void updateLineTo(Line *line, char *value, int delay)
{
	updateLayerText(line->nextLayer, value);
	makeAnimationsForLayer(line, delay);

	// Swap current/next layers
	TextLayer *tmp = line->nextLayer;
	line->nextLayer = line->currentLayer;
	line->currentLayer = tmp;
}

// Check to see if the current line needs to be updated
bool needToUpdateLine(Line *line, char *nextValue)
{
	const char *currentStr = text_layer_get_text(line->currentLayer);

	if (strcmp(currentStr, nextValue) != 0) {
		return true;
	}
	return false;
}

// Configure bold line of text
void configureBoldLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_color(textlayer, boldTextColor);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, TEXT_ALIGN);
}

// Configure bold line of text
void configureSmallBoldLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_color(textlayer, boldTextColor);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, TEXT_ALIGN);
}

// Configure compact bold line of text
void configureCompactBoldLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
	text_layer_set_text_color(textlayer, boldTextColor);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, TEXT_ALIGN);
}

// Configure light line of text
void configureLightLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_get_system_font(FONT_KEY_BITHAM_42_LIGHT));
	text_layer_set_text_color(textlayer, regularTextColor);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, TEXT_ALIGN);
}

// Configure small line of text
void configureSmallLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	text_layer_set_text_color(textlayer, regularTextColor);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, TEXT_ALIGN);
}

// Configure compact line of text
void configureCompactLayer(TextLayer *textlayer)
{
	text_layer_set_font(textlayer, fonts_get_system_font(FONT_KEY_GOTHIC_28));
	text_layer_set_text_color(textlayer, regularTextColor);
	text_layer_set_background_color(textlayer, GColorClear);
	text_layer_set_text_alignment(textlayer, TEXT_ALIGN);
}

// Configure the layers for the given text
int configureLayersForText(char text[NUM_LINES][BUFFER_SIZE], char format[])
{
	int numLines = 0;
	int height = 0;
	int offsets[4];
	bool compactLayout = false;

	// Count lines first so we can choose a compact style for 4-row layouts.
	for (int i = 0; i < NUM_LINES; i++) {
		if (strlen(text[i]) == 0) {
			break;
		}
		numLines++;
	}
	compactLayout = (numLines == NUM_LINES);

	// Set bold layer.
	int i;
	for (i = 0; i < numLines; i++) {
		if (strlen(text[i]) == 0) {
			break;
		}

		offsets[i] = compactLayout ? ROW_OFFSET_COMPACT : ROW_OFFSET;
		if (format[i] == 'B') // Bold
		{
			if (compactLayout) {
				configureCompactBoldLayer(lines[i].nextLayer);
			} else {
				configureBoldLayer(lines[i].nextLayer);
			}
		}
		else if (format[i] == 'b') // Small bold
		{
			configureSmallBoldLayer(lines[i].nextLayer);
			offsets[i] = ROW_OFFSET_SMALL;
			// If there is a line above, increase its offset a bit 
			if (i > 0) {
				offsets[i - 1] += TOP_MARGIN_SMALL;
				height += TOP_MARGIN_SMALL;
			}
		}
		else if (format[i] == 's') // Small
		{
			configureSmallLayer(lines[i].nextLayer);
			offsets[i] = ROW_OFFSET_SMALL;
			// If there is a line above, increase its offset a bit 
			if (i > 0) {
				offsets[i - 1] += TOP_MARGIN_SMALL;
				height += TOP_MARGIN_SMALL;
			}
		}
		else // Normal line
		{
			if (compactLayout) {
				configureCompactLayer(lines[i].nextLayer);
			} else {
				configureLightLayer(lines[i].nextLayer);
			}
		}
		height += offsets[i];
	}
	numLines = i;

	// Center the animated time block between top status and bottom day lines.
	int topInset = STATUS_BAR_HEIGHT + 2;
	int bottomInset = DAY_LINE_RESERVED_HEIGHT;
	int availableHeight = yres - topInset - bottomInset;
	int ypos = topInset + (availableHeight - height) / 2 + TIME_BLOCK_Y_BIAS;

	// Set y positions for the lines
	for (int i = 0; i < numLines; i++)
	{
		layer_set_frame((Layer *)lines[i].nextLayer, GRect(xres, ypos, xres, ROW_HEIGHT));
		ypos += offsets[i];
	}

	return numLines;
}

void string_to_lines(char *str, char lines[NUM_LINES][BUFFER_SIZE], char format[])
{
	char *start = str;
	char *end = strstr(start, " ");
	int l = 0;

	// Empty all lines
	for (int i = 0; i < NUM_LINES; i++)
	{
		lines[i][0] = '\0';
	}

	while (end != NULL && l < NUM_LINES) {
		// Check word for bold prefix
		if (*start == '*' && end - start > 1)
		{
			// Mark line bold and move start to the first character of the word
			format[l] = 'B';
			start++;
			if (*start == '<') { // small bold, actually
				format[l] = 'b';
				start++;
			}
		}
		else if (*start == '<' && end - start > 1) {
			// Mark line small
			format[l] = 's';
			start++;
		} else {
			// Mark line normal
			format[l] = ' ';
		}

		char *nextWord = end + 1;

		// Can we add another word to the line?
		if (format[l] == ' ' && *nextWord != '*'  // are both lines formatted normal?
			&& *nextWord != ' '                   // no no-join annotation (double space)
			&& strlenUtf8(start, end) < LINE_APPEND_LIMIT)  // is the first word short enough?
		{
			// See if next word fits
			char *try = strstr(end + 1, " ");
			if (try != NULL
				&& strlenUtf8(start, try) <= LINE_LENGTH)
			{
				end = try;
			}
		}

		// copy to line
		*end = '\0';
		strcpy(lines[l++], start);

		// Look for next word
		start = end + 1;
		while (*start == ' ') start++; // Skip all spaces
		end = strstr(start, " ");
	}
}

void time_to_lines(int hours, int minutes, char lines[NUM_LINES][BUFFER_SIZE], char format[])
{
	int length = NUM_LINES * BUFFER_SIZE + 1;
	char timeStr[length];
	time_to_words(hours, minutes, timeStr, length);
	
	string_to_lines(timeStr, lines, format);
}

void day_to_ordinal_words(int day, char *output, size_t length)
{
	get_day_of_month_message(day, output, length);
}

void update_day_of_month_line(struct tm *t, bool force)
{
	if (!force && lastDisplayedDay == t->tm_mday) {
		return;
	}

	lastDisplayedDay = t->tm_mday;
	day_to_ordinal_words(t->tm_mday, dayOfMonthText, sizeof(dayOfMonthText));
	text_layer_set_text(dayOfMonthLayer, dayOfMonthText);
}

// Update screen based on new time
void display_message(char *message, int displayTime)
{
	if (displayTime > 0 && resetMessageTime == 0) {
		// The current time text will be stored in the following strings
		char textLine[NUM_LINES][BUFFER_SIZE];
		char format[NUM_LINES];

		string_to_lines(message, textLine, format);
		
		int nextNLines = configureLayersForText(textLine, format);

		int delay = 0;
		for (int i = 0; i < NUM_LINES; i++) {
		    updateLineTo(&lines[i], textLine[i], delay);
		    delay += ANIMATION_STAGGER_TIME;
		}
		
		currentNLines = nextNLines;

		time(&resetMessageTime);
		resetMessageTime += displayTime;
	}
}

// Update screen based on new time
void display_time(struct tm *t, bool force)
{
	if (resetMessageTime != 0) { // Don't update time if a message is showing
		return;
	}

	time_t timestamp = mktime(t);
	timestamp += timeOffset; // Add offset time
	t = localtime(&timestamp);

#if DEBUG == 0
	if (lastMinute == t->tm_min && !force) { // No change in time
		return;
	}
#endif

	// Mark this minute as checked;
	lastMinute = t->tm_min;

	// The current time text will be stored in the following strings
	char textLine[NUM_LINES][BUFFER_SIZE];
	char format[NUM_LINES];

#if DEBUG == 1
	time_to_lines(t->tm_hour, t->tm_sec, textLine, format);
#else
	time_to_lines(t->tm_hour, t->tm_min, textLine, format);
#endif
	
	int nextNLines = configureLayersForText(textLine, format);

	int delay = 0;
	for (int i = 0; i < NUM_LINES; i++) {
		if (force || nextNLines != currentNLines || needToUpdateLine(&lines[i], textLine[i])) {
			updateLineTo(&lines[i], textLine[i], delay);
			delay += ANIMATION_STAGGER_TIME;
		}
	}
	
	currentNLines = nextNLines;
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Tap event: %d  conf: %d", axis, dateGesture);

  if (dateGesture != GESTURE_ANY) {
  	if (axis == ACCEL_AXIS_X && dateGesture != GESTURE_X) {
	  return;
	}
  	if (axis == ACCEL_AXIS_Y && dateGesture != GESTURE_Y) {
	  return;
	}
  	if (axis == ACCEL_AXIS_Z && dateGesture != GESTURE_Z) {
	  return;
	}
  }

  light_enable(true);
  if (backlightTimer != NULL) {
  	app_timer_cancel(backlightTimer);
  }
  backlightTimer = app_timer_register(1800, backlight_off_handler, NULL);
}

void check_connection(time_t *now) {
	if (connectionLostTime > 0 && connectionLostTime <= *now) {
		if (!connection_service_peek_pebble_app_connection()) {
			notify_bt_lost();
		}
		connectionLostTime = 0;
	}
}

// Time handler called every second by the system
void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
  // If resetMessageTime != 0, then display_time() will not update screen
  bool force = false;
  time_t now;
  time(&now);
  if (resetMessageTime != 0) {
  	if (now >= resetMessageTime) {
  		resetMessageTime = 0;
  		force = true;
  	}
  }

  check_connection(&now);

  update_day_of_month_line(tick_time, false);
  display_time(tick_time, force);
}

void init_line(Line* line) {
	// Create layers with dummy position to the right of the screen
	line->currentLayer = text_layer_create(GRect(xres, 0, xres, ROW_HEIGHT));
	line->nextLayer = text_layer_create(GRect(xres, 0, xres, ROW_HEIGHT));

	// Configure a style
	configureLightLayer(line->currentLayer);
	configureLightLayer(line->nextLayer);

	// Set the text buffers
	line->lineStr1[0] = '\0';
	line->lineStr2[0] = '\0';
	text_layer_set_text(line->currentLayer, line->lineStr1);
	text_layer_set_text(line->nextLayer, line->lineStr2);

	// Initially there are no animations
	line->animation1 = NULL;
	line->animation2 = NULL;
}

struct tm *get_localtime()
{
	time_t raw_time;
	time(&raw_time);
	return localtime(&raw_time);
}

void refresh_time() {
	update_day_of_month_line(get_localtime(), true);
	display_time(get_localtime(), true);
}

void set_offset(int offset) {
	timeOffset = offset;
}

void set_message_time(int mTime) {
	messageTime = mTime;
}

void set_gesture(int gesture) {
	dateGesture = gesture;
	accel_tap_service_unsubscribe();
	if (gesture != GESTURE_OFF) {
		accel_tap_service_subscribe(accel_tap_handler);
	}
}

void set_bt_lost_notification(int bt_notification) {
	bt_lost_notification = bt_notification;
}

void set_meeting_status(int status) {
	if (status < MEETING_STATUS_NONE || status > MEETING_STATUS_NOW) {
		return;
	}
	meetingStatus = status;
	update_status_indicators();
}

void inbox_received_handler(DictionaryIterator *iter, void *context) {
  (void)context;
  Tuple *meeting_status_t = dict_find(iter, KEY_MEETING_STATUS);
  if (meeting_status_t) {
  	set_meeting_status(meeting_status_t->value->uint8);
  }
  config_message_handle_inbox(iter, &config_message_context);
}

void notify_bt_lost() {
	if (bt_lost_notification != BT_NOTIFY_OFF) {
		if (bt_lost_notification == BT_NOTIFY_ON && !quiet_time_is_active()) {
			vibes_long_pulse();
		}
		light_enable_interaction();
		char message[48];
		get_connection_lost_message(message);
		display_message(message, BT_LOST_DISPLAY_TIME);	
	}
}

void bt_handler(bool connected) {
	btConnected = connected;
	update_status_indicators();

	if (connected) {
		connectionLostTime = 0;
	} else {
		time_t now;
		time(&now);
		connectionLostTime = now + CONNECTION_LOST_MARGIN;
	}
}

void battery_handler(BatteryChargeState charge_state) {
	batteryPercent = charge_state.charge_percent;
	update_status_indicators();
}

void readPersistedState() {
	config_message_read_persisted_state(&config_message_context);
}

void handle_init() {
	window = window_create();
	window_stack_push(window, true);

	config_message_context.window = window;
	config_message_context.regular_text_color = &regularTextColor;
	config_message_context.bold_text_color = &boldTextColor;
	config_message_context.set_language = set_language;
	config_message_context.set_offset = set_offset;
	config_message_context.set_message_time = set_message_time;
	config_message_context.set_gesture = set_gesture;
	config_message_context.set_bt_lost_notification = set_bt_lost_notification;
	config_message_context.refresh_time = refresh_time;

	Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    xres = window_bounds.size.w;
    yres = window_bounds.size.h;

	dayOfMonthLayer = text_layer_create(GRect(0, yres - 30, xres, 28));
	text_layer_set_font(dayOfMonthLayer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
	text_layer_set_text_color(dayOfMonthLayer, regularTextColor);
	text_layer_set_background_color(dayOfMonthLayer, GColorClear);
	text_layer_set_text_alignment(dayOfMonthLayer, TEXT_ALIGN);
	dayOfMonthText[0] = '\0';
	text_layer_set_text(dayOfMonthLayer, dayOfMonthText);
	layer_add_child(window_layer, text_layer_get_layer(dayOfMonthLayer));

	btStatusLayer = text_layer_create(GRect(2, 0, xres / 2, STATUS_BAR_HEIGHT));
	text_layer_set_font(btStatusLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_color(btStatusLayer, regularTextColor);
	text_layer_set_background_color(btStatusLayer, GColorClear);
	text_layer_set_text_alignment(btStatusLayer, GTextAlignmentLeft);
	btStatusText[0] = '\0';
	text_layer_set_text(btStatusLayer, btStatusText);
	layer_add_child(window_layer, text_layer_get_layer(btStatusLayer));

	meetingStatusLayer = text_layer_create(GRect(0, 0, xres, STATUS_BAR_HEIGHT));
	// this isn't where the font is set -- look at text_layer_set_font() in meetingStatus branch
	text_layer_set_font(meetingStatusLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_color(meetingStatusLayer, regularTextColor);
	text_layer_set_background_color(meetingStatusLayer, GColorClear);
	text_layer_set_text_alignment(meetingStatusLayer, GTextAlignmentCenter);
	meetingStatusText[0] = '\0';
	text_layer_set_text(meetingStatusLayer, meetingStatusText);
	layer_add_child(window_layer, text_layer_get_layer(meetingStatusLayer));

	batteryStatusLayer = text_layer_create(GRect(xres / 2 - 2, 0, xres / 2, STATUS_BAR_HEIGHT));
	text_layer_set_font(batteryStatusLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
	text_layer_set_text_color(batteryStatusLayer, regularTextColor);
	text_layer_set_background_color(batteryStatusLayer, GColorClear);
	text_layer_set_text_alignment(batteryStatusLayer, GTextAlignmentRight);
	batteryStatusText[0] = '\0';
	text_layer_set_text(batteryStatusLayer, batteryStatusText);
	layer_add_child(window_layer, text_layer_get_layer(batteryStatusLayer));

    // Subscribe to taps
	accel_tap_service_subscribe(accel_tap_handler);


	readPersistedState();

	// Init and load lines
	for (int i = 0; i < NUM_LINES; i++)
	{
		init_line(&lines[i]);
	  	layer_add_child(window_layer, (Layer *)lines[i].currentLayer);
		layer_add_child(window_layer, (Layer *)lines[i].nextLayer);
	}

	// Show greeting message
	char greeting[48];
	time_to_greeting(get_localtime()->tm_hour, greeting);
#if DEBUG == 1
	time_to_greeting(get_localtime()->tm_sec * 24 / 60, greeting);
#endif
	if (messageTime > 0) {
		display_message(greeting, messageTime);
	} else {
		refresh_time();
	}
	// Subscribe to ticks
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);

	// Subscribe to bluetooth events
	connection_service_subscribe((ConnectionHandlers) {
	  .pebble_app_connection_handler = bt_handler
	});
	btConnected = connection_service_peek_pebble_app_connection();

	// Subscribe to battery events
	battery_state_service_subscribe(battery_handler);
	batteryPercent = battery_state_service_peek().charge_percent;
	update_status_indicators();

	// Set up listener for configuration changes
	app_message_register_inbox_received(inbox_received_handler);
  	AppMessageResult result = app_message_open(512, 512);
  	if (result != APP_MSG_OK) {
  		APP_LOG(APP_LOG_LEVEL_WARNING, "app_message_open() failed with error %d", result);
  	}
}

void destroy_line(Line* line)
{
	// Free layers
	text_layer_destroy(line->currentLayer);
	text_layer_destroy(line->nextLayer);
}

void handle_deinit()
{
	// Free lines
	for (int i = 0; i < NUM_LINES; i++)
	{
		destroy_line(&lines[i]);
	}
	text_layer_destroy(dayOfMonthLayer);
	text_layer_destroy(btStatusLayer);
	text_layer_destroy(meetingStatusLayer);
	text_layer_destroy(batteryStatusLayer);
	connection_service_unsubscribe();
	battery_state_service_unsubscribe();

	// Free window
	window_destroy(window);
}

int main(void)
{
	handle_init();
	app_event_loop();
	handle_deinit();
}

