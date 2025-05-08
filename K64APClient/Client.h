#pragma once

#include <string>

#if defined(_MSC_VER)
//  Microsoft
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define DLLEXPORT __attribute__((visibility("default")))
#define DLLIMPORT
#else
#define DLLEXPORT
#define DLLIMPORT
#endif

typedef uint64_t gpr;

typedef union {
    double d;
    struct {
        float fl;
        float fh;
    };
    struct {
        uint32_t u32l;
        uint32_t u32h;
    };
    uint64_t u64;
} fpr;

typedef struct {
    gpr r0, r1, r2, r3, r4, r5, r6, r7,
        r8, r9, r10, r11, r12, r13, r14, r15,
        r16, r17, r18, r19, r20, r21, r22, r23,
        r24, r25, r26, r27, r28, r29, r30, r31;
    fpr f0, f1, f2, f3, f4, f5, f6, f7,
        f8, f9, f10, f11, f12, f13, f14, f15,
        f16, f17, f18, f19, f20, f21, f22, f23,
        f24, f25, f26, f27, f28, f29, f30, f31;
    uint64_t hi, lo;
    uint32_t* f_odd;
    uint32_t status_reg;
    uint8_t mips3_float_mode;
} recomp_context;

#define TO_PTR(type, var) ((type*)(&rdram[(uint64_t)var - 0xFFFFFFFF80000000]))
#define MEM_B(offset, reg) (*(int8_t*)(rdram + ((((reg) + (offset)) ^ 3) - 0xFFFFFFFF80000000)))
#define MEM_W(offset, reg) (*(int32_t*)(rdram + ((((reg) + (offset))) - 0xFFFFFFFF80000000)))

#define PTR(x) uint32_t

void getStr(uint8_t* rdram, PTR(char) ptr, std::string& outString) {
    char c = MEM_B(0, (gpr)ptr);
    uint32_t i = 0;
    while (c != 0) {
        outString += c;
        i += 1;
        c = MEM_B(i, (gpr)ptr);
    }
}

template<int index, typename T>
T _arg(uint8_t* rdram, recomp_context* ctx) {
    static_assert(index < 4, "Only args 0 through 3 supported");
    gpr raw_arg = (&ctx->r4)[index];
    if constexpr (std::is_same_v<T, float>) {
        if constexpr (index < 2) {
            static_assert(index != 1, "Floats in arg 1 not supported");
            return ctx->f12.fl;
        }
        else {
            // static_assert in else workaround
            [] <bool flag = false>() {
                static_assert(flag, "Floats in a2/a3 not supported");
            }();
        }
    }
    else if constexpr (std::is_pointer_v<T>) {
        static_assert (!std::is_pointer_v<std::remove_pointer_t<T>>, "Double pointers not supported");
        return TO_PTR(std::remove_pointer_t<T>, raw_arg);
    }
    else if constexpr (std::is_integral_v<T>) {
        static_assert(sizeof(T) <= 4, "64-bit args not supported");
        return static_cast<T>(raw_arg);
    }
    else {
        // static_assert in else workaround
        [] <bool flag = false>() {
            static_assert(flag, "Unsupported type");
        }();
    }
};

template <typename T>
void _return(recomp_context* ctx, T val) {
    static_assert(sizeof(T) <= 4, "Only 32-bit value returns supported currently");
    if constexpr (std::is_same_v<T, float>) {
        ctx->f0.fl = val;
    }
    else if constexpr (std::is_integral_v<T> && sizeof(T) <= 4) {
        ctx->r2 = int32_t(val);
    }
    else {
        // static_assert in else workaround
        [] <bool flag = false>() {
            static_assert(flag, "Unsupported type");
        }();
    }
};
DLLEXPORT void InitializeArchipelago(uint8_t* rdram, recomp_context* ctx);