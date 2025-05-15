#pragma once

#include "recompui.h"
#include "types.h"
#include "libc/stdbool.h"

typedef struct {
    RecompuiResource root;
    RecompuiResource container;
} UiFrame;

void randoStart(bool multiworld);
void createUiFrame(RecompuiContext context, UiFrame* frame);