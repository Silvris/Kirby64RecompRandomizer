#include "modding.h"
#include "Player.h"
#include "Archipelago.h"


RECOMP_HOOK_RETURN("func_80222038_ovl19") void wallDedede() {
    if(!HasFriend(KingDedede)){
        gKirbyState.abilityState = 0;
    }
}

RECOMP_HOOK_RETURN("func_80227F90_ovl19") void cartWaddleDee() {
    if(!HasFriend(WaddleDee)){
        gKirbyState.abilityState = 0;
    }
}
