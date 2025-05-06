#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "types.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/stdint.h"

// Patches a function in the base game that's used to check if the player should quickspin.
RECOMP_HOOK("play_sound") s32 on_play_sound(s32 sound) {
    recomp_printf("enum_option: %d\n", recomp_get_config_u32("enum_option"));
    recomp_printf("number_option: %d\n", recomp_get_config_double("number_option"));
    char* string_option = recomp_get_config_string("string_option");
    if (string_option != NULL) {
        recomp_printf("string_option: %s\n", string_option);
        recomp_free_config_string(string_option);
    }
    recomp_printf("played_sfx: %d\n", sound);
    // Always spin attack.
    return true;
}


