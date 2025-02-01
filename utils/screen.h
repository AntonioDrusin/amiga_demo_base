#pragma once
#include <exec/types.h>

struct VerticalPositions {
    ULONG vblank;
};

extern void SetScreenMode();
extern const struct VerticalPositions * const verticalPositions;