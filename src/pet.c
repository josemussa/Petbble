#include "pet.h"
#include "pebble.h"

#define EGG_STAGE_0 0
#define EGG_STAGE_1 1
#define EGG_STAGE_2 2
#define BABY_STAGE 3
#define TODDLER_STAGE 4
#define TEENAGER_STAGE_HEALTHY 5
#define TEENAGER_STAGE_UNHEALTHY 6
#define ADULT_STAGE_HEALTHY 7
#define ADULT_STAGE_AVERAGE 8
#define ADULT_STAGE_UNHEALTHY 9

#define MAX_STAT 5



int pet_load_state(Pet *p) {
    for (int i = 0; i < NUM_PET_FIELDS; i++) {
        if (persist_exists(i)) {
            p->fields[i] = persist_read_int(i);
        } else {
            return 0;
        }
    }
    return 1;
}

void pet_save_state(Pet *p) {
    for (int i = 0; i < NUM_PET_FIELDS; i++) {
        persist_write_int(i, p->fields[i]);
    }
}

void pet_new(Pet *p) {
    p->fields[CURRENT_STAGE_KEY] = 0;
    p->fields[HAPPINESS_KEY] = -1;
    p->fields[HUNGER_KEY] = -1;
    p->fields[DISCIPLINE_KEY] = -1;
    p->fields[TOTAL_AGE_KEY] = time(NULL);
    p->fields[CURRENT_STAGE_AGE_KEY] = time(NULL);
    for (int i = 0; i < NUM_PET_FIELDS; i++) {
        persist_delete(i);
    }
}

void pet_feed(Pet *p) {
    if (p->fields[HUNGER_KEY] < MAX_STAT) {
        p->fields[HUNGER_KEY] = p->fields[HUNGER_KEY] + 1;
    }
}



// Periodically called to see if conditions are met for various statuses
// @return 1 if any changes were made; 0 otherwise.
int pet_check_status(Pet *p) {
    int modify = 0;
    // Hunger will decrease once per 5 timer ticks.  Should probably come up with a better solution without magic numbers later.
    if (rand() % 5 == 0) {
        p->fields[HUNGER_KEY] = p->fields[HUNGER_KEY] - 1;
        modify = 1;
    }

    if (p->fields[CURRENT_STAGE_KEY] < TODDLER_STAGE && p->fields[HUNGER_KEY] == MAX_STAT) {
        p->fields[CURRENT_STAGE_KEY] = p->fields[CURRENT_STAGE_KEY] + 1;
        modify = 1;
    }
    return modify;
}


