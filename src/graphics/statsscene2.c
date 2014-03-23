#include "statsscene2.h"
#include "graphics.h"
#include "pet.h"

#define MAX_ROW 8

static Window *window;
static BitmapLayer *heart_layer[MAX_ROW][4];
static GBitmap *heart;
static GBitmap *heart_outline;
static TextLayer *text[MAX_ROW];
static char *weight = NULL;
static char *age = NULL;

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

void graphics_generate_stats_scene2(Window *w, Pet *p) {
	window = w;
	heart = gbitmap_create_with_resource_safe(RESOURCE_ID_HEART);
	heart_outline = gbitmap_create_with_resource_safe(RESOURCE_ID_HEART_OUTLINE);
	init_text("Energy", 0);
	init_hearts(p, ENERGY_KEY, 1);

	int weight_int = p->fields[WEIGHT_KEY];
	if (weight == NULL) {
		// weight = malloc(weight_int % 10 + 1);	// number of digits in weight, plus one for null termination
		weight = malloc(10);		// This is really hacky and I don't like this solution, but I am getting really weird behavior when trying
									// to free strings that were used in text layers.
	}
	snprintf(weight, weight_int / 10 + 1, "%d", weight_int);

	weight[weight_int / 10] = '\0';

	init_text("Weight", 2);
	init_text(weight, 3);

	int age_in_years = pet_calculate_age(p);
	if (age == NULL) {
		// age = malloc(age_in_years % 10 + 1);
		age = malloc(10);
	}
	snprintf(age, age_in_years / 10 + 1, "%d", age_in_years);
	age[age_in_years / 10] = '\0';

	init_text("Age", 4);
	init_text(age, 5);
}

void graphics_destroy_stats_scene2() {
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
	// if (weight != NULL) {			// Leaking memory, but app is crashing when I try to free these for some reason.
	// 	free(weight);
	// }
	// if (age != NULL) {
	// 	free(age);
	// }
}