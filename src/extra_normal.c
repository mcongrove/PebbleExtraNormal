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

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x00, 0xB9, 0x24, 0x31, 0x45, 0x6B, 0x4E, 0xB7, 0xB2, 0x7D, 0x4A, 0x7E, 0x36, 0xD3, 0x8D, 0x59 }

PBL_APP_INFO(MY_UUID, "Extra Normal", "Matthew Congrove", 1, 0, DEFAULT_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;
GRect AnalogueGRect;
Layer AnalogueHourLayer, AnalogueMinuteLayer, AnalogueSecondLayer;
GPath AnalogueHourPath, AnalogueMinutePath, AnalogueSecondPath;
TextLayer HourLabelLayer1, HourLabelLayer2, HourLabelLayer3, HourLabelLayer4, HourLabelLayer5, HourLabelLayer6, HourLabelLayer7, HourLabelLayer8, HourLabelLayer9, HourLabelLayer10, HourLabelLayer11, HourLabelLayer12;

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

void init_layer_path_and_center(Layer *layer, GPath *path, const GPathInfo *pathInfo, const void *updateProc) {
	layer_init(layer, AnalogueGRect);
	layer->update_proc = updateProc;
	
	gpath_init(path, pathInfo);
	gpath_move_to(path, grect_center_point(&layer->frame));
}

void init_layer_line(Layer *layer, const void *updateProc) {
	layer_init(layer, AnalogueGRect);
	layer->update_proc = updateProc;
}

void hour_display_layer_update_callback(Layer *me, GContext* ctx) {
	(void)me;

	PblTm t;
	get_time(&t);

	unsigned int angle = (t.tm_hour * 30) + (t.tm_min / 2);
	gpath_rotate_to(&AnalogueHourPath, (TRIG_MAX_ANGLE / 360) * angle);

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorBlack);
	gpath_draw_filled(ctx, &AnalogueHourPath);
	gpath_draw_outline(ctx, &AnalogueHourPath);
}

void minute_display_layer_update_callback(Layer *me, GContext* ctx) {
	(void)me;

	PblTm t;
	get_time(&t);

	unsigned int angle = (t.tm_min * 6);
	gpath_rotate_to(&AnalogueMinutePath, (TRIG_MAX_ANGLE / 360) * angle);

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_context_set_stroke_color(ctx, GColorWhite);
	gpath_draw_filled(ctx, &AnalogueMinutePath);
	gpath_draw_outline(ctx, &AnalogueMinutePath);
}

void second_display_layer_update_callback(Layer *me, GContext* ctx) {
	PblTm t;
	get_time(&t);
	
	GPoint secondHand;	
	const GPoint center = grect_center_point(&me->bounds);
	const int16_t secondHandLength = (me->bounds.size.w / 1.9);

	int32_t second_angle = (TRIG_MAX_ANGLE * t.tm_sec / 60);
	secondHand.y = (int16_t)(-cos_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.y;
	secondHand.x = (int16_t)(sin_lookup(second_angle) * (int32_t)secondHandLength / TRIG_MAX_RATIO) + center.x;
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, secondHand, center);

	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, grect_center_point(&me->frame), 11);
}

void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {
	layer_mark_dirty(&AnalogueSecondLayer);

	if (t->tick_time->tm_sec == 0) {
		layer_mark_dirty(&AnalogueMinuteLayer);
		
		if (t->tick_time->tm_min % 2 == 0) {
			layer_mark_dirty(&AnalogueHourLayer);
		}
	}
}

