#include "pebble.h"
#include "petscene.h"
#include "graphics.h"
#include "../pet.h"


#define ONE_SECOND 1000
#define ANIMATION_STEP_MS (ONE_SECOND * 1)
#define NUM_ACTIONS 5

#define EAT 0
#define POOP 1

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

static BitmapLayer *pet_layer;
static BitmapLayer *actions_layer;
static BitmapLayer *static_actions_layer;
#define MAX_ANIMATION_FRAMES 10
static GBitmap *pet_sprites[NUM_PET_STAGES][MAX_ANIMATION_FRAMES + 1];
static GBitmap *actions_sprites[NUM_ACTIONS][MAX_ANIMATION_FRAMES + 1];
static int animationCounter = 0;
static int actionsCounter = 0;
static int static_actionsCounter = 0;

static AppTimer *pet_animation_timer;
static AppTimer *actions_animation_timer;
static AppTimer *static_actions_animation_timer;
static Pet *p;

static void animate_pet_timer_callback(void *data);
static void animate_actions_timer_callback(void *data);
static void animate_static_actions_timer_callback(void *data);


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

void generate_actions_scene(){
    actionsCounter = 0;
    switch(0){
        case 0:
            actions_sprites[EAT][0] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EAT1);
            actions_sprites[EAT][1] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EAT2);
            actions_sprites[EAT][2] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EAT3);
            actions_sprites[EAT][3] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EAT4);
            actions_sprites[EAT][4] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_EAT5);
            actions_sprites[EAT][5] = NULL;
            actions_layer = bitmap_layer_create_safe(GRect(10,45,31, 31)); 
            break;
        default:
            break;
    }

    app_timer_register(ONE_SECOND, animate_actions_timer_callback, NULL);
    
}

void stop_actions_scene(){
    actionsCounter = 0;
    app_timer_cancel(actions_animation_timer);
    bitmap_layer_destroy_safe(actions_layer);
    
}

static void animate_actions_timer_callback(void *data) {

    
    if (actions_sprites[EAT][actionsCounter] == NULL) {
        stop_actions_scene();
    }else{
        actions_animation_timer = app_timer_register(ONE_SECOND, animate_actions_timer_callback, NULL);
    }

    bitmap_layer_set_bitmap(actions_layer, actions_sprites[EAT][actionsCounter]);
    actionsCounter += 1;
    

}

void generate_actions(Window *window){

    Layer *window_layer = window_get_root_layer(window);
    generate_actions_scene();
    layer_add_child(window_layer, bitmap_layer_get_layer(actions_layer));
    layer_insert_above_sibling(bitmap_layer_get_layer(actions_layer), bitmap_layer_get_layer(pet_layer)) ;

}

void generate_static_actions_scene(){
    switch(0){
        case 0:
            actions_sprites[POOP][0] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_POOP1);
            actions_sprites[POOP][1] = gbitmap_create_with_resource_safe(RESOURCE_ID_IMAGE_POOP2);
            actions_sprites[POOP][2] = NULL;
            static_actions_layer = bitmap_layer_create_safe(GRect(100,60,31, 35)); 
            break;
        default:
            break;
    }

    app_timer_register(ONE_SECOND, animate_static_actions_timer_callback, NULL);
    
}

static void animate_static_actions_timer_callback(void *data) {

    static_actionsCounter += 1;
    if (actions_sprites[POOP][static_actionsCounter] == NULL) {
        static_actionsCounter = 0;
    }
    bitmap_layer_set_bitmap(static_actions_layer, actions_sprites[POOP][static_actionsCounter]);
    static_actions_animation_timer = app_timer_register(ONE_SECOND, animate_static_actions_timer_callback, NULL);
    

}

void generate_static_actions(Window *window){

    Layer *window_layer = window_get_root_layer(window);
    generate_static_actions_scene();
    layer_add_child(window_layer, bitmap_layer_get_layer(static_actions_layer));
    layer_insert_above_sibling(bitmap_layer_get_layer(static_actions_layer), bitmap_layer_get_layer(pet_layer)) ;

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
    app_timer_register(800, animate_pet_timer_callback, NULL);

}

void graphics_destroy_pet_scene() {
	if (p != NULL) {
		deallocate_pet_sprite(p->fields[CURRENT_STAGE_KEY]);
	}
	bitmap_layer_destroy_safe(pet_layer);
    bitmap_layer_destroy_safe(actions_layer);
    bitmap_layer_destroy_safe(static_actions_layer);
	destroy_icons();
	app_timer_cancel(pet_animation_timer);
    app_timer_cancel(actions_animation_timer);
    app_timer_cancel(static_actions_animation_timer);
}

static void animate_pet_timer_callback(void *data) {
    animationCounter += 1;
	int level = p->fields[CURRENT_STAGE_KEY];
	if (pet_sprites[level][animationCounter] == NULL) {
		animationCounter = 0;
	}
	bitmap_layer_set_bitmap(pet_layer, pet_sprites[level][animationCounter]);
    pet_animation_timer = app_timer_register(800, animate_pet_timer_callback, NULL);
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