#pragma once
#include "types.h"
#include "modding.h"
#include "Game.h"


RECOMP_IMPORT(".", bool rando_location_is_checked(u32 location_id));
RECOMP_IMPORT(".", bool rando_location_is_checked_async(u32 location_id));
RECOMP_IMPORT(".", bool rando_get_death_link_pending());
RECOMP_IMPORT(".", void rando_reset_death_link_pending());
RECOMP_IMPORT(".", bool rando_get_death_link_enabled());
RECOMP_IMPORT(".", void rando_send_death_link());
RECOMP_IMPORT(".", void rando_send_location(u32 location_id));
RECOMP_IMPORT(".", void rando_complete_goal());
RECOMP_IMPORT(".", u32 rando_has_item(u32 item_id));
RECOMP_IMPORT(".", u32 rando_has_item_async(u32 item_id));
RECOMP_IMPORT(".", u32 rando_get_own_team_id());
RECOMP_IMPORT(".", u32 rando_get_own_slot_id());
RECOMP_IMPORT(".", u32 rando_get_items_size());
RECOMP_IMPORT(".", u32 rando_get_item(u32 items_i));
RECOMP_IMPORT(".", u32 rando_get_sending_player(u32 items_i));
RECOMP_IMPORT(".", void rando_get_item_name_from_id(u32 item_id, char* out_str));
RECOMP_IMPORT(".", void rando_get_sending_player_name(u32 items_i, char* out_str));
RECOMP_IMPORT(".", void rando_get_location_item_player(u32 location_id, char* out_str));
RECOMP_IMPORT(".", void rando_get_location_item_name(u32 location_id, char* out_str));
RECOMP_IMPORT(".", u32 rando_get_last_location_sent());
RECOMP_IMPORT(".", u32 rando_get_seed_name(char* seed_name_out, u32 buffer_size));

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