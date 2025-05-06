#include "modding.h"
#include "recomputils.h"
#include "recompconfig.h"
#include "types.h"
#include "libc/stddef.h"
#include "libc/stdbool.h"
#include "libc/stdint.h"
#include "Player.h"
#include "Game.h"

extern void func_8012310C(s32);

//Apply Copy Ability
RECOMP_PATCH void func_8011E190(void) {
	u32 temp_a0;
	u32 temp_v0;
	
    if (gGameState != 0x21) {
		temp_a0 = gKirbyState.currentInhale;
		temp_v0 = gKirbyState.currentInhale;
        if ((temp_v0 != 0) && (gCopyAbilityUnlocked & (1 << (temp_v0 - 1)))) {
            gKirbyState.ability = (s32) gKirbyState.currentInhale;
            recomp_printf("Copy Ability: %d", gKirbyState.ability);
            func_8012310C(temp_a0);
        }
        gKirbyState.secondInhale = 0;
        gKirbyState.firstInhale = 0;
        gKirbyState.currentInhale = 0;
    }
}