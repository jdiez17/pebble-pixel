#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0xDE, 0xEF, 0xAD, 0xDE, 0xFE, 0xAC, 0xBE, 0xEF, 0x99, 0xEF, 0xBE, 0xEF, 0xBE, 0xEF, 0xBE, 0xEF }
PBL_APP_INFO(MY_UUID,
    "Pixel Watch",
    "Jdiez",
    1, 0,
    DEFAULT_MENU_ICON,
    APP_INFO_WATCH_FACE);
    
Window window;
BmpContainer background;

BmpContainer numbers[4];
int states[4] = {-1, -1, -1, -1};
int resources[10] = {
    RESOURCE_ID_IMAGE_ZERO,
    RESOURCE_ID_IMAGE_ONE, 
    RESOURCE_ID_IMAGE_TWO,
    RESOURCE_ID_IMAGE_THREE,
    RESOURCE_ID_IMAGE_FOUR,
    RESOURCE_ID_IMAGE_FIVE,
    RESOURCE_ID_IMAGE_SIX,
    RESOURCE_ID_IMAGE_SEVEN,
    RESOURCE_ID_IMAGE_EIGHT,
    RESOURCE_ID_IMAGE_NINE
};

void unload_number(int row, int pos) {
    int idx = 2 * row + pos;

    if(states[idx] != -1) {
        layer_remove_from_parent(&numbers[idx].layer.layer);
        bmp_deinit_container(&numbers[idx]);
        states[idx] = -1;
    }
} 
    
void load_number(int row, int pos, int resid) {
    int idx = 2 * row + pos;

    unload_number(row, pos);
    
    states[idx] = resid;
    bmp_init_container(resid, &numbers[idx]);
    numbers[idx].layer.layer.frame.origin.x = (pos == 0) ? 25 : 77;
    numbers[idx].layer.layer.frame.origin.y = (row == 0) ? 11 : 89;
    layer_add_child(&window.layer, &numbers[idx].layer.layer);
}

void show_time(PblTm* t) {
    load_number(0, 0, resources[t->tm_hour / 10]);
    load_number(0, 1, resources[t->tm_hour % 10]);
    load_number(1, 0, resources[t->tm_min / 10]);
    load_number(1, 1, resources[t->tm_min % 10]);
}

void handle_init(AppContextRef ctx) {
    (void) ctx;
    
    window_init(&window, "Pixel Watch");
    window_stack_push(&window, true);
    
    resource_init_current_app(&PIXELWATCH);
    
    bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background);
    layer_add_child(&window.layer, &background.layer.layer);
    
    PblTm t;
    get_time(&t);
    show_time(&t);
}

void handle_deinit(AppContextRef ctx) {
    (void) ctx;
    
    for(int i = 0; i < 4; i++)
        unload_number(i >> 1, i & 1);
    
    bmp_deinit_container(&background);
}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent* t) {
    (void) ctx;
    
    show_time(t->tick_time);
}

void pbl_main(void *params) {
    PebbleAppHandlers handlers = {
        .init_handler = &handle_init,
        .deinit_handler = &handle_deinit,
        
        .tick_info = {
            .tick_handler = &handle_minute_tick,
            .tick_units = MINUTE_UNIT
        }
    };
    
    app_event_loop(params, &handlers);
}
