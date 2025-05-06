#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "types.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/stdint.h"
#include "Player.h"
#include "Game.h"

RECOMP_HOOK_RETURN("func_800A2C80") void post_game_tick(){
    // lots of ground to cover
    // set data storage and print to log current stage
    if (gGameState == 0xC){
        // we're in a menu
        recomp_printf("Current Stage: %d_S", gLevelMenu);
    }
    if (gGameState == 0xF){
        // we're in a stage
        recomp_printf("Current Stage: %d_%d", gLoadedLevel, gLoadedStage);
    }
    if (recomp_get_config_u32("enum_option") == 1){
        gCopyAbilityUnlocked = 0xFFFFFFFFFFFFFFFF;
    }
    else {
        gCopyAbilityUnlocked = 0;
    }
}