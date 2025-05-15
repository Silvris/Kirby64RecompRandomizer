#pragma once
#include "types.h"

typedef struct GObj {
	/* 0x00 */ u32 objId;
	/* 0x04 */ struct GObj* unk4;
    /* 0x08 */ struct GObj* unk8;
    /* 0x0C */ u8 link;
    // todo: find the array this indexes
    /* 0x0D */ u8 dl_link;
    // seems to be similar to other GObj's in proximity
    u8 unkE;
    u8 unkF;
    u32 unk10;
    // location of render callback according to HSD
    void (*unk14)(void);
    struct GObjProcess *proc;
    u32 unk1C;
    struct GObj* nextDL;
    struct GObj* prevDL;
    u32 renderPriority;
    // location of user data removal function according to HSD
    // based on usage this might actually be the render callback
    void (*unk2C)(struct GObj *);
    u32 unk30;
    u32 unk34;
    u32 unk38;
    // user_data???
	struct UnkStruct8004A7C4_3C *unk3C;
	// goes up by 2.0f per frame until it hits 40.0f, then resets to 0.0f
	f32 unk40;
	u32 unk44;
	// grab arguments
	void (*unk48)(void);
	u32 unk4C;
} GObj;