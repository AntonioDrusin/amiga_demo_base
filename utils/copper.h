#pragma once
#include "../support/gcc8_c_support.h"
#include <exec/types.h>
#include "hard.h"


__attribute__((always_inline)) inline UWORD* copSetPlanes(UBYTE bplPtrStart,UWORD* copListEnd,const UBYTE **planes,int numPlanes) {
    for (UWORD i=0;i<numPlanes;i++) {
        ULONG addr=(ULONG)planes[i];
        *copListEnd++=offsetof(struct Custom, bplpt[0]) + (i + bplPtrStart) * sizeof(APTR);
        *copListEnd++=(UWORD)(addr>>16);
        *copListEnd++=offsetof(struct Custom, bplpt[0]) + (i + bplPtrStart) * sizeof(APTR) + 2;
        *copListEnd++=(UWORD)addr;
    }
    return copListEnd;
}

__attribute__((always_inline)) inline UWORD* copWaitXY(UWORD *copListEnd,UWORD x,UWORD i) {
    *copListEnd++=(i<<8)|(x<<1)|1;	//bit 1 means wait. waits for vertical position x<<8, first raster stop position outside the left 
    *copListEnd++=0xfffe;
    return copListEnd;
}

__attribute__((always_inline)) inline UWORD* copWaitY(UWORD* copListEnd,UWORD i) {
    *copListEnd++=(i<<8)|6|1;	//bit 1 means wait. waits for vertical position x<<8, first raster stop position outside the left 
    *copListEnd++=0xfffe;
    return copListEnd;
}

extern BOOL copper_wrap_flag;

__attribute__((always_inline)) inline UWORD* copSafeWaitY(UWORD* copListEnd,UWORD y) {
    if ( y <= (UWORD)255 ) copper_wrap_flag = FALSE;
    if ( y > (UWORD)255 && FALSE == copper_wrap_flag) {
        copListEnd = copWaitXY(copListEnd, (UWORD)0x70, (UWORD)0xff);
        copper_wrap_flag = TRUE;
    }
    copListEnd = copWaitY(copListEnd, y);
    return copListEnd;
}

__attribute__((always_inline)) inline UWORD* copSafeWaitXY(UWORD *copListEnd,UWORD x,UWORD y) {
    if ( y <= (UWORD)255 ) copper_wrap_flag = FALSE;
    if ( y > (UWORD)255 && FALSE == copper_wrap_flag) {
        copListEnd = copWaitXY(copListEnd, (UWORD)0x70, (UWORD)0xff);
        copper_wrap_flag = TRUE;
    }
    copListEnd = copWaitXY(copListEnd,x,y);    
    return copListEnd;
}

__attribute__((always_inline)) inline UWORD* copSkipY(UWORD* copListEnd,UWORD i) {
    *copListEnd++=((i<<8)|6)|1;	//bit 0 means skip. waits for vertical position x<<8, first raster stop position outside the left 
    *copListEnd++=0xffff;
    return copListEnd;
}

__attribute__((always_inline)) inline UWORD* copSetColor(UWORD* copListCurrent,UWORD index,UWORD color) {    
    *copListCurrent++=offsetof(struct Custom, color) + sizeof(UWORD) * index;
    *copListCurrent++=color;
    return copListCurrent;
}

__attribute__((always_inline)) inline UWORD* copSetSprite(UWORD* copListCurrent, UWORD spriteIndex, APTR spriteData) {    
    UWORD r = offsetof(struct Custom, sprpt[spriteIndex]);
    ULONG spritePtr = (ULONG)spriteData;
    *copListCurrent++=r;
    *copListCurrent++=(UWORD)(spritePtr>>16);
    *copListCurrent++=r+2;
    *copListCurrent++=(UWORD)(spritePtr);
    return copListCurrent;
}