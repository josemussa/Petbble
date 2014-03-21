#include "pebble.h"
#include "../pet.h"

void graphics_generate_pet_scene(Window *window, Pet *pet);
void graphics_destroy_pet_scene();
void update_menu(int item);
void generate_actions(Window *window, int actions);
void generate_static_actions(Window *window);