void init_text_layers() {
	text_layer_init(&HourLabelLayer1, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer1, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer1, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer1, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer1.layer, GRect(92, 31, 12, 30));
	text_layer_set_text(&HourLabelLayer1, "1");
	text_layer_set_font(&HourLabelLayer1, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer1.layer);
	
	text_layer_init(&HourLabelLayer2, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer2, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer2, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer2, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer2.layer, GRect(110, 48, 12, 30));
	text_layer_set_text(&HourLabelLayer2, "2");
	text_layer_set_font(&HourLabelLayer2, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer2.layer);
	
	text_layer_init(&HourLabelLayer3, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer3, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer3, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer3, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer3.layer, GRect(116, 72, 12, 30));
	text_layer_set_text(&HourLabelLayer3, "3");
	text_layer_set_font(&HourLabelLayer3, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer3.layer);
	
	text_layer_init(&HourLabelLayer4, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer4, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer4, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer4, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer4.layer, GRect(108, 95, 12, 30));
	text_layer_set_text(&HourLabelLayer4, "4");
	text_layer_set_font(&HourLabelLayer4, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer4.layer);
	
	text_layer_init(&HourLabelLayer5, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer5, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer5, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer5, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer5.layer, GRect(91, 112, 12, 30));
	text_layer_set_text(&HourLabelLayer5, "5");
	text_layer_set_font(&HourLabelLayer5, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer5.layer);
	
	text_layer_init(&HourLabelLayer6, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer6, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer6, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer6, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer6.layer, GRect(68, 117, 12, 30));
	text_layer_set_text(&HourLabelLayer6, "6");
	text_layer_set_font(&HourLabelLayer6, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer6.layer);
	
	text_layer_init(&HourLabelLayer7, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer7, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer7, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer7, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer7.layer, GRect(46, 111, 12, 30));
	text_layer_set_text(&HourLabelLayer7, "7");
	text_layer_set_font(&HourLabelLayer7, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer7.layer);
	
	text_layer_init(&HourLabelLayer8, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer8, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer8, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer8, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer8.layer, GRect(28, 95, 12, 30));
	text_layer_set_text(&HourLabelLayer8, "8");
	text_layer_set_font(&HourLabelLayer8, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer8.layer);
	
	text_layer_init(&HourLabelLayer9, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer9, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer9, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer9, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer9.layer, GRect(21, 72, 12, 30));
	text_layer_set_text(&HourLabelLayer9, "9");
	text_layer_set_font(&HourLabelLayer9, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer9.layer);
	
	text_layer_init(&HourLabelLayer10, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer10, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer10, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer10, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer10.layer, GRect(26, 50, 25, 30));
	text_layer_set_text(&HourLabelLayer10, "10");
	text_layer_set_font(&HourLabelLayer10, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer10.layer);
	
	text_layer_init(&HourLabelLayer11, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer11, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer11, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer11, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer11.layer, GRect(43, 33, 25, 30));
	text_layer_set_text(&HourLabelLayer11, "11");
	text_layer_set_font(&HourLabelLayer11, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer11.layer);
	
	text_layer_init(&HourLabelLayer12, window.layer.frame);
	text_layer_set_text_color(&HourLabelLayer12, GColorBlack);
	text_layer_set_background_color(&HourLabelLayer12, GColorClear);
	text_layer_set_text_alignment(&HourLabelLayer12, GTextAlignmentLeft);
	layer_set_frame(&HourLabelLayer12.layer, GRect(64, 25, 25, 30));
	text_layer_set_text(&HourLabelLayer12, "12");
	text_layer_set_font(&HourLabelLayer12, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	layer_add_child(&window.layer, &HourLabelLayer12.layer);
}

void display_init(AppContextRef *ctx) {
	window_init(&window, "Extra Normal");
	window_set_fullscreen(&window, true);
	window_set_background_color(&window, GColorBlack);
	window_stack_push(&window, true);

	init_layer_line(&AnalogueSecondLayer, &second_display_layer_update_callback);
	init_layer_path_and_center(&AnalogueMinuteLayer, &AnalogueMinutePath, &MINUTE_HAND_PATH_POINTS, &minute_display_layer_update_callback);
	init_layer_path_and_center(&AnalogueHourLayer, &AnalogueHourPath, &HOUR_HAND_PATH_POINTS, &hour_display_layer_update_callback);
	
	layer_add_child(&window.layer, &AnalogueHourLayer);
	init_text_layers();
	layer_add_child(&window.layer, &AnalogueMinuteLayer);
	layer_add_child(&window.layer, &AnalogueSecondLayer);
}

void handle_init(AppContextRef ctx) {
	AnalogueGRect = GRect(0, 0, 144, 168);
	
	display_init(&ctx);

	layer_mark_dirty(&AnalogueSecondLayer);
	layer_mark_dirty(&AnalogueMinuteLayer);
	layer_mark_dirty(&AnalogueHourLayer);
}

void pbl_main(void *params) {
	PebbleAppHandlers handlers = {
		.init_handler = &handle_init,
		.tick_info = {
			.tick_handler = &handle_second_tick,
			.tick_units = SECOND_UNIT
		}
	};
	
	app_event_loop(params, &handlers);
}