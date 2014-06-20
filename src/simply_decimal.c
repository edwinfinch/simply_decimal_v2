#include <pebble.h>
#include <math.h>
#include "mini-printf.h"
	
Window *window;

TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_dt_layer;

Layer *line_layer;

int hour, min, sec;

AppTimer *timer;

unsigned int decimal_time;
int decimal_hour;
int decimal_minute;
int decimal_seconds;

#define COOKIE_DECIMAL_TIME_TIMER 1
#define DECIMAL_TIME_CONVERSION_MULTIPLIER 1.157
// current time zone offset 
#define TIME_ZONE_OFFSET 0
static char date_text[] = "Xxxxxxxxx 00";
static char time_string[8];

void line_layer_update_callback(Layer *me, GContext* ctx) {
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_draw_line(ctx, GPoint(8, 97), GPoint(131, 97));
	graphics_draw_line(ctx, GPoint(8, 98), GPoint(131, 98));
}

void timer_callback(void *data){
    memset(&time_string[0], 0, sizeof(time_string));
    snprintf(time_string, 8, "%d:%02d", decimal_hour, decimal_minute);

    text_layer_set_text(text_time_layer, time_string);
	text_layer_set_text(text_dt_layer, "dt");

	timer = app_timer_register(50, (AppTimerCallback) timer_callback, NULL);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	hour = tick_time->tm_hour;
	min = tick_time->tm_min;
	sec = tick_time->tm_sec;
	
	decimal_time = floor(((hour * 3600) + (min * 60) + sec) / .864);

	decimal_hour = floor(decimal_time / 10000);
	decimal_time = decimal_time - (decimal_hour * 10000);

	decimal_minute = floor(decimal_time / 100);
	decimal_time = decimal_time - (decimal_minute * 100);
  
	decimal_seconds = decimal_time;
	
	strftime(date_text, sizeof(date_text), "%B %e", tick_time);
	text_layer_set_text(text_date_layer, date_text);
}

void init(void) {
	window = window_create();
	window_set_background_color(window, GColorBlack);
	
	Layer *window_layer = window_get_root_layer(window);
	
	text_date_layer = text_layer_create(GRect(0, 0, 144, 168));
	text_layer_set_text_color(text_date_layer, GColorWhite);
	text_layer_set_background_color(text_date_layer, GColorClear);
	layer_set_frame(text_layer_get_layer(text_date_layer), GRect(8, 68, 144-8, 168-68));
	text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	layer_add_child(window_layer, text_layer_get_layer(text_date_layer));
	
	text_time_layer = text_layer_create(GRect(0, 0, 144, 168));
	text_layer_set_text_color(text_time_layer, GColorWhite);
	text_layer_set_background_color(text_time_layer, GColorClear);
	layer_set_frame(text_layer_get_layer(text_time_layer), GRect(7, 92, 144-7, 168-92));
	text_layer_set_font(text_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(window_layer, text_layer_get_layer(text_time_layer));
	
	text_dt_layer = text_layer_create(GRect(0, 0, 144, 168));
	text_layer_set_text_color(text_dt_layer, GColorWhite);
	text_layer_set_background_color(text_dt_layer, GColorClear);
	layer_set_frame(text_layer_get_layer(text_dt_layer), GRect(110, 120, 144-110, 168-120));
	text_layer_set_font(text_dt_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
	layer_add_child(window_layer, text_layer_get_layer(text_dt_layer));
	
	line_layer = layer_create(GRect(0, 0, 144, 168));
	layer_set_update_proc(line_layer, line_layer_update_callback);
	layer_add_child(window_layer, line_layer);
	
	timer = app_timer_register(50, (AppTimerCallback) timer_callback, NULL);
	tick_timer_service_subscribe(SECOND_UNIT, &tick_handler);
	window_stack_push(window, true);
}

void deinit(void){
	window_destroy(window);
}

int main(){
	init();
	app_event_loop();
	deinit();
}