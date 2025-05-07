#include "libc/stdbool.h"
#include "types.h"
#include "Archipelago.h"

int level_start[8] = {0, 3, 4, 4, 4, 4, 3, 1};
int crystal_requirements[8] = {3, 6, 9, 12, 15, 18, 0xFF, 0xFF};
int level_index[48] = {
    0, 0, 0, 0,-1,-1,-1,-1,
    1, 1, 1, 1, 1,-1,-1,-1,
    2, 2, 2, 2, 2,-1,-1,-1,
    3, 3, 3, 3, 3,-1,-1,-1,
    4, 4, 4, 4, 4,-1,-1,-1,
    5, 5, 5, 5,-1,-1,-1,-1,
};
int stage_index[48] = {
    0, 1, 2, 3,-1,-1,-1,-1,
    0, 1, 2, 3, 4,-1,-1,-1,
    0, 1, 2, 3, 4,-1,-1,-1,
    0, 1, 2, 3, 4,-1,-1,-1,
    0, 1, 2, 3, 4,-1,-1,-1,
    0, 1, 2, 3,-1,-1,-1,-1,
};


bool InitArchipelagoVariables(){
    for (int i = 0; i < 8; i++){
        gLevelStart[i] = level_start[i];
        gCrystalRequirement[i] = crystal_requirements[i];
    }
    gSlotData.SplitPowerCombos = false;
    gSlotData.DeathLink = false;
    gSlotData.GoalSpeed = false;
    for (int i = 0; i < 48; i++){
        gLevelIndex[i] = level_index[i];
        gStageIndex[i] = stage_index[i];
    }

    return true;
}

bool HasFriend(int index){
    if (index < 0) return false;
    if (index > 2) return false;
    return gFriendUnlocked[index];
}