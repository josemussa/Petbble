#include "pebble.h"
#include "pet.h"

// Change to 0 to take off debug mode.
#define DEBUG_MODE 1

#define ITEM_MENU_DEFAULT 0
#define ONE_SECOND 1000
#define TIMER_STEP_MS (ONE_SECOND * 1)  // TODO: eventually change to 1 minute

static int animationCounter = 0;

static int pet_level = 2;

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

static BitmapLayer *pet_layer;

static GBitmap *pet_sprites[16];
static GBitmap *lightoff;

static Pet p;

static AppTimer *timer;

static bool animationPetPaused = false;

TextLayer *time_layer; // The clock

static void handle_tick(struct tm *tick_time, TimeUnits units_changed);

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

static void animationPet(){
    if(!animationPetPaused){   
        bitmap_layer_set_bitmap(pet_layer, pet_sprites[animationCounter]);
    }
}

static void redraw() {
    update_text();
}

static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (!animationPetPaused) {
        // Wrap-around menu
        if (item_menu >= 7) {
            item_menu=0;
        } else {
            item_menu++;
        }
        
        update_text();
    }
}
static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (!animationPetPaused) {
        if (item_menu <= 0) {
            item_menu = 7;
        } else {
            item_menu--;
        }
        
        update_text();
    }
}

static void lightsOff(){
    animationPetPaused = true;
    bitmap_layer_set_bitmap(pet_layer, lightoff);
    
    //bitmap_layer_set_bitmap(pet_layer, pet_sprites[14]);
    
    time_layer = text_layer_create(GRect(29, 54, 144-40 /* width */, 168-54 /* height */));
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
    
    // Ensures time is displayed immediately (will break if NULL tick event accessed).
    // (This is why it's a good idea to have a separate routine to do the update itself.)
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    handle_tick(current_time, SECOND_UNIT);
    tick_timer_service_subscribe(SECOND_UNIT, &handle_tick);
    
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
}

static void lightsOn() {
    text_layer_destroy(time_layer);
    animationPetPaused = false;
    animationPet();
}

static void toggleLights() {
    if (!animationPetPaused) {
        lightsOff();
    } else {
        lightsOn();
    }
}

static void select_menu() {
    switch (item_menu) {
        case 0:
            //PIZZA
            pet_feed(&p);
            redraw();
            break;
        case 1:
            //BULB
            toggleLights();
            break;
        case 2:
            //PARK
            // TODO: accelerometer minigame
            pet_play(&p);
            redraw();
            break;
        case 3:
            //PILL
            pet_heal(&p);
            redraw();
            break;
        case 4:
            //BATH
            break;
        case 5:
            //HEALTH
            break;
        case 6:
            //DISCIPLINE
            break;
        case 7:
            //CALL
            break;
        default:
            break;

    }
}

static const VibePattern custom_pattern = {
    .durations = (uint32_t []) {100},
    .num_segments = 1
};

static void select_long_click_handler(ClickRecognizerRef recognizer, void *context) {
    vibes_enqueue_custom_pattern(custom_pattern);
    // Long-press to reset app (start new petbble from scratch.)
    if (DEBUG_MODE) {
        pet_new(&p);
        redraw();
    }
}

static void click_config_provider(void *context) {
    const uint16_t repeat_interval_ms = 200;
    window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) decrement_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) increment_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_menu);
    
    window_long_click_subscribe(BUTTON_ID_SELECT, 4000, select_long_click_handler, NULL);
}

static void generateMiscImages(){
    
    pizza = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PIZZA);
    bulb = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BULB);
    park = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PARK);
    pill = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PILL);
    bath = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BATH);
    health = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HEALTH);
    discipline = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DISCIPLINE);
    call = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CALL);
    lightoff = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LIGHTSOFF);
    
}

static void generatePet(){
    
    switch (pet_level) {
        case 0:
            // EGG
            pet_sprites[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGG1);
            pet_sprites[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGG2);
            break;
        case 2:
            pet_sprites[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI1);
            pet_sprites[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI2);
            pet_sprites[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI3);
            pet_sprites[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI4);
            pet_sprites[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI5);
            pet_sprites[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI6);
            pet_sprites[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI7);
            pet_sprites[11] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI8);
            pet_sprites[12] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI9);
            pet_sprites[13] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BABITCHI10);
            break;
        default:
            break;
    }

    
   
    pet_sprites[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGG3);
    pet_sprites[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_EGG4);
   
    pet_sprites[14] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RETURN1);
    pet_sprites[15] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RETURN2);
    
}

static void generateIcons(){
    
    Layer *window_layer = window_get_root_layer(window);
    
    // This needs to be deinited on app exit which is when the event loop ends
    generateMiscImages();
    
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
    
}

static void showPet(){
    
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_frame(window_layer);
    
    // This needs to be deinited on app exit which is when the event loop ends
    generatePet();
    
    // CREATE PET
    pet_layer = bitmap_layer_create(bounds);
    bitmap_layer_set_alignment(pet_layer, GAlignCenter);
    layer_add_child(window_layer, bitmap_layer_get_layer(pet_layer));
    
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
    
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed)
{
    ++animationCounter; 
    switch (pet_level) {
        case 0:
            // EGG
            if (animationCounter == 2){
                animationCounter = 0;
            }
            animationPet();
            break;
        case 2:
            if (animationCounter < 4){
                animationCounter = 4;
            }
            if (animationCounter == 13){
                animationCounter = 4;
            }
            
            animationPet();
            break;
        default:
            break;
    }
    
    if(animationPetPaused){
        static char time_text[] = "00:00:00"; // Needs to be static because it's used by the system later.
        strftime(time_text, sizeof(time_text), "%T", tick_time);
        text_layer_set_text(time_layer, time_text);   
    }
}

static void timer_callback(void *data) {
    if (pet_check_status(&p)) {
        redraw();
    }
    timer = app_timer_register(TIMER_STEP_MS, timer_callback, NULL);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
    if (animationPetPaused) {
        toggleLights();
    }
}

static void window_load(Window *me){
    if (!pet_load_state(&p)) {
        pet_new(&p);
    }
    generateIcons();
    showPet();
    // If app was on standby, call check_status appropriate number of times as if it were open.
    for (int i = 0; i < (p.fields[LAST_OPEN_KEY] - time(NULL)) / TIMER_STEP_MS; i++) {
        pet_check_status(&p);
    }
    redraw();
    timer = app_timer_register(TIMER_STEP_MS, timer_callback, NULL);
    accel_tap_service_subscribe(&accel_tap_handler);
}

static void destroyIcons(){
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
    for (int i = 0; i < 16; i++) {
        gbitmap_destroy(pet_sprites[i]);
    }
    gbitmap_destroy(lightoff);
    bitmap_layer_destroy(pet_layer);
    text_layer_destroy(time_layer);
    accel_tap_service_unsubscribe();
}

static void window_unload(Window *window) {
    pet_save_state(&p);
    destroyIcons();
}