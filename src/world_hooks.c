#include "modding.h"
#include "recompui.h"
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
#include "menus.h"

RECOMP_HOOK_RETURN("func_800A2C80") void post_game_tick(){
    // lots of ground to cover
    // set data storage and print to log current stage'
    recomp_printf("Game State: 0x%x\n", gGameState);
    if (gGameState == 0xC){
        // we're in a menu
        recomp_printf("Current Stage: %d_S\n", gSelectedLevel);
    }
    if (gGameState == 0xF){
        // we're in a stage
        recomp_printf("Current Stage: %d_%d\n", gLoadedLevel, gLoadedStage);
    }
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

static bool save_init = false;

RECOMP_HOOK_RETURN("func_8000256C") void on_main(){
    //recomp_printf("main loop?\n");
    if (!save_init && rando_is_connected()) {
        // Set the filename based on the seed and session type.
        char file_name[72];
        char multi_prefix[7] = "multi_";
        char solo_prefix[7] = "solo___";
        rando_get_seed_name(file_name + 7, sizeof(file_name) - 7);
        recomp_printf(file_name);
        if (is_multiworld) {
            for (int i = 0; i < 7; i++){
                file_name[i] = multi_prefix[i];
            }
        }
        else {
            for (int i = 0; i < 7; i++){
                file_name[i] = solo_prefix[i];
            }
        }
        recomp_change_save_file(file_name);
        rando_get_player_levels();
        //colors_set_human_tunic(C_TO_PARAMS(rando_get_tunic_color()));
        save_init = true;
    }//*/
}