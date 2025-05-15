#include "Archipelago.h"
#include "recompui.h"
#include "apconnect_ui.h"
#include "menus.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "os_thread.h"

void createUiFrame(RecompuiContext context, UiFrame* frame) {
    RecompuiColor bg_color;
    bg_color.r = 255;
    bg_color.g = 255;
    bg_color.b = 255;
    bg_color.a = 0.1f * 255;

    RecompuiColor border_color;
    border_color.r = 255;
    border_color.g = 255;
    border_color.b = 255;
    border_color.a = 0.7f * 255;

    RecompuiColor modal_color;
    modal_color.r = 8;
    modal_color.g = 7;
    modal_color.b = 13;
    modal_color.a = 0.9f * 255;

    const float modal_border_width = 5.1f;
    const float modal_border_radius = 16.0f;

    frame->root = recompui_context_root(context);
    // Set up the root element so it takes up the full screen.
    recompui_set_position(frame->root, POSITION_ABSOLUTE);
    recompui_set_top(frame->root, 0, UNIT_DP);
    recompui_set_right(frame->root, 0, UNIT_DP);
    recompui_set_bottom(frame->root, 0, UNIT_DP);
    recompui_set_left(frame->root, 0, UNIT_DP);
    recompui_set_width_auto(frame->root);
    recompui_set_height_auto(frame->root);

    // Set up the root element's background color so the modal contents don't touch the screen edges.
    recompui_set_background_color(frame->root, &bg_color);

    // Set up the flexbox properties of the root element.
    recompui_set_flex_direction(frame->root, FLEX_DIRECTION_COLUMN);
    recompui_set_justify_content(frame->root, JUSTIFY_CONTENT_CENTER);
    recompui_set_align_items(frame->root, ALIGN_ITEMS_CENTER);

    // Create a container to act as the modal background and hold the elements in the modal.
    frame->container = recompui_create_element(context, frame->root);

    // Set the container's size to grow based on the child elements.
    recompui_set_flex_grow(frame->container, 0.0f);
    recompui_set_flex_shrink(frame->container, 0.0f);
    recompui_set_width_auto(frame->container);
    recompui_set_height_auto(frame->container);

    // Set up the properties of the container.
    recompui_set_display(frame->container, DISPLAY_BLOCK);
    recompui_set_padding(frame->container, 16.0f, UNIT_DP);
    recompui_set_align_items(frame->container, ALIGN_ITEMS_STRETCH);
    
    // Set up the container to be the modal's background.
    recompui_set_border_width(frame->container, modal_border_width, UNIT_DP);
    recompui_set_border_radius(frame->container, modal_border_radius, UNIT_DP);
    recompui_set_border_color(frame->container, &border_color);
    recompui_set_background_color(frame->container, &modal_color);
}

#define C_TO_PARAMS(c) (c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF

void randoStart(bool multiworld) {
    rando_started = true;
    is_multiworld = multiworld;
}

#define MAIN_THREAD_STACK_LEN 0x4000
#define MAIN_THREAD_STACK_LEN_U64 ((MAIN_THREAD_STACK_LEN) / sizeof(u64))
#define OS_PRIORITY_IDLE 0
#define STACK_TOP_MAGIC 0x00000000FEDCBA98

extern u64 gGameThreadStack[];
extern void crash_screen_start_thread();
extern void thread5_game(void*);
extern OSThread gGameThread;
extern s32 D_8003DC94;
OSThread* APThread;

void threadx_archipelago(void *arg) {
    while(1){
        recomp_printf("Printing from a new thread!\n");
        osSetThreadPri(APThread, 1);
        break;
    }
}

RECOMP_HOOK_RETURN("cboot") void on_boot(){
    randoCreateAPConnectMenu();
    ShowArchipelagoConnect();
}