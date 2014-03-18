#include "pebble.h"
#include "menu.h"
#include "graphics.h"

#define ONE_SECOND 1000
#define ANIMATION_STEP_MS (ONE_SECOND * 1)

static BitmapLayer *pizza_layer;
static BitmapLayer *bulb_layer;
static BitmapLayer *park_layer;
static BitmapLayer *pill_layer;
static BitmapLayer *bath_layer;
static BitmapLayer *health_layer;
static BitmapLayer *discipline_layer;
static BitmapLayer *call_layer;

static GBitmap *pizza;
static GBitmap *bulb;
static GBitmap *park;
static GBitmap *pill;
static GBitmap *bath;
static GBitmap *health;
static GBitmap *discipline;
static GBitmap *call;

int currentScene;

static BitmapLayer *pet_layer;
#define MAX_ANIMATION_FRAMES 10
static GBitmap *pet_sprites[NUM_PET_STAGES][MAX_ANIMATION_FRAMES + 1];
static int animationCounter = 0;
static AppTimer *pet_animation_timer;
static GBitmap *lightoff;

static BitmapLayer *lightsoff_layer;
TextLayer *time_layer; // The clock

static Pet *p;

// function definitions
static void animate_pet_timer_callback(void *data);
static void animate_clock_callback(struct tm *tick_time, TimeUnits units_changed);

// INITIALIZATION STUFF

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

static void generate_icons(Window *window){
    
    Layer *window_layer = window_get_root_layer(window);
    
    pizza = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_PIZZA);
    bulb = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BULB);
    park = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_PARK);
    pill = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_PILL);
    bath = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BATH);
    health = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_HEALTH);
    discipline = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_DISCIPLINE);
    call = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_CALL);

    // PIZZA
    pizza_layer = bitmap_layer_create_safe(GRect(8,2,20, 20));
    bitmap_layer_set_bitmap(pizza_layer, pizza);
    layer_add_child(window_layer, bitmap_layer_get_layer(pizza_layer));
    
    // BULB
    bulb_layer = bitmap_layer_create_safe(GRect(44,2,20,20));
    bitmap_layer_set_bitmap(bulb_layer, bulb);
    layer_add_child(window_layer, bitmap_layer_get_layer(bulb_layer));
    
    // PARK
    park_layer = bitmap_layer_create_safe(GRect(80,2,20,20));
    bitmap_layer_set_bitmap(park_layer, park);
    layer_add_child(window_layer, bitmap_layer_get_layer(park_layer));
    
    // PILL
    pill_layer = bitmap_layer_create_safe(GRect(116,2,20,20));
    bitmap_layer_set_bitmap(pill_layer, pill);
    layer_add_child(window_layer, bitmap_layer_get_layer(pill_layer));
    
    // BATH
    bath_layer = bitmap_layer_create_safe(GRect(8,130,20, 20));
    bitmap_layer_set_bitmap(bath_layer, bath);
    layer_add_child(window_layer, bitmap_layer_get_layer(bath_layer));
    
    // HEALTH
    health_layer = bitmap_layer_create_safe(GRect(44,130,20, 20));
    bitmap_layer_set_bitmap(health_layer, health);
    layer_add_child(window_layer, bitmap_layer_get_layer(health_layer));
    
    // DISCIPLINE
    discipline_layer = bitmap_layer_create_safe(GRect(80,130,20, 20));
    bitmap_layer_set_bitmap(discipline_layer, discipline);
    layer_add_child(window_layer, bitmap_layer_get_layer(discipline_layer));
    
    // CALL
    call_layer = bitmap_layer_create_safe(GRect(116,130,20, 20));
    bitmap_layer_set_bitmap(call_layer, call);
    layer_add_child(window_layer, bitmap_layer_get_layer(call_layer));
    
}

