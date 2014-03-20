#include "menu.h"
#include "pebble.h"
#include "pet.h"
#include "graphics/graphics.h"
#include "graphics/petscene.h"
#include "graphics/clockscene.h"
#include "graphics/gamescene.h"
#include "graphics/statsscene.h"

#define ITEM_MENU_DEFAULT 0
#define ONE_SECOND 1000
#define TIMER_STEP_MS (ONE_SECOND * 1)  // TODO: eventually change to 1 minute

#define PET_SCENE 0
#define CLOCK_SCENE 1
#define GAME_SCENE 2
#define STATS_SCENE_HEALTH 3
#define STATS_SCENE_HAPPINESS 3
#define STATS_SCENE_HUNGER 3
#define STATS_SCENE_DISCIPLINE 3

static int item_menu = ITEM_MENU_DEFAULT;
static Pet p;
static AppTimer *pet_status_timer;
static Window *window;
static int currentScene;

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

static void toggle_lights() {
    if (currentScene == PET_SCENE) {
        graphics_generate_clock_scene(window);
        currentScene = CLOCK_SCENE;

        graphics_destroy_pet_scene();
    } else if (currentScene == CLOCK_SCENE) {
        graphics_generate_pet_scene(window, &p);
        update_menu(item_menu);
        currentScene = PET_SCENE;

        graphics_destroy_clock_scene();
    }
}

static void toggle_game() {
    if (currentScene == PET_SCENE) {
        graphics_generate_game_scene(window);
        currentScene = GAME_SCENE;

        graphics_destroy_pet_scene();
    } else if (currentScene == GAME_SCENE) {
        graphics_generate_pet_scene(window, &p);
        update_menu(item_menu);
        currentScene = PET_SCENE;

        graphics_destroy_game_scene();
    }
}

static void toggle_stats() {
    if (currentScene == PET_SCENE) {
        //graphics_generate_stats_scene(window, &p, HEALTH_KEY, "Health");
        //currentScene = STATS_SCENE_HEALTH;
        graphics_generate_stats_scene(window, &p);
        currentScene = STATS_SCENE_HEALTH;
        graphics_destroy_pet_scene();
    } else if (currentScene == STATS_SCENE_HEALTH) {
        graphics_generate_pet_scene(window, &p);
        update_menu(item_menu);
        currentScene =  PET_SCENE;

        graphics_destroy_stats_scene();
    }
}

static void select_menu() {
    switch (item_menu) {
        case 0:
            //PIZZA
            pet_feed(&p);
            generate_actions(window);
            break;
        case 1:
            //BULB
            toggle_lights();
            break;
        case 2:
            //PARK
            toggle_game();
            //pet_play(&p);
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
            toggle_stats();
            break;
        case 6:
            //DISCIPLINE
            break;
        case 7:
            //CALL
            break;
        default:
            app_log(0, "menu.c", 103, "Switch case in select_menu() does not exist.");
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
    if (currentScene == CLOCK_SCENE) {
        toggle_lights();
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
    update_menu(item_menu);
}

static void destroy_current_scene() {
    switch (currentScene) {
        case PET_SCENE:
            graphics_destroy_pet_scene();
            break;
        case CLOCK_SCENE:
            graphics_destroy_clock_scene();
            break;
        case GAME_SCENE:
            graphics_destroy_game_scene();
            break;
        default:
            app_log(0, "menu.c", 170, "Switch case in destroy_current_scene() does not exist.");
            break;
    }
}

static void window_unload(Window *window) {
    pet_save_state(&p);
    destroy_current_scene();
    app_timer_cancel(pet_status_timer);
    accel_tap_service_unsubscribe();
}