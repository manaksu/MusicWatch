#include <pebble.h>
#include "ipod_image.h"

static Window *s_window;
static Layer  *s_canvas;
static GFont   s_font_time;
static GFont   s_font_ampm;

static void draw_ipod(GContext *ctx) {
  // ── Draw iPod pixel art ──────────────────────────────────────────────────
  // White bg already filled; draw mid (light gray), lite (dark gray),
  // then dark (black) on top — same layering as the source image
  int oy = IPOD_IMG_OY;
  int i;

  // lite = GColorDarkGray
  graphics_context_set_stroke_color(ctx, GColorDarkGray);
  i = 0;
  while (IPOD_LITE[i] != 255) {
    int row=IPOD_LITE[i], x0=IPOD_LITE[i+1], x1=IPOD_LITE[i+2];
    graphics_draw_line(ctx, GPoint(x0, oy+row), GPoint(x1, oy+row));
    i += 3;
  }

  // mid = GColorLightGray
  graphics_context_set_stroke_color(ctx, GColorLightGray);
  i = 0;
  while (IPOD_MID[i] != 255) {
    int row=IPOD_MID[i], x0=IPOD_MID[i+1], x1=IPOD_MID[i+2];
    graphics_draw_line(ctx, GPoint(x0, oy+row), GPoint(x1, oy+row));
    i += 3;
  }

  // dark = GColorBlack (outlines)
  graphics_context_set_stroke_color(ctx, GColorBlack);
  i = 0;
  while (IPOD_DARK[i] != 255) {
    int row=IPOD_DARK[i], x0=IPOD_DARK[i+1], x1=IPOD_DARK[i+2];
    graphics_draw_line(ctx, GPoint(x0, oy+row), GPoint(x1, oy+row));
    i += 3;
  }
}

static void canvas_update_proc(Layer *layer, GContext *ctx) {
  // ── White background ──
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);

  // ── iPod illustration ──
  draw_ipod(ctx);

  // ── Time overlay on screen area ──
  // Screen sits roughly at y=27+18=45 to y=27+82=109, x=28 to x=112
  // Draw time centered in screen
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  char time_buf[6];
  char ampm_buf[3];
  int hour = t->tm_hour % 12;
  if (hour == 0) hour = 12;
  snprintf(time_buf, sizeof(time_buf), "%d:%02d", hour, t->tm_min);
  snprintf(ampm_buf, sizeof(ampm_buf), "%s", t->tm_hour < 12 ? "AM" : "PM");

  graphics_context_set_text_color(ctx, GColorBlack);

  // Time — centered in screen area (x:28-112, y:50-100)
  graphics_draw_text(ctx, time_buf, s_font_time,
    GRect(32, 52, 80, 36),
    GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);

  // AM/PM superscript
  graphics_draw_text(ctx, ampm_buf, s_font_ampm,
    GRect(98, 54, 20, 14),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  s_font_time = fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS);
  s_font_ampm = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);

  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, canvas_update_proc);
  layer_add_child(root, s_canvas);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
}

static void init(void) {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load   = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
