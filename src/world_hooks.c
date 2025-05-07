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
        recomp_printf("Current Stage: %d_S\n", gSelectedLevel);
    }
    if (gGameState == 0xF){
        // we're in a stage
        recomp_printf("Current Stage: %d_%d\n", gLoadedLevel, gLoadedStage);
    }
    if (recomp_get_config_u32("enum_option") == 1){
        gCopyAbilityUnlocked = 0xFFFFFFFFFFFFFFFF;
    }
    else {
        gCopyAbilityUnlocked = 0;
    }
}

RECOMP_PATCH s32 func_8015874C_ovl4(void) {
    recomp_printf("Checking Dark Star");
    if (gDarkStarUnlock) return 6;
    return 5;
}

RECOMP_HOOK_RETURN("init_save_file_maybe") void post_init_save(){
    recomp_printf("Save file initialized");
    gUnlockedStage = 3;
    switch(gCurrentSaveFile){
        case 0:
            gSave1Stage = 3;
            break;
        case 1:
            gSave2Stage = 3;
            break;
        case 2:
            gSave3Stage = 3;
            break;
        default:
            break;
    }
}

RECOMP_PATCH u32 func_80153294_ovl4(u32 a1, u32 a2, u32 a3) {
    // you ever just match a new function just to replace it
    u32 temp_a0 = (a1 << 2) + a2;
    if ((gCrystalArray[temp_a0] & (1 << a3))){
        return 1;
    };
    return 0;
}