static void deallocate_pet_sprite(int previous_stage) {
	for (int i = 0; i < MAX_ANIMATION_FRAMES; i++) {
		if (pet_sprites[previous_stage][i]) {
			gbitmap_destroy_safe(pet_sprites[previous_stage][i]);
		}
	}
}

static void generatePet(int current_stage) {
    switch (current_stage) {
        case EGG_STAGE:
        	pet_sprites[EGG_STAGE][0] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EGG1);
        	pet_sprites[EGG_STAGE][1] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EGG2);
        	pet_sprites[EGG_STAGE][2] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EGG3);
        	pet_sprites[EGG_STAGE][3] = NULL;
            break;
        case BABITCHI_STAGE:
            // BABITCHI
            pet_sprites[BABITCHI_STAGE][0] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI1);
            pet_sprites[BABITCHI_STAGE][1] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI2);
            pet_sprites[BABITCHI_STAGE][2] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI3);
            pet_sprites[BABITCHI_STAGE][3] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI4);
            pet_sprites[BABITCHI_STAGE][4] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI5);
            pet_sprites[BABITCHI_STAGE][5] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI6);
            pet_sprites[BABITCHI_STAGE][6] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI7);
            pet_sprites[BABITCHI_STAGE][7] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI8);
            pet_sprites[BABITCHI_STAGE][8] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI9);
            pet_sprites[BABITCHI_STAGE][9] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_BABITCHI10);
            pet_sprites[BABITCHI_STAGE][10] = NULL;
            break;
        case KUCHIT_STAGE:
            // KUCHIT
            pet_sprites[KUCHIT_STAGE][0] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT1);
            pet_sprites[KUCHIT_STAGE][1] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT2);
            pet_sprites[KUCHIT_STAGE][2] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT3);
            pet_sprites[KUCHIT_STAGE][3] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT4);
            pet_sprites[KUCHIT_STAGE][4] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT5);
            pet_sprites[KUCHIT_STAGE][5] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT6);
            pet_sprites[KUCHIT_STAGE][6] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT7);
            pet_sprites[KUCHIT_STAGE][7] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_KUCHIT8);
            pet_sprites[KUCHIT_STAGE][8] = NULL;
            break;
        case RETURN_STAGE:
            pet_sprites[RETURN_STAGE][0] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_RETURN1);
    		pet_sprites[RETURN_STAGE][1] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_RETURN2);
    		pet_sprites[RETURN_STAGE][2] = NULL;
    		break;
        default:
            break;
    }
}

void graphics_generate_pet_scene(Window *window, Pet *pet) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    p = pet;
    
    // This needs to be deinited on app exit which is when the event loop ends
    generatePet(p->fields[CURRENT_STAGE_KEY]);
    generate_icons(window);
    
    // CREATE PET
    pet_layer = bitmap_layer_create_safe(bounds);
    bitmap_layer_set_alignment(pet_layer, GAlignCenter);
    layer_add_child(window_layer, bitmap_layer_get_layer(pet_layer));
    
    app_timer_register(SECOND_UNIT, animate_pet_timer_callback, NULL);
}

