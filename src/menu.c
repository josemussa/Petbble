#include "menu.h"
#include "pebble.h"
#include "pet.h"
#include "graphics/graphics.h"
#include "graphics/petscene.h"
#include "graphics/clockscene.h"
#include "graphics/gamescene.h"
#include "graphics/statsscene.h"
#include "graphics/statsscene2.h"

#define ITEM_MENU_DEFAULT 0
#define ONE_SECOND 1000
#define TIMER_STEP_MS (ONE_SECOND * 1)  // TODO: eventually change to 1 minute

#define PET_SCENE 0
#define CLOCK_SCENE 1
#define GAME_SCENE 2
#define STATS_SCENE_0 3
#define STATS_SCENE_1 4


static int item_menu = ITEM_MENU_DEFAULT;
static Pet p;
static AppTimer *pet_status_timer;
static AppTimer *enable_menu_timer;
static Window *window;
static int currentScene;
static int select_enabled = 1;

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
        pet_sleep(&p);
        graphics_destroy_pet_scene();
        graphics_generate_clock_scene(window);
        currentScene = CLOCK_SCENE;
    } else if (currentScene == CLOCK_SCENE) {
        pet_wake_up(&p);
        graphics_destroy_clock_scene();
        graphics_generate_pet_scene(window, &p);
        update_menu(item_menu);
        currentScene = PET_SCENE;
    }
}

static void toggle_game() {
    if (currentScene == PET_SCENE) {
        if (pet_attempt_play(&p)) {
            graphics_destroy_pet_scene();
            graphics_generate_game_scene(window);
            currentScene = GAME_SCENE;
        }
    } else if (currentScene == GAME_SCENE) {
        int score = graphics_destroy_game_scene();
        graphics_generate_pet_scene(window, &p);
        update_menu(item_menu);
        currentScene = PET_SCENE;
        pet_play(&p, score);
    }
}

static void toggle_stats() {
    if (currentScene == PET_SCENE) {
        graphics_destroy_pet_scene();
        graphics_generate_stats_scene(window, &p);
        currentScene = STATS_SCENE_0;
    } else if (currentScene == STATS_SCENE_0) {
        graphics_destroy_stats_scene();
        graphics_generate_stats_scene2(window, &p);
        currentScene = STATS_SCENE_1;
    } else if (currentScene == STATS_SCENE_1) {
        graphics_destroy_stats_scene2();
        graphics_generate_pet_scene(window, &p);
        update_menu(item_menu);
        currentScene = PET_SCENE;
    }
}

static void enable_menu_callback() {
    app_log(0, "menu.c", 105, "Select_enabled changing back to true.");
    select_enabled = 1;
}

static void select_menu() {
    app_log(0, "menu.c", 105, "Called select_menu()");
    if (select_enabled == 1) {
        app_log(0, "menu.c", 105, "Select_enabled changing to false.");
        select_enabled = 0;
        enable_menu_timer = app_timer_register(ONE_SECOND, enable_menu_callback, NULL);
        switch (item_menu) {
            case 0:
                //PIZZA
                pet_feed(&p);
                generate_actions(window,0);
                break;
            case 1:
                //BULB
                // sleep / wakeup function called in toggle_lights
                toggle_lights();
                break;
            case 2:
                //PARK
                toggle_game();
                break;
            case 3:
                //PILL
                pet_heal(&p);
                generate_actions(window,1);
                break;
            case 4:
                //BATH
                generate_actions(window,3);
                pet_bath(&p);
                break;
            case 5:
                //HEALTH
                toggle_stats();

                break;
            case 6:
                //DISCIPLINE
                generate_actions(window,2);
                pet_discipline(&p);
                break;
            case 7:
                //CALL
                generate_static_actions(window); //im just using this button for an example
                break;
            default:
                app_log(0, "menu.c", 103, "Switch case in select_menu() does not exist.");
                break;

        }
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
    // currentScene = STATS_SCENE_1;
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
    // graphics_generate_stats_scene(window, &p, 1);
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
        case STATS_SCENE_0:
            graphics_destroy_stats_scene();
            break;
        case STATS_SCENE_1:
            graphics_destroy_stats_scene2();
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