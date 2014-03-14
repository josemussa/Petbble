#ifndef PET_H
#define PET_H

#define NUM_PET_FIELDS 10

// These serve double duty as keys for persistent storage, and indexes into the Pet struct array.
#define CURRENT_STAGE_KEY 0
#define HEALTH_KEY 1
#define HAPPINESS_KEY 2
#define HUNGER_KEY 3
#define DISCIPLINE_KEY 4
#define WEIGHT_KEY 5
#define ENERGY_KEY 6
#define TOTAL_AGE_KEY 7
#define CURRENT_STAGE_AGE_KEY 8
#define LAST_OPEN_KEY 9

typedef struct {
    int fields[NUM_PET_FIELDS];
} Pet;

#endif

int pet_load_state(Pet *p);
void pet_save_state(Pet *p);
void pet_new(Pet *p);
void pet_feed(Pet *p);
int pet_check_status(Pet *p);