static void graphics_generate_clock_scene(Window *window) {
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

// DEINITIALIZATION STUFF

static void destroy_icons() {
    gbitmap_destroy_safe(bulb);
    gbitmap_destroy_safe(pizza);
    gbitmap_destroy_safe(park);
    gbitmap_destroy_safe(pill);
    gbitmap_destroy_safe(bath);
    gbitmap_destroy_safe(health);
    gbitmap_destroy_safe(discipline);
    gbitmap_destroy_safe(call);
    bitmap_layer_destroy_safe(pizza_layer);
    bitmap_layer_destroy_safe(bulb_layer);
    bitmap_layer_destroy_safe(park_layer);
    bitmap_layer_destroy_safe(pill_layer);
    bitmap_layer_destroy_safe(bath_layer);
    bitmap_layer_destroy_safe(health_layer);
    bitmap_layer_destroy_safe(discipline_layer);
    bitmap_layer_destroy_safe(call_layer);
}

void graphics_destroy_current_scene() {
	switch (currentScene) {
		case PET_SCENE:
			if (p != NULL) {
				deallocate_pet_sprite(p->fields[CURRENT_STAGE_KEY]);
			}
    		bitmap_layer_destroy_safe(pet_layer);
    		destroy_icons();
    		app_timer_cancel(pet_animation_timer);
			break;
		case CLOCK_SCENE:
			text_layer_destroy(time_layer);
			gbitmap_destroy_safe(lightoff);
			bitmap_layer_destroy_safe(lightsoff_layer);
			tick_timer_service_unsubscribe();
			break;
	}
}

// ANIMATION STUFF

static void animate_pet_timer_callback(void *data) {
    animationCounter += 1;
	int level = p->fields[CURRENT_STAGE_KEY];
	if (pet_sprites[level][animationCounter] == NULL) {
		animationCounter = 0;
	}
	bitmap_layer_set_bitmap(pet_layer, pet_sprites[level][animationCounter]);
    pet_animation_timer = app_timer_register(ANIMATION_STEP_MS, animate_pet_timer_callback, NULL);
}

static void animate_clock_callback(struct tm *tick_time, TimeUnits units_changed) {
    static char time_text[] = "00:00:00"; // Needs to be static because it's used by the system later.
    strftime(time_text, sizeof(time_text), "%T", tick_time);
    text_layer_set_text(time_layer, time_text);
}

static void clear_buttons(){
    bitmap_layer_set_background_color(pizza_layer, GColorWhite);
    bitmap_layer_set_background_color(bulb_layer, GColorWhite);
    bitmap_layer_set_background_color(park_layer, GColorWhite);
    bitmap_layer_set_background_color(pill_layer, GColorWhite);
    bitmap_layer_set_background_color(bath_layer, GColorWhite);
    bitmap_layer_set_background_color(health_layer, GColorWhite);
    bitmap_layer_set_background_color(discipline_layer, GColorWhite);
    bitmap_layer_set_background_color(call_layer, GColorWhite);
}

void update_menu(int item_menu) {
    switch (item_menu) {
        case 0:
            //PIZZA
            clear_buttons();
            bitmap_layer_set_background_color(pizza_layer, GColorBlack);
            break;
        case 1:
            //BULB
            clear_buttons();
            bitmap_layer_set_background_color(bulb_layer, GColorBlack);
            break;
        case 2:
            //PARK
            clear_buttons();
            bitmap_layer_set_background_color(park_layer, GColorBlack);
            break;
        case 3:
            //PILL
            clear_buttons();
            bitmap_layer_set_background_color(pill_layer, GColorBlack);
            break;
        case 4:
            //BATH
            clear_buttons();
            bitmap_layer_set_background_color(bath_layer, GColorBlack);
            break;
        case 5:
            //HEALTH
            clear_buttons();
            bitmap_layer_set_background_color(health_layer, GColorBlack);
            break;
        case 6:
            //DISCIPLINE
            clear_buttons();
            bitmap_layer_set_background_color(discipline_layer, GColorBlack);
            break;
        case 7:
            //CALL
            clear_buttons();
            bitmap_layer_set_background_color(call_layer, GColorBlack);
            break;
        default:
            break;
    }

}

// SCENE SWITCHING STUFF

void switchScene(int scene, Window *window) {
	app_log(0, "graphics.c", 344, "Switching to scene %d", scene);
    
    // First, clean up old scene.
    graphics_destroy_current_scene();

    // Then, change to new scene.
    switch (scene) {
        case PET_SCENE:
            graphics_generate_pet_scene(window, p);
            break;
        case CLOCK_SCENE:
            graphics_generate_clock_scene(window);
            break;
    }

    currentScene = scene;
    app_log(0, "graphics.c", 360, "Current scene after assignment: %d", currentScene);
}