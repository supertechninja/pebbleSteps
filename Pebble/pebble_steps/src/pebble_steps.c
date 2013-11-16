#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"
#include "resource_ids.auto.h"
#include <stdint.h>
#include <string.h>

#define BITMAP_BUFFER_BYTES 1024

// 42c86ea4-1c3e-4a07-b889-2cccca914198
#define MY_UUID {0x42, 0xc8, 0x6e, 0xa4, 0x1c, 0x3e, 0x4a, 0x7, 0xb8, 0x89, 0x2c, 0xcc, 0xca, 0x91, 0x41, 0x98}
PBL_APP_INFO(MY_UUID, "Pebble Steps", "Pebble Technology", 0x1, 0x0, RESOURCE_ID_APP_ICON, APP_INFO_STANDARD_APP);

#define SM_OPEN_SIRI_KEY 0xFC03
#define STRING_LENGTH 255
	
  Window window;
  TextLayer temperature_layer, text_weather_temp_layer;
  TextLayer steps_text_header;
  TextLayer distance_title;
  TextLayer distance_layer;
  AppSync sync;
  uint8_t sync_buffer[32];
  char distance_string;


enum {     // TUPLE_INT
  WEATHER_TEMPERATURE_KEY = 0x0,  // TUPLE_CSTRING
  DISTANCE_KEY = 0x1	
};

// TODO: Error handling

//send update request to app
static void send_cmd() {
    DictionaryIterator *iter;
    app_message_out_get(&iter);
    
    if (iter == NULL)
        return;
    
    dict_write_int8(iter, WEATHER_TEMPERATURE_KEY, -1);
	dict_write_int8(iter, DISTANCE_KEY, -1);
    dict_write_end(iter);
    
    app_message_out_send();
    app_message_out_release();
}

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {

  switch (key) {
  case WEATHER_TEMPERATURE_KEY:
    // App Sync keeps the new_tuple around, so we may use it directly
    text_layer_set_text(&temperature_layer, new_tuple->value->cstring);
	
  break;

  default:
    return;
  }
}

//AppMessage Callbacks
void my_out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
}
void my_out_fail_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
}
void my_in_rcv_handler(DictionaryIterator *received, void *context) {
  // incoming message received
	Tuple *t;
	t = dict_find(received, WEATHER_TEMPERATURE_KEY);
	text_layer_set_text(&temperature_layer, t->value->cstring);	
	t = dict_find(received, DISTANCE_KEY);

    strcpy(&distance_string, t->value->cstring); /* copy name into the new var */
    strcat(&distance_string, " mi");
	
	text_layer_set_text(&distance_layer, &distance_string);			
}
void my_in_drp_handler(void *context, AppMessageResult reason) {
  // incoming message dropped
}

//Click handlers
void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
  //... called on single click ...
	vibes_short_pulse();
	text_layer_set_text(&temperature_layer, "Refreshing...");
	send_cmd();
	  
}
void select_multi_click_handler(ClickRecognizerRef recognizer, Window *window) {
  //... called for multi-clicks ...
   // const uint16_t count = click_number_of_clicks_counted(recognizer);
}
void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
  ///... called on long click start ...
}
void select_long_click_release_handler(ClickRecognizerRef recognizer, Window *window) {
 // ... called when long click is released ...
}

//Click providers
void config_provider(ClickConfig **config, Window *window) {
 // single click / repeat-on-hold config:
  config[BUTTON_ID_SELECT]->click.handler = (ClickHandler) select_single_click_handler;
  config[BUTTON_ID_SELECT]->click.repeat_interval_ms = 1000; // "hold-to-repeat" gets overridden if there's a long click handler configured!
  // multi click config:
  config[BUTTON_ID_SELECT]->multi_click.handler = (ClickHandler) select_multi_click_handler;
  config[BUTTON_ID_SELECT]->multi_click.min = 2;
  config[BUTTON_ID_SELECT]->multi_click.max = 10;
  config[BUTTON_ID_UP]->multi_click.last_click_only = true;
  // long click config:
  config[BUTTON_ID_SELECT]->long_click.handler = (ClickHandler) select_long_click_handler;
  config[BUTTON_ID_SELECT]->long_click.release_handler = (ClickHandler) select_long_click_release_handler;
  config[BUTTON_ID_SELECT]->long_click.delay_ms = 700;
}
//App initilization
static void weather_app_init(AppContextRef c) {

  window_init(&window, "Pebble Steps");
  window_set_background_color(&window, GColorWhite);
  //window_set_fullscreen(&window, true);
	
   window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);
	
  window_stack_push(&window, true);
	
  text_layer_init(&steps_text_header, GRect(0, 10, 144, 68));
  text_layer_set_text_color(&steps_text_header, GColorBlack);
  text_layer_set_background_color(&steps_text_header, GColorClear);
  text_layer_set_font(&steps_text_header, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&steps_text_header, GTextAlignmentCenter);
  layer_add_child(&window.layer, &steps_text_header.layer);
  text_layer_set_text(&steps_text_header, "Total Steps"); 	

  text_layer_init(&temperature_layer, GRect(0, 40, 144, 68));
  text_layer_set_text_color(&temperature_layer, GColorBlack);
  text_layer_set_background_color(&temperature_layer, GColorClear);
  text_layer_set_font(&temperature_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&temperature_layer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &temperature_layer.layer);
	
  text_layer_init(&distance_title, GRect(0, 70, 144, 68));
  text_layer_set_text_color(&distance_title, GColorBlack);
  text_layer_set_background_color(&distance_title, GColorClear);
  text_layer_set_font(&distance_title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&distance_title, GTextAlignmentCenter);
  layer_add_child(&window.layer, &distance_title.layer);
  text_layer_set_text(&distance_title, "Total Distance"); 		
	
  text_layer_init(&distance_layer, GRect(0, 100, 144, 68));
  text_layer_set_text_color(&distance_layer, GColorBlack);
  text_layer_set_background_color(&distance_layer, GColorClear);
  text_layer_set_font(&distance_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(&distance_layer, GTextAlignmentCenter);
  layer_add_child(&window.layer, &distance_layer.layer);
  text_layer_set_text(&distance_layer, "0 mi"); 

  
	
  Tuplet initial_values[] = {
    TupletCString(WEATHER_TEMPERATURE_KEY, "0"),
	TupletCString(DISTANCE_KEY, "0 mi")
		
  };
  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
                sync_tuple_changed_callback, sync_error_callback, NULL);
	
  //window_set_click_config_provider(&window, (ClickConfigProvider) click_config_provider);
  //window_set_click_config_provider(&window, (ClickConfigProvider) config_provider);
	

  
}

static void weather_app_deinit(AppContextRef c) {
  app_sync_deinit(&sync);
}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &weather_app_init,
    .deinit_handler = &weather_app_deinit,
    .messaging_info = {
      .buffer_sizes = {
        .inbound = 124,
        .outbound = 256,
      },
	  .default_callbacks.callbacks = {
        .out_sent = my_out_sent_handler,
        .out_failed = my_out_fail_handler,
        .in_received = my_in_rcv_handler,
        .in_dropped = my_in_drp_handler,
      }
	}
	};
  app_event_loop(params, &handlers);
}
