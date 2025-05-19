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

int levelRemap[28] = {
    0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 0, 1, 2, 3, 4, 5
};

int stageRemap[28] = {
    0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 4, 4, 4, 4, 3
};

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

int power_combo_offset[8] = {
    0, 7, 14, 20, 25, 29, 32, 34
};

void handle_copy_ability(u32 ability){
    //check the trivial case
    gCopyAbilityUnlocked |= (1 << (ability - 1));
    if (gSlotData.SplitPowerCombos){
        return;
    }
    // on non-split, need to handle the copy ability
    for (int i = 1; i < 8; i++){
        for(int j = i; j < 8; j++){
            if ((gCopyAbilityUnlocked & (1 << (i-1))) && (gCopyAbilityUnlocked & (1 << (j-1)))){
                gCopyAbilityUnlocked |= (1 << (power_combo_offset[i] + (j-i)));
            }
        }
    }

}

static bool save_init = false;

extern void play_sound(int sfx);
RECOMP_HOOK_RETURN("func_8000256C") void on_main(){
    //recomp_printf("main loop?\n");
    if (!save_init && rando_is_connected()) {
        // Set the filename based on the seed and session type.
        char file_name[72];
        char multi_prefix[7] = "multi_";
        char solo_prefix[7] = "solo___";
        rando_get_seed_name(file_name + 7, sizeof(file_name) - 7);
        for (int i = 0; i < 7; i++){
            file_name[i] = multi_prefix[i];
        }
        recomp_change_save_file(file_name);
        play_sound(1);
        u64 player_levels = rando_get_slotdata_raw("player_levels");
        u64 boss_requirements = rando_get_slotdata_raw("boss_requirements");
        u64 current_level;
        char  level_ind[2];
        for (u32 i = 1; i < 7; i++){
            int len = 3;
            if (1 < i < 6) len++;
            // this might be the worst code i've ever written
            level_ind[0] = (char) ('0' + i);
            level_ind[1] = ""[0];
            recomp_printf("%s\n", level_ind);
            current_level = rando_access_slotdata_raw_dict(player_levels, level_ind);
            recomp_printf("%x\n", current_level);
            for(u32 j = 0; j < len; j++){
                s32 lvl = * (s32*)rando_access_slotdata_raw_array(current_level, j) & 0xFFF;
                recomp_printf("%i\n", lvl);
                if (lvl & 0x200) {
                    lvl = (lvl & 0xFF) + 22;
                }
                else {
                    lvl--;
                }
                gLevelIndex[j + ((i - 1) << 3)] = levelRemap[lvl];
                gStageIndex[j + ((i - 1) << 3)] = stageRemap[lvl];
            }
        }
        for (u32 i = 0; i < 7; i++){
            s32 requirement = * (s32*) rando_access_slotdata_raw_array(boss_requirements, i);
            gCrystalRequirement[i] = requirement;
        }
        gSlotData.SplitPowerCombos = rando_get_slotdata_u32("split_power_combos");
        gSlotData.GoalSpeed = rando_get_slotdata_u32("goal_speed");
        gSlotData.DeathLink = rando_get_slotdata_u32("death_link");
        //colors_set_human_tunic(C_TO_PARAMS(rando_get_tunic_color()));
        save_init = true;
    }
    if(rando_is_connected()){
        // rando main loop
        u32 item_list = rando_get_items_size();
        if (item_list > gReceivedIndex){
            u32 received_item = rando_get_item(gReceivedIndex);
            u32 received_group = received_item & 0xF00;
            gReceivedIndex++;
            switch (received_group) {
                case 0:
                    // copy ability or misc item, check first
                    recomp_printf("Received item: %i", received_item & 0xFF);
                    switch(received_item & 0xFF){
                        case 0x20:
                            gCrystalShards++;
                            play_sound(203);
                            break;
                        case 0x21:
                            gKirbyLives++;
                            gKirbyLivesVisual++;
                            play_sound(1);
                            break;
                        case 0x22:
                            gKirbyHp = 6.0f;
                            gKirbyHpVisual = 6;
                            play_sound(202);
                            break;
                        case 0x23:
                            gKirbyState.actionChange = 0x100;
                            play_sound(202);
                            break;
                        default:
                            handle_copy_ability(received_item & 0xFF);
                            play_sound(625);
                            break;
                    }
                    break;
                case 0x100:
                    // friend
                    gFriendUnlocked[received_item & 0xF] = 1;
                    play_sound(625);
                    break;
                case 0x200:
                    gCopyAbilityUnlocked |= (1 << (7 + (received_item & 0xFF)));
                    play_sound(625);
                    break;
            }
        }
    }
}