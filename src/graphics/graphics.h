#include "../pet.h"

GBitmap* gbitmap_create_with_resource_safe(uint32_t resource_id);
BitmapLayer* bitmap_layer_create_safe(GRect frame);
void gbitmap_destroy_safe(GBitmap* bitmap);
void bitmap_layer_destroy_safe(BitmapLayer* bitmap_layer);