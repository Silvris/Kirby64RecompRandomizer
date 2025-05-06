#include "types.h"

struct CrystalLevel{
    u8 Crystal1 : 1;
    u8 Crystal2 : 1;
    u8 Crystal3 : 1;
    u8 high : 5;
};

extern s32 gGameState;
extern s32 gLoadedLevel;
extern s32 gLoadedStage;
extern s32 gLoadedRoom;
extern s32 gLevelMenu;
extern u8  gBossCrystals[5];
extern struct CrystalLevel gCrystalArray[24];
extern u64 gCopyAbilityUnlocked;
extern s32 gCrystalShards;
extern u32 gReceivedIndex;
extern bool gDeathLinkSwitch;
extern u32 gDarkStarUnlock;
extern bool gFriendUnlocked[3];