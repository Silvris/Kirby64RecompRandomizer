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
            recomp_printf("Copy Ability: %d\n", gKirbyState.ability);
            func_8012310C(temp_a0);
        }
        gKirbyState.secondInhale = 0;
        gKirbyState.firstInhale = 0;
        gKirbyState.currentInhale = 0;
    }
}

struct normal {
	float x;
	float y;
	float z;
	float originOffset;
};
typedef struct normal Normal;

struct CollisionTriangle
{
/*0x0*/     u16    vertex[3];
/*0x6*/     u16    polyCount;
/*0x8*/     u16    normalType;          //(1 forward norm, 2 back norm, 4 no shadow, 8 non solid)
/*0xA*/     u16    collisionIndex; //based on col type this num references array pos
/*0xC*/     u16    breakParticle;       //(seen in DEDEDE hammer break)
/*0xE*/     u16    Halt_Movement;        //Stops kirby from moving/triggers automatic behavior.
/*0x10*/    u16    collisionParameter;           //ex.Amount to move kirby while on certain col types or Break Condition
/*0x12*/    u16    collisionType;       //see col type list
};

struct ColStateUnk4 {
    u16 cell;
    f32 projection; // how far kirby is from the plane
    // u16 unk6;
};



struct CollisionState {
    s32 numCells;

    struct ColStateUnk4 *unk4; // valid hits for normal cells

    /* 0x08 */ Vector currPos;

    /* 0x14 */ Vector nextPos;

    /* 0x20 */ Vector deltaPos;

    /* 0x2C */ struct Normal *someNormal;

    struct vCollisionHeader *unk30;

    struct Normal *unk34;
    struct Normal *unk38;
    u32 (*unk3C)(void);
    u8 (*unk40)(struct CollisionTriangle *a0, struct Normal *a1, struct Normal *a2, struct Normal *a3);
    u8 (*unk44)(struct Normal *a0, s32 arg1);
    u16 unk48;
    u16 unk4A;
    u16 unk4C;
    u16 unk4E;
};

extern struct CollisionState *gCollisionState;
extern u8 func_80101F4C(struct CollisionTriangle*, struct Normal*, struct Normal*, struct Normal*);
extern u32 func_80103D80(u32, u32, u32, u32);

extern u8 func_80102364(struct Normal *a0, s32 arg1);

// DeathLink on collision
RECOMP_PATCH u32 func_8010474C(Vector *v0, Vector *v1) {
    struct CollisionState newColState;
    gCollisionState = &newColState;
    newColState.currPos = *v0;
    newColState.nextPos = *v1;
    newColState.someNormal = NULL;
    newColState.unk3C = NULL;
    newColState.unk44 = func_80102364;
    newColState.unk40 = func_80101F4C;
    newColState.unk34 = NULL;
    newColState.unk38 = NULL;
    if (gDeathLinkSwitch){
        gDeathLinkSwitch = false;
        return true;
    }
    return func_80103D80(0, 0, 0, 0);
}