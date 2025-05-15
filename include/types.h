#pragma once
typedef signed char            s8;
typedef unsigned char          u8;
typedef signed short int       s16;
typedef unsigned short int     u16;
typedef signed long            s32;
typedef unsigned long          u32;
typedef signed long long int   s64;
typedef unsigned long long int u64;

typedef float                  f32;
typedef double                 f64;

typedef float             Vec3f[3];

#define NULL (void*) 0

typedef struct  {
    f32 x;
    f32 y;
    f32 z;
} Vector;
