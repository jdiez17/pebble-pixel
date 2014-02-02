#include <pebble.h>
#include <time.h>

Window* window;
BitmapLayer* background;
Layer* w_layer;

BitmapLayer* numbers[4];
GBitmap* loaded_bitmaps[4] = {NULL, NULL, NULL, NULL};
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

    if(loaded_bitmaps[idx] != NULL)  {
        gbitmap_destroy(loaded_bitmaps[idx]);
        layer_remove_from_parent(bitmap_layer_get_layer(numbers[idx]));
        bitmap_layer_destroy(numbers[idx]);

        loaded_bitmaps[idx] = NULL;
    }
}
    
void load_number(int row, int pos, int resid) {
    int idx = 2 * row + pos;

    unload_number(row, pos);
    
    loaded_bitmaps[idx] = gbitmap_create_with_resource(resources[resid]);
    GRect frame = (GRect) {
        .origin = {
            .x = (pos == 0) ? 25 : 77,
            .y = (row == 0) ? 11 : 89
        }, 
        .size = loaded_bitmaps[idx]->bounds.size
    };
    numbers[idx] = bitmap_layer_create(frame);
    bitmap_layer_set_bitmap(numbers[idx], loaded_bitmaps[idx]);
    layer_add_child(w_layer, bitmap_layer_get_layer(numbers[idx]));
}

void show_time(struct tm* t) {
    unsigned short hour = (clock_is_24h_style() ? t->tm_hour : (t->tm_hour > 12 ? t->tm_hour - 12 : t->tm_hour));
    
    load_number(0, 0, hour / 10);
    load_number(0, 1, hour % 10);
    load_number(1, 0, t->tm_min / 10);
    load_number(1, 1, t->tm_min % 10);
}

void handle_minute_tick(struct tm* t, TimeUnits delta_t) {
    show_time(t);
}

void handle_init(void) {
    window = window_create();
    window_stack_push(window, true);
    w_layer = window_get_root_layer(window);

    background = bitmap_layer_create(layer_get_frame(w_layer));
    bitmap_layer_set_bitmap(background, gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND));
    layer_add_child(w_layer, bitmap_layer_get_layer(background));

    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    show_time(t);
}

void handle_deinit(void) {
    window_destroy(window);
    bitmap_layer_destroy(background);
    
    for(int i = 0; i < 4; i++)
        unload_number(i >> 1, i & 1);
    
    tick_timer_service_unsubscribe();
}

int main(void) {
    handle_init();
    app_event_loop();
    handle_deinit();
}
