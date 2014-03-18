#include "pet.h"

#define PET_SCENE 0
#define CLOCK_SCENE 1
#define GAME_SCENE 2

extern int currentScene;

GBitmap* gbitmap_create_with_resource_safe(uint32_t resource_id);
void graphics_generate_pet_scene(Window *window, Pet *p);
void graphics_destroy_current_scene();
void update_menu(int item);
void switchScene(int scene, Window *window);