#include "Game.h"
#include "game_tick_functions.h"
#include "types.h"
#include "Archipelago.h"

void RedirectStage() {
    // It might be better in the long run to handle this from an AP array rather
    // than in RAM, but it works
    s32 level_stage_index = gLoadedLevel << 5;
    level_stage_index += gSelectedStage << 2;
    gLoadedLevel = gLevelIndex[level_stage_index];
    gLoadedStage = gStageIndex[level_stage_index];
}

void CheckBossAccess() {
    int level_stage_index = 6 * gLoadedLevel;
    level_stage_index += (gLoadedStage - 1);
    //gStageComplete[level_stage_index] = 2;
    u8 required_crystals = gCrystalRequirement[gUnlockedLevel - 1];
    int acquired_crystals = gCrystalShards;
    if (acquired_crystals >= required_crystals) {
        gUnlockedStage++;
    }
    // now check fast goal
    if (gSlotData.GoalSpeed){
        if (CheckDarkStarAccess()){
            gDarkStarUnlock = true;
        }
    }
}

bool MiracleMatterCheck(){
    // little optimization, we don't have to check crystals
    // since this is running after defeating miracle matter
    for (int i = 0; i < 3; i++){
        if (!HasFriend(i)) return false;
    }
    return true;
}

bool CheckDarkStarAccess(){
    for (int i = 0; i < 3; i++){
        if (!HasFriend(i)) return false;
    }
    u8 miracle_matter = gCrystalRequirement[6];
    return gCrystalShards >= miracle_matter;
}