#include "pet.h"
#include "pebble.h"

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

void pet_die(Pet *p) {
    // TODO: unimplemented
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

// Attempts to increase stat by 1.  If it is successful, returns 1.  Otherwise returns 0.
static int increment_stat(Pet *p, int stat) {
    if (stat < MAX_STAT) {
        p->fields[stat] += 1;
        return 1;
    }
    return 0;
}

// Attempts to decrease stat by 1.  If it is successful, returns 1.  Otherwise returns 0.
static int decrement_stat(Pet *p, int stat) {
    if (stat > 0) {
        p->fields[stat] -= 1;
        return 1;
    }
    return 0;
}

void pet_new(Pet *p) {
    p->fields[CURRENT_STAGE_KEY] = 0;
    p->fields[HEALTH_KEY] = 0;          // TODO change to 3
    p->fields[HAPPINESS_KEY] = 1;       // TODO change to 3
    p->fields[HUNGER_KEY] = 2;          // TODO change to 3
    p->fields[DISCIPLINE_KEY] = 3;      // TODO change to 3
    p->fields[WEIGHT_KEY] = randRange(5, 10);
    p->fields[ENERGY_KEY] = 3;
    p->fields[SICK_KEY] = 0;
    p->fields[TOTAL_AGE_KEY] = time(NULL);
    p->fields[CURRENT_STAGE_AGE_KEY] = time(NULL);
    p->fields[LAST_OPEN_KEY] = time(NULL);
    for (int i = 0; i < NUM_PET_FIELDS; i++) {
        persist_delete(i);
    }
}

void pet_feed(Pet *p) {
    p->fields[WEIGHT_KEY] = p->fields[WEIGHT_KEY] + 1;
    if (increment_stat(p, HUNGER_KEY)) {
        return;
    } else if (decrement_stat(p, HEALTH_KEY)) {
        return;
    } else {
        // Dies from over feeding? (0 health, full belly.)
        pet_die(p);
    }
}

void pet_play(Pet *p) {
    // Can only play if positive energy, and not sick.
    if (p->fields[ENERGY_KEY] > 0 && !p->fields[SICK_KEY]) {
        decrement_stat(p, ENERGY_KEY);
        decrement_stat(p, WEIGHT_KEY);
        increment_stat(p, HAPPINESS_KEY);
        increment_stat(p, HEALTH_KEY);
    } else if (decrement_stat(p, HEALTH_KEY)) {
        return;
    } else if (!p->fields[SICK_KEY]) {
        p->fields[SICK_KEY] = 1;
    } else {
        // If you attempt to play with 0 energy, 0 health, and sick, you die :(
        pet_die(p);
    }
}

void pet_heal(Pet *p) {
    if (p->fields[SICK_KEY]) {
        p->fields[SICK_KEY] = 0;
    } else {
        decrement_stat(p, DISCIPLINE_KEY);
    }
}

static void update_stats(Pet *p, int i) {
    // Approximately every i mintes, stats decrease by 1.
    if (rand() % i == 0) {
        decrement_stat(p, HEALTH_KEY);
    }
    if (rand() % i == 0) {
        decrement_stat(p, HAPPINESS_KEY);
    }
    if (rand() % i == 0) {
        decrement_stat(p, HUNGER_KEY);
    }
    if (rand() % i == 0) {
        decrement_stat(p, DISCIPLINE_KEY);
    }
    if (rand() % i == 0) {
        decrement_stat(p, WEIGHT_KEY);
    }
    if (rand() % i == 0) {
        decrement_stat(p, ENERGY_KEY);
    }
    // Hungry and unhealthy leads to greater chance of getting sick.
    if (rand() % (5 * i * (p->fields[HEALTH_KEY] + p->fields[HUNGER_KEY]))) {
        p->fields[SICK_KEY] = 1;
    }
}

// Periodically called to see if conditions are met for various statuses
// @return 1 if any changes were made; 0 otherwise.
int pet_check_status(Pet *p) {
    int modify = 0;
    switch (p->fields[CURRENT_STAGE_KEY]) {
        case EGG_STAGE:
            if (time(NULL) - p->fields[CURRENT_STAGE_AGE_KEY] < ONE_MINUTE) {
                break;
            }
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
                   (counting_stat(i) && (p->fields[i] < MAX_STAT)) ||
                    p->fields[SICK_KEY]) {
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


