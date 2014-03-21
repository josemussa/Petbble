#ifndef PET_H
#define PET_H

#define NUM_PET_FIELDS 13

#define NUM_PET_STAGES 5
#define EGG_STAGE 0
#define BABITCHI_STAGE 1
#define MARUTCHI_STAGE 2
#define KUCHIT_STAGE 3
#define RETURN_STAGE 4

// These serve double duty as keys for persistent storage, and indexes into the Pet struct array.
#define CURRENT_STAGE_KEY 0
#define HEALTH_KEY 1
#define HAPPINESS_KEY 2
#define HUNGER_KEY 3
#define DISCIPLINE_KEY 4
#define WEIGHT_KEY 5
#define ENERGY_KEY 6
#define SICK_KEY 7
#define TOTAL_AGE_KEY 8
#define CURRENT_STAGE_AGE_KEY 9
#define LAST_OPEN_KEY 10
#define SLEEPING_KEY 11
#define POOP_KEY 12

typedef struct {
    int fields[NUM_PET_FIELDS];
} Pet;

#endif

int pet_load_state(Pet *p);
void pet_save_state(Pet *p);
void pet_new(Pet *p);
void pet_feed(Pet *p);
int pet_attempt_play(Pet *p);
void pet_play(Pet *p, int score);
void pet_heal(Pet *p);
void pet_sleep(Pet *p);
void pet_wake_up(Pet *p);
void pet_discipline(Pet *p);
void pet_bath(Pet *p);
int pet_check_status(Pet *p);
int pet_calculate_age(Pet *p);