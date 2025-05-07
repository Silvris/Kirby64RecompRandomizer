#include "types.h"
#include "Game.h"

struct SlotData{
    u8 SplitPowerCombos;
    u8 DeathLink;
    u8 GoalSpeed;
    u8 filler[13];
};

extern u8  gArchipelago[16];
extern u8  gLevelStart[8];
extern u8  gCrystalRequirement[8];
extern struct SlotData gSlotData;
extern u32 gLevelIndex[48];
extern u32 gStageIndex[48];

bool InitArchipelagoVariables();
bool HasFriend(int index);
s32 GetLevelStageIndex(s32 level, s32 stage);
//bool SetArchipelagoVariables();

//bool SetKirbyFlavor(int flavor);