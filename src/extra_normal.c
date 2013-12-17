/**
 * This code is provided under the MIT License.
 * 
 * Copyright (c) 2013 Matthew Congrove (http://github.com/mcongrove)
 * 
 * Utilizes portions of code by Peter Hardy (http://github.com/phardy)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <pebble.h>

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
	
	tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
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
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}