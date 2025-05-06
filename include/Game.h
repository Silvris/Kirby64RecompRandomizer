#include "types.h"

struct CrystalLevel{
    u8 Crystal1 : 1;
    u8 Crystal2 : 1;
    u8 Crystal3 : 1;
    u8 high : 5;
};

extern s32 gGameState;
extern s32 gCurrentSaveFile;
extern s32 gSave1Stage;
extern s32 gSave2Stage;
extern s32 gSave3Stage;
extern s32 gLoadedLevel;
extern s32 gLoadedStage;
extern s32 gLoadedRoom;
extern s32 gUnlockedLevel;
extern s32 gUnlockedStage;
extern s32 gSelectedLevel;
extern u8  gBossCrystals[5];
extern struct CrystalLevel gCrystalArray[24];
extern u64 gCopyAbilityUnlocked;
extern s32 gCrystalShards;
extern u32 gReceivedIndex;
extern bool gDeathLinkSwitch;
extern u32 gDarkStarUnlock;
extern bool gFriendUnlocked[3];