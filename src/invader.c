#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

// 144x168

#define MY_UUID { 0xEF, 0x6D, 0x8E, 0x4F, 0xEB, 0xD0, 0x48, 0x4B, 0xB5, 0x19, 0x6C, 0x7B, 0x2C, 0xE9, 0xD7, 0xB4 }
PBL_APP_INFO(MY_UUID,
             "Zingy Animated", "Roberto Hortal",
             0, 1, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;

TextLayer timeLayer; // The clock

Layer imageLayer; // where we'll draw Zingy

BmpContainer zingy1_image;
BmpContainer zingy2_image;



void draw_layer_image(Layer *me, GContext* ctx) {
  (void)me;
  (void)ctx;

  // We make sure the dimensions of the GRect to draw into
  // are equal to the size of the bitmap--otherwise the image
  // will automatically tile. Which might be what *you* want.

  PblTm tick_time;
  get_time(&tick_time);

  if ( ( tick_time.tm_sec % 2 ) == 0 ) {
	GRect destination = layer_get_frame(&zingy1_image.layer.layer);
	graphics_draw_bitmap_in_rect(ctx, &zingy1_image.bmp, destination);
  } else {
	GRect destination = layer_get_frame(&zingy2_image.layer.layer);
	graphics_draw_bitmap_in_rect(ctx, &zingy2_image.bmp, destination);
  }

}


// Called once per second
void handle_second_tick(AppContextRef ctx, PebbleTickEvent *t) {

  (void)t;
  (void)ctx;

  static char timeText[] = "00:00"; // Needs to be static because it's used by the system later.

  PblTm currentTime;

  get_time(&currentTime);

  string_format_time(timeText, sizeof(timeText), "%R", &currentTime);

  text_layer_set_text(&timeLayer, timeText);

  layer_mark_dirty( &imageLayer ); // request the image layer redraw itself later

}


void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Zingy");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorWhite);

  text_layer_init(&timeLayer, GRect(50, 134, 144-40 /* width */, 168-130 /* height */));
  text_layer_set_text_color(&timeLayer, GColorBlack);
  text_layer_set_background_color(&timeLayer, GColorClear);
  text_layer_set_font(&timeLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_second_tick(ctx, NULL);

  //layer_add_child(&window.layer, &timeLayer.layer);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_ZINGY1, &zingy1_image);
  bmp_init_container(RESOURCE_ID_ZINGY2, &zingy2_image);

  layer_init(&imageLayer, GRect( 6, 6, 132, 224));
  imageLayer.update_proc = &draw_layer_image;
  layer_add_child(&window.layer, &imageLayer);
  
  // Moved here so text doesn't get covered by the image
  layer_add_child(&window.layer, &timeLayer.layer);

}

void handle_deinit(AppContextRef ctx) {
	(void)ctx;
  
	bmp_deinit_container(&zingy1_image);
	bmp_deinit_container(&zingy2_image);
}



void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
	.deinit_handler = &handle_deinit,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
