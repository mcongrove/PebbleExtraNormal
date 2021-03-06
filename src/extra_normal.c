/**
 * This code is provided under the Apache 2.0 license.
 * Please read the LICENSE.md file for more information
 * 
 * Copyright (c) 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Utilizes portions of code by Peter Hardy (http://github.com/phardy)
 */
#include <pebble.h>

static char THEME[6] = "dark";

Window *window;
Layer *hour_layer;
Layer *minute_layer;
Layer *second_layer;
static GPath *hour_path;
static GPath *minute_path;
static GPath *second_path;
TextLayer *label_layer_1;
TextLayer *label_layer_2;
TextLayer *label_layer_3;
TextLayer *label_layer_4;
TextLayer *label_layer_5;
TextLayer *label_layer_6;
TextLayer *label_layer_7;
TextLayer *label_layer_8;
TextLayer *label_layer_9;
TextLayer *label_layer_10;
TextLayer *label_layer_11;
TextLayer *label_layer_12;
InverterLayer *inverter_layer;

enum {
	KEY_THEME = 0x0
};

const GPathInfo HOUR_HAND_PATH_POINTS = {
	4,
	(GPoint[]) {
		{-10, 0},
		{10, 0},
		{10, -55},
		{-10, -55}
	}
};

const GPathInfo MINUTE_HAND_PATH_POINTS = {
	4,
	(GPoint[]) {
		{-4, 0},
		{4, 0},
		{4, -65},
		{-4, -65}
	}
};

static void set_theme() {
	if (persist_exists(KEY_THEME)) {
		persist_read_string(KEY_THEME, THEME, 6);
	}
	
	bool hide = strcmp(THEME, "light") == 0 ? true : false;
	
	layer_set_hidden(inverter_layer_get_layer(inverter_layer), hide);
	
	APP_LOG(APP_LOG_LEVEL_INFO, "SELECTED THEME: %s", THEME);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	Tuple *theme_tuple = dict_find(iter, KEY_THEME);
	
	theme_tuple ? persist_write_string(KEY_THEME, theme_tuple->value->cstring) : false;
	
	set_theme();
}

static void in_dropped_handler(AppMessageResult reason, void *context) {
	
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
	layer_mark_dirty(second_layer);
	
	if (tick_time->tm_sec == 0) {
		layer_mark_dirty(minute_layer);
		
		if (tick_time->tm_min % 2 == 0) {
			layer_mark_dirty(hour_layer);
		}
	}
}

static void hour_display_layer_update_callback(Layer *layer, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	unsigned int angle = (t->tm_hour * 30) + (t->tm_min / 2);
	gpath_rotate_to(hour_path, (TRIG_MAX_ANGLE / 360) * angle);

	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_context_set_stroke_color(ctx, GColorClear);
	gpath_draw_filled(ctx, hour_path);
	gpath_draw_outline(ctx, hour_path);
}

static void minute_display_layer_update_callback(Layer *layer, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	unsigned int angle = (t->tm_min * 6);
	gpath_rotate_to(minute_path, (TRIG_MAX_ANGLE / 360) * angle);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_context_set_stroke_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, minute_path);
	gpath_draw_outline(ctx, minute_path);
}

