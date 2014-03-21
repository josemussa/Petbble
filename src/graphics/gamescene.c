// Code in this file adapted from PebbleSDK-2.0.1/Examples/watchapps/feature_accel_discs

#include "gamescene.h"

#define MATH_PI 3.141592653589793238462
#define DISC_DENSITY 0.05
#define ACCEL_RATIO 0.05
#define ACCEL_STEP_MS 50
#define DEFAULT_RADIUS 10
#define DEFAULT_FOOD_RADIUS 5

typedef struct Vec2d {
  double x;
  double y;
} Vec2d;

typedef struct Disc {
  Vec2d pos;
  Vec2d vel;
  double mass;
  double radius;
} Disc;

static GRect window_frame;
static Layer *disc_layer;
static AppTimer *timer;
static Window *window;
static Disc disc;
static GPoint food;
static int score = 0;

static void init_disc() {
	disc.pos.x = window_frame.size.w / 2;
	disc.pos.y = window_frame.size.h / 2;
	disc.vel.x = 0;
	disc.vel.y = 0;
	disc.radius = DEFAULT_RADIUS;
	disc.mass = MATH_PI * DEFAULT_RADIUS * DEFAULT_RADIUS * DISC_DENSITY;
}

static void init_food() {
	int r1 = rand();
	int r2 = rand();
	app_log(0, "gamescene.c", 42, "Rand returns: %d followed by %d", r1, r2);
	food.x = r1 % (window_frame.size.w);
	food.y = r2 % (window_frame.size.h);
	app_log(0, "gamescene.c", 42, "Window size is %d by %d", window_frame.size.w, window_frame.size.h);
	app_log(0, "gamescene.c", 45, "Food coordinates: (%d, %d)", food.x, food.y);
}

static void disc_apply_force(Disc *disc, Vec2d force) {
	disc->vel.x += force.x / disc->mass;
	disc->vel.y += force.y / disc->mass;
}

static void disc_apply_accel(Disc *disc, AccelData accel) {
	Vec2d force;
	force.x = accel.x * ACCEL_RATIO;
	force.y = -accel.y * ACCEL_RATIO;
	disc_apply_force(disc, force);
}

static void disc_update(Disc *disc) {
	const GRect frame = window_frame;
	double e = 0.5;
	// Bounce off of walls
	if ((disc->pos.x - disc->radius < 0 && disc->vel.x < 0)
	    || (disc->pos.x + disc->radius > frame.size.w && disc->vel.x > 0)) {
		disc->vel.x = -disc->vel.x * e;
	}
	if ((disc->pos.y - disc->radius < 0 && disc->vel.y < 0)
	   || (disc->pos.y + disc->radius > frame.size.h && disc->vel.y > 0)) {
		disc->vel.y = -disc->vel.y * e;
	}
	disc->pos.x += disc->vel.x;
	disc->pos.y += disc->vel.y;
}

static void disc_draw(GContext *ctx, Disc *disc) {
	app_log(0, "gamescene.c", 75, "Drawing disc at point (%d, %d)", (int) disc->pos.x, (int) disc->pos.y);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, GPoint(disc->pos.x, disc->pos.y), disc->radius);
}

static void food_draw(GContext *ctx, GPoint *point) {
	app_log(0, "gamescene.c", 80, "Drawing food at point (%d, %d)", point->x, point->y);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_fill_circle(ctx, food, DEFAULT_FOOD_RADIUS);
}

static void disc_layer_update_callback(Layer *me, GContext *ctx) {
	disc_draw(ctx, &disc);
	food_draw(ctx, &food);
}

static float square_root(const float num) {
	const uint MAX_STEPS = 40;
	const float MAX_ERROR = 0.001;

	float answer = num;
	float ans_sqr = answer * answer;
	uint step = 0;
	while((ans_sqr - num > MAX_ERROR) && (step++ < MAX_STEPS)) {
		answer = (answer + (num / answer)) / 2;
		ans_sqr = answer * answer;
	}
	return answer;
}

static float distance(GPoint *p1, GPoint *p2) {
	int x = p1->x - p2->x;
	int y = p1->y - p2->y;
	return square_root((float) (x * x + y * y));
}

static int collision(Disc *disc, GPoint *food) {
	float d = distance(&GPoint(disc->pos.x, disc->pos.y), food);
	return d <= (disc->radius + DEFAULT_FOOD_RADIUS);
}

static void timer_callback(void *data) {
	AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };

	accel_service_peek(&accel);

	disc_apply_accel(&disc, accel);
	disc_update(&disc);

	if (collision(&disc, &food)) {
		score += 1;
		init_food();
	}
	
	layer_mark_dirty(disc_layer);
	timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
}

void graphics_generate_game_scene(Window *w) {
	window = w;
	Layer *window_layer = window_get_root_layer(window);
  	window_frame = layer_get_frame(window_layer);
  	score = 0;

  	disc_layer = layer_create(window_frame);

	window_set_background_color(window, GColorBlack);
	init_disc();
	init_food();

	timer = app_timer_register(ACCEL_STEP_MS, timer_callback, NULL);
  	layer_set_update_proc(disc_layer, disc_layer_update_callback);
  	layer_add_child(window_layer, disc_layer);
}

int graphics_destroy_game_scene() {
	layer_destroy(disc_layer);
	app_timer_cancel(timer);
	window_set_background_color(window, GColorWhite);
	return score;
}