#include "pebble.h"

#include "menu.c"

static Window *window;

static void init(void){
    //srand(time(NULL));  //TODO: uncomment this later.  there is a bug with the Pebble API which makes srand leak memory, 
                          //but it is very small so it isn't that important.  Easier debugging for now though with this commented out.
    
    window = window_create();
    window_set_click_config_provider(window, click_config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
    
    window_stack_push(window, true /* Animated */);
}



static void deinit(void) {

    window_destroy(window);
    
}


int main(void) {
    init();
    app_event_loop();
    deinit();
    
    return 0;
}
