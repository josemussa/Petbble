#include "statsscene.h"
#include "graphics.h"
#include "pet.h"

#define MAX_ROW 8

static Window *window;
static BitmapLayer *heart_layer[MAX_ROW][4];
static GBitmap *heart;
static GBitmap *heart_outline;
static TextLayer *text[MAX_ROW];

static void init_hearts(Pet *p, int key, int row) {
	Layer *window_layer = window_get_root_layer(window);
	int size_of_heart = 16;
	int height = row * 16;
	int leftmost_heart = 144 / 2 - (size_of_heart * 2);
	GRect bounds[4];
	//144 x 168
	for (int i = 0; i < 4; i++) {
		bounds[i].origin = GPoint(leftmost_heart + (size_of_heart * i), height);
		bounds[i].size = GSize(size_of_heart, size_of_heart);
		heart_layer[row][i] = bitmap_layer_create_safe(bounds[i]);
		bitmap_layer_set_alignment(heart_layer[row][i], GAlignCenter);
		if (p->fields[key] > i) {
			bitmap_layer_set_bitmap(heart_layer[row][i], heart);
		} else {
			bitmap_layer_set_bitmap(heart_layer[row][i], heart_outline);
		}
		layer_add_child(window_layer, bitmap_layer_get_layer(heart_layer[row][i]));
	}
}

static void init_text(char* stat, int row) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds;
	bounds.origin = GPoint(0, row * 16);
	bounds.size = GSize(144, 16);
	text[row] = text_layer_create(bounds);
	text_layer_set_text(text[row], stat);
	text_layer_set_text_alignment(text[row], GTextAlignmentCenter);
	layer_add_child(window_layer, text_layer_get_layer(text[row]));
}

void graphics_generate_stats_scene(Window *w, Pet *p) {
	window = w;
	heart = gbitmap_create_with_resource_safe(RESOURCE_ID_HEART);
	heart_outline = gbitmap_create_with_resource_safe(RESOURCE_ID_HEART_OUTLINE);
	init_text("Health", 0);
	init_hearts(p, HEALTH_KEY, 1);
	
	init_text("Happiness", 2);
	init_hearts(p, HAPPINESS_KEY, 3);

	init_text("Satiation", 4);
	init_hearts(p, HUNGER_KEY, 5);

	init_text("Discipline", 6);
	init_hearts(p, DISCIPLINE_KEY, 7);
}

void graphics_destroy_stats_scene() {
	gbitmap_destroy_safe(heart);
	gbitmap_destroy_safe(heart_outline);
	for (int i = 0; i < MAX_ROW; i++) {
		for (int j = 0; j < 4; j++) {
			if (heart_layer[i][j] != NULL) {
				bitmap_layer_destroy_safe(heart_layer[i][j]);
			}
		}
		if (text[i] != NULL) {
			text_layer_destroy(text[i]);
		}
	}
}