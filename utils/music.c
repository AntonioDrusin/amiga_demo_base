#include "../support/gcc8_c_support.h"
#include <exec/types.h>

extern void playRtn();

int p61Init(const void* module) { // returns 0 if success, non-zero otherwise
    register volatile const void* _a0 ASM("a0") = module;
    register volatile const void* _a1 ASM("a1") = NULL;
    register volatile const void* _a2 ASM("a2") = NULL;
    register volatile const void* _a3 ASM("a3") = playRtn;
    register                int   _d0 ASM("d0"); // return value
    __asm volatile (
        "movem.l %%d1-%%d7/%%a4-%%a6,-(%%sp)\n"
        "jsr 0(%%a3)\n"
        "movem.l (%%sp)+,%%d1-%%d7/%%a4-%%a6"
    : "=r" (_d0), "+rf"(_a0), "+rf"(_a1), "+rf"(_a2), "+rf"(_a3)
    :
    : "cc", "memory");
    return _d0;
}

void p61Music() {    
    register volatile const void* _a6 ASM("a6") = (void*)0xdff000;
    // Can't clobber a5 due to compiler error, so saving manually
    __asm volatile (
        "move.l %%a5,-(%%sp)\n"
        "jsr (playRtn + 4)\n"
        "move.l (%%sp)+,%%a5"
    : "+rf"(_a6)
    :
    : "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "a0", "a1", "a2", "a3", "a4", "cc", "memory");
}

void p61Restart() {
    register volatile const void* _a3 ASM("a3") = playRtn;
    register volatile const void* _a6 ASM("a6") = (void*)0xdff000;
    __asm volatile (
        "movem.l %%d0-%%d7/%%a0-%%a2/%%a4-%%a5,-(%%sp)\n"
        "moveq.l #26,%%d0\n"
        "jsr 16(%%a3)\n"
        "movem.l (%%sp)+,%%d0-%%d7/%%a0-%%a2/%%a4-%%a5"
    : "+rf"(_a3), "+rf"(_a6)
    :
    : "cc", "memory");
}

void p61End() {
    register volatile const void* _a3 ASM("a3") = playRtn;
    register volatile const void* _a6 ASM("a6") = (void*)0xdff000;
    __asm volatile (
        "movem.l %%d0-%%d1/%%a0-%%a1,-(%%sp)\n"
        "jsr 8(%%a3)\n"
        "movem.l (%%sp)+,%%d0-%%d1/%%a0-%%a1"
    : "+rf"(_a3), "+rf"(_a6)
    :
    : "cc", "memory");
}
