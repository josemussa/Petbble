#include "menu.h"
#include "pebble.h"
#include "pet.h"
#include "graphics.h"

#define ITEM_MENU_DEFAULT 0
#define ONE_SECOND 1000
#define TIMER_STEP_MS (ONE_SECOND * 1)  // TODO: eventually change to 1 minute

static int item_menu = ITEM_MENU_DEFAULT;
static Pet p;
static AppTimer *pet_status_timer;
static Window *window;

static void increment_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (currentScene == PET_SCENE) {
        // Wrap-around menu
        if (item_menu >= 7) {
            item_menu=0;
        } else {
            item_menu++;
        }
        
        update_menu(item_menu);
    }
}
static void decrement_click_handler(ClickRecognizerRef recognizer, void *context) {
    if (currentScene == PET_SCENE) {
        if (item_menu <= 0) {
            item_menu = 7;
        } else {
            item_menu--;
        }
        
        update_menu(item_menu);
    }
}

static void toggleLights() {
    if (currentScene == PET_SCENE) {
        switchScene(CLOCK_SCENE, window);
    } else if (currentScene == CLOCK_SCENE) {
        switchScene(PET_SCENE, window);
        update_menu();
    }
}

static void select_menu() {
    switch (item_menu) {
        case 0:
            //PIZZA
            pet_feed(&p);
            break;
        case 1:
            //BULB
            toggleLights();
            break;
        case 2:
            //PARK
            // TODO: accelerometer minigame
            pet_play(&p);
            break;
        case 3:
            //PILL
            pet_heal(&p);
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
    }
}

static void click_config_provider(void *context) {
    const uint16_t repeat_interval_ms = 200;
    window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) decrement_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) increment_click_handler);
    window_single_click_subscribe(BUTTON_ID_SELECT, select_menu);
    
    window_long_click_subscribe(BUTTON_ID_SELECT, 4000, select_long_click_handler, NULL);
}

static void update_pet_timer_callback(void *data) {
    pet_check_status(&p);
    pet_status_timer = app_timer_register(TIMER_STEP_MS, update_pet_timer_callback, NULL);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction) {
    app_log(0, "menu.c", 112, "Current scene: %d", currentScene);
    if (currentScene == CLOCK_SCENE) {
        //toggleLights();
    }
}

static void window_load(Window *me){
    currentScene = PET_SCENE;
    window = me;
    if (!pet_load_state(&p)) {
        pet_new(&p);
    }
    graphics_generate_pet_scene(me, &p);
    // If app was on standby, call check_status appropriate number of times as if it were open.
    for (int i = 0; i < (p.fields[LAST_OPEN_KEY] - time(NULL)) / TIMER_STEP_MS; i++) {
        pet_check_status(&p);
    }
    pet_status_timer = app_timer_register(TIMER_STEP_MS, update_pet_timer_callback, NULL);
    accel_tap_service_subscribe(&accel_tap_handler);
    update_menu();
}

static void window_unload(Window *window) {
    pet_save_state(&p);
    graphics_destroy_current_scene();
    app_timer_cancel(pet_status_timer);
    accel_tap_service_unsubscribe();
}