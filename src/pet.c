#include "pet.h"
#include "pebble.h"

#define EGG_STAGE_0 0
#define EGG_STAGE_1 1
#define EGG_STAGE_2 2
#define BABITCHI_STAGE 3
#define MARUTCHI_STAGE 4

#define MAX_STAT 3

#define ONE_SECOND  1000
#define ONE_MINUTE  (ONE_SECOND * 60)
#define ONE_HOUR    (ONE_MINUTE * 60)

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
    p->fields[LAST_OPEN_KEY] = time(NULL);
    for (int i = 0; i < NUM_PET_FIELDS; i++) {
        persist_write_int(i, p->fields[i]);
    }
}

// Returns 1 if key is a stat from 0 - 3, and 0 otherwise.
static int counting_stat(int i) {
    switch (i) {
        case HEALTH_KEY:
        case HAPPINESS_KEY:
        case HUNGER_KEY:
        case DISCIPLINE_KEY:
        case ENERGY_KEY:
            return 1;
            break;
        default:
            return 0;
  }
}

// Returns a rand number between low and high, inclusive on the low, exclusive on high.
static int randRange(int low, int high) {
    if (high <= low) {
        return 0;
    }
    return (rand() % (high - low) + low);
}

void pet_new(Pet *p) {
    p->fields[CURRENT_STAGE_KEY] = 0;
    p->fields[HEALTH_KEY] = 3;
    p->fields[HAPPINESS_KEY] = 3;
    p->fields[HUNGER_KEY] = 3;
    p->fields[DISCIPLINE_KEY] = 3;
    p->fields[WEIGHT_KEY] = randRange(5, 10);
    p->fields[ENERGY_KEY] = 3;
    p->fields[TOTAL_AGE_KEY] = time(NULL);
    p->fields[CURRENT_STAGE_AGE_KEY] = time(NULL);
    p->fields[LAST_OPEN_KEY] = time(NULL);
    for (int i = 0; i < NUM_PET_FIELDS; i++) {
        persist_delete(i);
    }
}

void pet_feed(Pet *p) {
    p->fields[WEIGHT_KEY] = p->fields[WEIGHT_KEY] + 1;
    if (p->fields[HUNGER_KEY] < MAX_STAT) {
        p->fields[HUNGER_KEY] = p->fields[HUNGER_KEY] + 1;
    } else {
        // If already full, decrease health by 1.
        p->fields[HEALTH_KEY] = p->fields[HEALTH_KEY] - 1;
    }
}

static void update_stats(Pet *p, int i) {
    // Approximately every i mintes, stats decrease by 1.
    if (rand() % i == 0) {
        p->fields[HEALTH_KEY] = p->fields[HEALTH_KEY] - 1;
    }
    if (rand() % i == 0) {
        p->fields[HAPPINESS_KEY] = p->fields[HAPPINESS_KEY] - 1;
    }
    if (rand() % i == 0) {
        p->fields[HUNGER_KEY] = p->fields[HUNGER_KEY] - 1;
    }
    if (rand() % i == 0) {
        p->fields[DISCIPLINE_KEY] = p->fields[DISCIPLINE_KEY] - 1;
    }
    if (rand() % i == 0) {
        p->fields[WEIGHT_KEY] = p->fields[WEIGHT_KEY] - 1;
    }
    if (rand() % i == 0) {
        p->fields[ENERGY_KEY] = p->fields[ENERGY_KEY] - 1;
    }
}

// Periodically called to see if conditions are met for various statuses
// @return 1 if any changes were made; 0 otherwise.
int pet_check_status(Pet *p) {
    int modify = 0;
    switch (p->fields[CURRENT_STAGE_KEY]) {
        case EGG_STAGE_0:
            p->fields[CURRENT_STAGE_KEY] = EGG_STAGE_1;
            p->fields[CURRENT_STAGE_AGE_KEY] = time(NULL);
            modify = 1;
            break;
        case EGG_STAGE_1:
            p->fields[CURRENT_STAGE_KEY] = EGG_STAGE_2;
            p->fields[CURRENT_STAGE_AGE_KEY] = time(NULL);
            modify = 1;
            break;
        case EGG_STAGE_2:
            p->fields[CURRENT_STAGE_KEY] = BABITCHI_STAGE;
            p->fields[HEALTH_KEY] = 3;
            p->fields[HAPPINESS_KEY] = 3;
            p->fields[HUNGER_KEY] = 3;
            p->fields[DISCIPLINE_KEY] = 1;
            // WEIGHT purposefully not touched.
            p->fields[CURRENT_STAGE_AGE_KEY] = time(NULL);
            modify = 1;
            break;
        case BABITCHI_STAGE:
            update_stats(p, 10);
            for (int i = 0; i < NUM_PET_FIELDS; i++) {
                // If one of the stats required to evolve is not maximum, then return.
                if ((time(NULL) - p->fields[CURRENT_STAGE_AGE_KEY] < ONE_HOUR * 24) || 
                   (counting_stat(i) && (p->fields[i] < MAX_STAT))) {
                    break;
                }
            }
            // If all stats are max, then we can evolve.
            p->fields[CURRENT_STAGE_KEY] = MARUTCHI_STAGE;
            p->fields[HEALTH_KEY] = 3;
            p->fields[HAPPINESS_KEY] = 2;
            p->fields[HUNGER_KEY] = 1;
            p->fields[DISCIPLINE_KEY] = 1;
            p->fields[WEIGHT_KEY] += 5;
            p->fields[CURRENT_STAGE_AGE_KEY] = time(NULL);
            modify = 1;
            break;
    }
    return modify;
}


