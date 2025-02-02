#pragma once
#include "../support/gcc8_c_support.h"
#include <exec/types.h>

struct VerticalPositions {
    ULONG vblank;
};

extern void SetScreenMode();
extern const struct VerticalPositions * const verticalPositions;

//vblank begins at vpos 312 hpos 1 and ends at vpos 25 hpos 1
//vsync begins at line 2 hpos 132 and ends at vpos 5 hpos 18 
__attribute__((always_inline)) inline void WaitVbl() {
	debug_start_idle();
	while (1) {
		volatile ULONG vpos=*(volatile ULONG*)0xDFF004;
		vpos&=0x1ff00;
		if (vpos!=(verticalPositions->vblank<<8))
			break;
	}
	while (1) {
		volatile ULONG vpos=*(volatile ULONG*)0xDFF004;
		vpos&=0x1ff00;
		if (vpos==(verticalPositions->vblank<<8))
			break;
	}
	debug_stop_idle();
}


__attribute__((always_inline)) inline void WaitLine(UWORD line) {
    while (1) {
        volatile ULONG vpos=*(volatile ULONG*)0xDFF004;
        if(((vpos >> 8) & 511) == line)
            break;
    }
}

__attribute__((always_inline)) inline void Wait10() { WaitLine(0x10); }
