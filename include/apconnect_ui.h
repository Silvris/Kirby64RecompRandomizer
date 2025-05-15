#pragma once
#include "recompui.h"
#include "menus.h"

typedef struct {
    RecompuiContext context;
    UiFrame frame;

    RecompuiResource server_label;
    RecompuiResource slotname_label;
    RecompuiResource password_label;

    RecompuiResource server_textinput;
    RecompuiResource slot_textinput;
    RecompuiResource password_textinput;

    RecompuiResource connect_button;
} ApconnectMenu;

void randoCreateAPConnectMenu();
void ShowArchipelagoConnect();