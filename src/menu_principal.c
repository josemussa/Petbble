#include "pebble.h"


#define ITEM_MENU_DEFAULT 0

static int item_menu = ITEM_MENU_DEFAULT;

static Window *window;

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



static BitmapLayer *egg_layer;

static GBitmap *egg1;

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



static void update_text() {
    
    
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

static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (item_menu >= 7) {
        item_menu=0;
    }else{
        item_menu++;
    }
    
    update_text();
}
static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (item_menu <= 0) {
        item_menu = 7;
    }else{
        item_menu--;
    }
    
    update_text();
}

static void select_menu(){}

static const VibePattern custom_pattern = {
    .durations = (uint32_t []) {100},
    .num_segments = 1
};

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    vibes_enqueue_custom_pattern(custom_pattern);
}


static void click_config_provider(void *context) {
    const uint16_t repeat_interval_ms = 200;
    window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) decrement_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) increment_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_menu);
    
    window_long_click_subscribe(BUTTON_ID_SELECT, 4000, select_long_click_handler, NULL);
}

static void generoMenu(){
    
    pizza = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PIZZA);
    bulb = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BULB);
    park = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PARK);
    pill = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PILL);
    bath = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATH);
    health = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEALTH);
    discipline = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DISCIPLINE);
    call = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CALL);
    
    egg1 = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGG1);

    

    
}

static void generoIconos(){
    
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);

    // This needs to be deinited on app exit which is when the event loop ends
    generoMenu();
    
    // PIZZA
    pizza_layer = bitmap_layer_create(GRect(8,2,20, 20));
    bitmap_layer_set_bitmap(pizza_layer, pizza);
    layer_add_child(window_layer, bitmap_layer_get_layer(pizza_layer));
    
    // BULB
    bulb_layer = bitmap_layer_create(GRect(44,2,20,20));
    bitmap_layer_set_bitmap(bulb_layer, bulb);
    layer_add_child(window_layer, bitmap_layer_get_layer(bulb_layer));
    
    // PARK
    park_layer = bitmap_layer_create(GRect(80,2,20,20));
    bitmap_layer_set_bitmap(park_layer, park);
    layer_add_child(window_layer, bitmap_layer_get_layer(park_layer));
    
    // PILL
    pill_layer = bitmap_layer_create(GRect(116,2,20,20));
    bitmap_layer_set_bitmap(pill_layer, pill);
    layer_add_child(window_layer, bitmap_layer_get_layer(pill_layer));
    
    // BATH
    bath_layer = bitmap_layer_create(GRect(8,130,20, 20));
    bitmap_layer_set_bitmap(bath_layer, bath);
    layer_add_child(window_layer, bitmap_layer_get_layer(bath_layer));
    
    // HEALTH
    health_layer = bitmap_layer_create(GRect(44,130,20, 20));
    bitmap_layer_set_bitmap(health_layer, health);
    layer_add_child(window_layer, bitmap_layer_get_layer(health_layer));
    
    // DISCIPLINE
    discipline_layer = bitmap_layer_create(GRect(80,130,20, 20));
    bitmap_layer_set_bitmap(discipline_layer, discipline);
    layer_add_child(window_layer, bitmap_layer_get_layer(discipline_layer));
    
    // CALL
    call_layer = bitmap_layer_create(GRect(116,130,20, 20));
    bitmap_layer_set_bitmap(call_layer, call);
    layer_add_child(window_layer, bitmap_layer_get_layer(call_layer));
    
    // CREO HUEVO
    egg_layer = bitmap_layer_create(bounds);
    bitmap_layer_set_bitmap(egg_layer, egg1);
    bitmap_layer_set_alignment(egg_layer, GAlignCenter);
    layer_add_child(window_layer, bitmap_layer_get_layer(egg_layer));
    
    update_text();
    select_menu();
    

}

static void window_load(Window *me){
    
    generoIconos();
    
    
}

static void destruyoIconos(){

    
    gbitmap_destroy(bulb);
    gbitmap_destroy(pizza);
    gbitmap_destroy(park);
    gbitmap_destroy(pill);
    gbitmap_destroy(bath);
    gbitmap_destroy(health);
    gbitmap_destroy(discipline);
    gbitmap_destroy(call);
    bitmap_layer_destroy(pizza_layer);
    bitmap_layer_destroy(bulb_layer);
    bitmap_layer_destroy(park_layer);
    bitmap_layer_destroy(pill_layer);
    bitmap_layer_destroy(bath_layer);
    bitmap_layer_destroy(health_layer);
    bitmap_layer_destroy(discipline_layer);
    bitmap_layer_destroy(call_layer);
    
    gbitmap_destroy(egg1);
    bitmap_layer_destroy(egg_layer);

}


static void window_unload(Window *window) {
    
    destruyoIconos();

}



