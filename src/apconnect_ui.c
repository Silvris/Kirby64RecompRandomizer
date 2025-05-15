#include "recompui.h"
#include "apconnect_ui.h"
#include "menus.h"
#include "Archipelago.h"
#include "recomputils.h"
#include "types.h"

RECOMP_IMPORT(".", bool rando_init(char* game, char* address, char* player_name, char* password));

ApconnectMenu connect_menu;

static void connectPressed(RecompuiResource resource, const RecompuiEventData* data, void* userdata) {
    recomp_printf("%i", data->type);
    if (data->type == UI_EVENT_CLICK) {
        recomp_printf("Pressed connect.");
        char* server_text = recompui_get_input_text(connect_menu.server_textinput);
        char* slot_text = recompui_get_input_text(connect_menu.slot_textinput);
        char* password_text = recompui_get_input_text(connect_menu.password_textinput);
        bool success = rando_init("Kirby 64 - The Crystal Shards", server_text, slot_text, password_text);
        recomp_free(server_text);
        recomp_free(slot_text);
        recomp_free(password_text);

        if (success) {
            randoStart(true);
            recompui_hide_context(connect_menu.context);
            recompui_close_context(connect_menu.context);
            recomp_printf("Successfully connected");
            recompui_open_context(connect_menu.context);
        }
        else {
            recompui_close_context(connect_menu.context);
            recomp_printf("Failed to connect");
            recompui_open_context(connect_menu.context);
        }
    }
}


RecompuiResource create_spacer(RecompuiContext context, RecompuiResource parent, float flex_grow, float flex_shrink, float flex_basis) {
    RecompuiResource ret = recompui_create_element(context, parent);

    recompui_set_flex_basis(ret, flex_basis, UNIT_DP);
    recompui_set_flex_grow(ret, flex_grow);
    recompui_set_flex_shrink(ret, flex_shrink);

    return ret;
}

void randoCreateAPConnectMenu() {
    connect_menu.context = recompui_create_context();
    recompui_open_context(connect_menu.context);

    createUiFrame(connect_menu.context, &connect_menu.frame);

    // Create a label for the server address.
    connect_menu.server_label = recompui_create_label(connect_menu.context, connect_menu.frame.container, "Server Address:Port", LABELSTYLE_NORMAL);
    recompui_set_font_size(connect_menu.server_label, 30.0f, UNIT_DP);

    // Create a text input for the server address.
    connect_menu.server_textinput = recompui_create_textinput(connect_menu.context, connect_menu.frame.container);
    recompui_set_font_size(connect_menu.server_textinput, 30.0f, UNIT_DP);
    recompui_set_input_text(connect_menu.server_textinput, "localhost:38281");
    recompui_set_margin_top(connect_menu.server_textinput, 10.0f, UNIT_DP);
    recompui_set_margin_bottom(connect_menu.server_textinput, 20.0f, UNIT_DP);
    recompui_set_max_width(connect_menu.server_textinput, 100.0f, UNIT_PERCENT);

    // Create a label for the slotname.
    connect_menu.slotname_label = recompui_create_label(connect_menu.context, connect_menu.frame.container, "Slotname", LABELSTYLE_NORMAL);
    recompui_set_font_size(connect_menu.slotname_label, 30.0f, UNIT_DP);

    // Create a text input for the slotname.
    connect_menu.slot_textinput = recompui_create_textinput(connect_menu.context, connect_menu.frame.container);
    recompui_set_font_size(connect_menu.slot_textinput, 30.0f, UNIT_DP);
    recompui_set_input_text(connect_menu.slot_textinput, "Player1");
    recompui_set_margin_top(connect_menu.slot_textinput, 10.0f, UNIT_DP);
    recompui_set_margin_bottom(connect_menu.slot_textinput, 20.0f, UNIT_DP);
    recompui_set_max_width(connect_menu.slot_textinput, 100.0f, UNIT_PERCENT);

    // Create a label for the password.
    connect_menu.password_label = recompui_create_label(connect_menu.context, connect_menu.frame.container, "Password", LABELSTYLE_NORMAL);
    recompui_set_font_size(connect_menu.password_label, 30.0f, UNIT_DP);

    // Create a text input for the password.
    connect_menu.password_textinput = recompui_create_passwordinput(connect_menu.context, connect_menu.frame.container);
    recompui_set_font_size(connect_menu.password_textinput, 30.0f, UNIT_DP);
    recompui_set_input_text(connect_menu.password_textinput, "");
    recompui_set_margin_top(connect_menu.password_textinput, 20.0f, UNIT_DP);
    recompui_set_max_width(connect_menu.password_textinput, 100.0f, UNIT_PERCENT);

    // Create the connect button.
    connect_menu.connect_button = recompui_create_button(connect_menu.context, connect_menu.frame.container, "Connect", BUTTONSTYLE_SECONDARY);
    recompui_set_text_align(connect_menu.connect_button, TEXT_ALIGN_CENTER);
    recompui_set_margin_top(connect_menu.connect_button, 70.0f, UNIT_DP);
    recompui_register_callback(connect_menu.connect_button, connectPressed, NULL);

    recompui_close_context(connect_menu.context);
}

void ShowArchipelagoConnect() {
    recompui_show_context(connect_menu.context);
}