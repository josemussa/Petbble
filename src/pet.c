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

#define CURRENT_STAGE_KEY 0
#define HAPPINESS_KEY 1
#define HUNGER_KEY 2
#define DISCIPLINE_KEY 3
#define TOTAL_AGE_KEY 4
#define CURRENT_STAGE_AGE_KEY 5

void pet_load_state(Pet *p) {
	p->current_stage = 0;
    if (persist_exists(CURRENT_STAGE_KEY)) {
        p->current_stage = persist_read_int(CURRENT_STAGE_KEY);
    }
    // TODO
    // variables defaulted to -1 are features not yet implemented.
    p->happiness_level = -1;
    if (persist_exists(HAPPINESS_KEY)) {
        p->happiness_level = persist_read_int(HAPPINESS_KEY);
    }
    p->hunger_level = -1;
    if (persist_exists(HUNGER_KEY)) {
        p->hunger_level = persist_read_int(HUNGER_KEY);
    }
    p->discipline_level = -1;
    if (persist_exists(DISCIPLINE_KEY)) {
        p->discipline_level = persist_read_int(DISCIPLINE_KEY);
    }
    p->total_age = time(NULL);
    if (persist_exists(TOTAL_AGE_KEY)) {
        p->total_age = persist_read_int(TOTAL_AGE_KEY);
    }
    p->current_stage_age = -1;
    if (persist_exists(CURRENT_STAGE_AGE_KEY)) {
        p->current_stage_age = persist_read_int(CURRENT_STAGE_AGE_KEY);
    }
}

void pet_save_state(Pet *p) {
    persist_write_int(CURRENT_STAGE_KEY, p->current_stage);
    persist_write_int(HAPPINESS_KEY, p->happiness_level);
    persist_write_int(HUNGER_KEY, p->hunger_level);
    persist_write_int(DISCIPLINE_KEY, p->discipline_level);
    persist_write_int(TOTAL_AGE_KEY, p->total_age);
    persist_write_int(CURRENT_STAGE_AGE_KEY, p->current_stage_age);
}

void pet_new(Pet *p) {
    p->current_stage = 0;
    p->happiness_level = -1;
    p->hunger_level = -1;
    p->discipline_level = -1;
    p->total_age = time(NULL);
    p->current_stage_age = -1;
    persist_delete(CURRENT_STAGE_KEY);
    persist_delete(HAPPINESS_KEY);
    persist_delete(HUNGER_KEY);
    persist_delete(DISCIPLINE_KEY);
    persist_delete(TOTAL_AGE_KEY);
    persist_delete(CURRENT_STAGE_AGE_KEY);
}

void pet_feed(Pet *p) {
	if (p->current_stage < TODDLER_STAGE) {
		p->current_stage = p->current_stage + 1;
	}
}