#include "pebble.h"
#include "clockscene.h"
#include "graphics.h"

static GBitmap *lightoff;

static BitmapLayer *lightsoff_layer;
TextLayer *time_layer; // The clock

static void animate_clock_callback(struct tm *tick_time, TimeUnits units_changed) {
    static char time_text[] = "00:00:00"; // Needs to be static because it's used by the system later.
    strftime(time_text, sizeof(time_text), "%T", tick_time);
    text_layer_set_text(time_layer, time_text);
}

void graphics_generate_clock_scene(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    lightsoff_layer = bitmap_layer_create_safe(bounds);

	time_layer = text_layer_create(GRect(29, 54, 144-40 /* width */, 168-54 /* height */));
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    
    // Ensures time is displayed immediately (will break if NULL tick event accessed).
    // (This is why it's a good idea to have a separate routine to do the update itself.)
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    animate_clock_callback(current_time, SECOND_UNIT);
    tick_timer_service_subscribe(SECOND_UNIT, &animate_clock_callback);
    
    lightoff = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_LIGHTSOFF);
    bitmap_layer_set_bitmap(lightsoff_layer, lightoff);
    layer_add_child(window_layer, bitmap_layer_get_layer(lightsoff_layer));
    layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

void graphics_destroy_clock_scene() {
	text_layer_destroy(time_layer);
	gbitmap_destroy_safe(lightoff);
	bitmap_layer_destroy_safe(lightsoff_layer);
	tick_timer_service_unsubscribe();
}