static void second_display_layer_update_callback(Layer *layer, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	GRect bounds = layer_get_bounds(layer);
	const GPoint center = grect_center_point(&bounds);
	const int16_t secondHandLength = bounds.size.w / 1.9;
	
	GPoint secondHand;
	
	int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
	secondHand.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.y;
	secondHand.x = (int16_t)(sin_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.x;
	
	graphics_context_set_stroke_color(ctx, GColorBlack);
	graphics_draw_line(ctx, secondHand, center);
	
	graphics_context_set_fill_color(ctx, GColorBlack);
	graphics_fill_circle(ctx, center, 11);
}

void init_text_layers() {
	label_layer_1 = text_layer_create(GRect(92, 31, 12, 30));
	text_layer_set_text_color(label_layer_1, GColorWhite);
	text_layer_set_background_color(label_layer_1, GColorClear);
	text_layer_set_text_alignment(label_layer_1, GTextAlignmentLeft);
	text_layer_set_text(label_layer_1, "1");
	text_layer_set_font(label_layer_1, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_1));
	
	label_layer_2 = text_layer_create(GRect(110, 48, 12, 30));
	text_layer_set_text_color(label_layer_2, GColorWhite);
	text_layer_set_background_color(label_layer_2, GColorClear);
	text_layer_set_text_alignment(label_layer_2, GTextAlignmentLeft);
	text_layer_set_text(label_layer_2, "2");
	text_layer_set_font(label_layer_2, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_2));
	
	label_layer_3 = text_layer_create(GRect(116, 72, 12, 30));
	text_layer_set_text_color(label_layer_3, GColorWhite);
	text_layer_set_background_color(label_layer_3, GColorClear);
	text_layer_set_text_alignment(label_layer_3, GTextAlignmentLeft);
	text_layer_set_text(label_layer_3, "3");
	text_layer_set_font(label_layer_3, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_3));
	
	label_layer_4 = text_layer_create(GRect(108, 95, 12, 30));
	text_layer_set_text_color(label_layer_4, GColorWhite);
	text_layer_set_background_color(label_layer_4, GColorClear);
	text_layer_set_text_alignment(label_layer_4, GTextAlignmentLeft);
	text_layer_set_text(label_layer_4, "4");
	text_layer_set_font(label_layer_4, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_4));
	
	label_layer_5 = text_layer_create(GRect(91, 112, 12, 30));
	text_layer_set_text_color(label_layer_5, GColorWhite);
	text_layer_set_background_color(label_layer_5, GColorClear);
	text_layer_set_text_alignment(label_layer_5, GTextAlignmentLeft);
	text_layer_set_text(label_layer_5, "5");
	text_layer_set_font(label_layer_5, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_5));
	
	label_layer_6 = text_layer_create(GRect(68, 117, 12, 30));
	text_layer_set_text_color(label_layer_6, GColorWhite);
	text_layer_set_background_color(label_layer_6, GColorClear);
	text_layer_set_text_alignment(label_layer_6, GTextAlignmentLeft);
	text_layer_set_text(label_layer_6, "6");
	text_layer_set_font(label_layer_6, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_6));
	
	label_layer_7 = text_layer_create(GRect(46, 111, 12, 30));
	text_layer_set_text_color(label_layer_7, GColorWhite);
	text_layer_set_background_color(label_layer_7, GColorClear);
	text_layer_set_text_alignment(label_layer_7, GTextAlignmentLeft);
	text_layer_set_text(label_layer_7, "7");
	text_layer_set_font(label_layer_7, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_7));
	
	label_layer_8 = text_layer_create(GRect(28, 95, 12, 30));
	text_layer_set_text_color(label_layer_8, GColorWhite);
	text_layer_set_background_color(label_layer_8, GColorClear);
	text_layer_set_text_alignment(label_layer_8, GTextAlignmentLeft);
	text_layer_set_text(label_layer_8, "8");
	text_layer_set_font(label_layer_8, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_8));
	
	label_layer_9 = text_layer_create(GRect(21, 72, 12, 30));
	text_layer_set_text_color(label_layer_9, GColorWhite);
	text_layer_set_background_color(label_layer_9, GColorClear);
	text_layer_set_text_alignment(label_layer_9, GTextAlignmentLeft);
	text_layer_set_text(label_layer_9, "9");
	text_layer_set_font(label_layer_9, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_9));
	
	label_layer_10 = text_layer_create(GRect(26, 50, 25, 30));
	text_layer_set_text_color(label_layer_10, GColorWhite);
	text_layer_set_background_color(label_layer_10, GColorClear);
	text_layer_set_text_alignment(label_layer_10, GTextAlignmentLeft);
	text_layer_set_text(label_layer_10, "10");
	text_layer_set_font(label_layer_10, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_10));
	
	label_layer_11 = text_layer_create(GRect(43, 33, 25, 30));
	text_layer_set_text_color(label_layer_11, GColorWhite);
	text_layer_set_background_color(label_layer_11, GColorClear);
	text_layer_set_text_alignment(label_layer_11, GTextAlignmentLeft);
	text_layer_set_text(label_layer_11, "11");
	text_layer_set_font(label_layer_11, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_11));
	
	label_layer_12 = text_layer_create(GRect(64, 25, 25, 30));
	text_layer_set_text_color(label_layer_12, GColorWhite);
	text_layer_set_background_color(label_layer_12, GColorClear);
	text_layer_set_text_alignment(label_layer_12, GTextAlignmentLeft);
	text_layer_set_text(label_layer_12, "12");
	text_layer_set_font(label_layer_12, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(label_layer_12));
}

static void init() {
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_open(64, 0);
	
	window = window_create();
	window_set_background_color(window, GColorWhite);
	window_stack_push(window, true);
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	// Add hours layer
	hour_layer = layer_create(bounds);
	layer_set_update_proc(hour_layer, hour_display_layer_update_callback);
	layer_add_child(window_layer, hour_layer);
	hour_path = gpath_create(&HOUR_HAND_PATH_POINTS);
	gpath_move_to(hour_path, grect_center_point(&bounds));
	
	// Add text layers
	init_text_layers();
	
	// Add minutes layer
	minute_layer = layer_create(bounds);
	layer_set_update_proc(minute_layer, minute_display_layer_update_callback);
	layer_add_child(window_layer, minute_layer);
	minute_path = gpath_create(&MINUTE_HAND_PATH_POINTS);
	gpath_move_to(minute_path, grect_center_point(&bounds));
	
	// Add seconds layer
	second_layer = layer_create(bounds);
	layer_set_update_proc(second_layer, second_display_layer_update_callback);
	layer_add_child(window_layer, second_layer);
	
	// Create the inverter layer
	inverter_layer = inverter_layer_create(bounds);
	layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));
	
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
	
	set_theme();
}

static void deinit() {
	window_destroy(window);
	layer_destroy(hour_layer);
	layer_destroy(minute_layer);
	layer_destroy(second_layer);
	gpath_destroy(hour_path);
	gpath_destroy(minute_path);
	gpath_destroy(second_path);
	text_layer_destroy(label_layer_1);
	text_layer_destroy(label_layer_2);
	text_layer_destroy(label_layer_3);
	text_layer_destroy(label_layer_4);
	text_layer_destroy(label_layer_5);
	text_layer_destroy(label_layer_6);
	text_layer_destroy(label_layer_7);
	text_layer_destroy(label_layer_8);
	text_layer_destroy(label_layer_9);
	text_layer_destroy(label_layer_10);
	text_layer_destroy(label_layer_11);
	text_layer_destroy(label_layer_12);
	inverter_layer_destroy(inverter_layer);
	
	tick_timer_service_unsubscribe();
	app_message_deregister_callbacks();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}