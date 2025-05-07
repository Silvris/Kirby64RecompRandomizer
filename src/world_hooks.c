#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "types.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/stdint.h"
#include "Player.h"
#include "Game.h"
#include "Save.h"
#include "Archipelago.h"

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
    if (recomp_get_config_u32("debug_copy") == 1){
        gCopyAbilityUnlocked = 0xFFFFFFFFFFFFFFFF;
    }
    else {
        gCopyAbilityUnlocked = 0;
    }
    if (recomp_get_config_u32("debug_dark") == 1){
        gDarkStarUnlock = true;
    }
    else {
        gDarkStarUnlock = false;
    }
    if (recomp_get_config_u32("debug_shift") == 1) {
        gLevelIndex[0] = 5;
    }
    else {
        gLevelIndex[0] = 0;
    }
    gCrystalShards = recomp_get_config_u32("debug_crystals");
}

RECOMP_PATCH s32 func_8015874C_ovl4(void) {
    recomp_printf("Checking Dark Star\n");
    if (gDarkStarUnlock) return 6;
    return 5;
}

RECOMP_HOOK_RETURN("init_save_file_maybe") void post_init_save(){
    //recomp_printf("Save file initialized\n");
    gSaveBuffer1.files[gCurrentSaveFile].level = 3;
    gSaveBuffer2.files[gCurrentSaveFile].level = 3;
}

RECOMP_PATCH u32 func_80153294_ovl4(u32 level, u32 stage, u32 crystal) {
    // you ever just match a new function just to replace it
    u32 level_stage = GetLevelStageIndex(level, stage);
    level = gLevelIndex[level_stage];
    stage = gStageIndex[level_stage];
    u32 offset = (level << 2) + stage;
    if ((gCrystalArray[offset] & (1 << crystal))){
        return 1;
    };
    return 0;
}