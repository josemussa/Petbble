#include "pebble.h"
#include "../menu.h"
#include "graphics.h"
#include "petscene.h"

#define ONE_SECOND 1000
#define ANIMATION_STEP_MS (ONE_SECOND * 1)

GBitmap* gbitmap_create_with_resource_safe(uint32_t resource_id) {
	GBitmap* val = gbitmap_create_with_resource(resource_id);
	app_log(0, "graphics.c", 46, "Creating gbitmap %d at pointer %p", (int) resource_id, val);
	if (!val && DEBUG_MODE) {
		app_log(0, "graphics.c", 48, "Ran out of memory while creating gbitmap.");
		GBitmap** runtime_error = NULL;
		return *runtime_error;
	}
	return val;
}

BitmapLayer* bitmap_layer_create_safe(GRect frame) {
	BitmapLayer* val = bitmap_layer_create(frame);
	app_log(0, "graphics.c", 47, "Creating bitmap layer at pointer %p", val);
	if (!val && DEBUG_MODE) {
		BitmapLayer** runtime_error = NULL;
		return *runtime_error;
	}
	return val;
}

void gbitmap_destroy_safe(GBitmap* bitmap) {
	app_log(0, "graphics.c", 66, "Destroying gbitmap at pointer %p", bitmap);
	gbitmap_destroy(bitmap);
}

void bitmap_layer_destroy_safe(BitmapLayer* bitmap_layer) {
	app_log(0, "graphics.c", 71, "Destroying bitmap layer at pointer %p", bitmap_layer);
	bitmap_layer_destroy(bitmap_layer);
}