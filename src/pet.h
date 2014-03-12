typedef struct {
	int current_stage;
	int happiness_level;
	int hunger_level;
	int discipline_level;
	// Date in UNIX seconds of creation of pet
	long total_age;
	// Date in UNIX seconds in which pet evolved into current stage
	long current_stage_age;
} Pet;

void pet_load_state(Pet *p);
void pet_save_state(Pet *p);
void pet_new(Pet *p);
void pet_feed(Pet *p);