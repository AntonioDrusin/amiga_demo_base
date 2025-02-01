#include "screen.h"

static const struct VerticalPositions PALPositions = {311};
static const struct VerticalPositions NTSCPositions = {262};

// As long as everything is const, the compiler will just "burn" in the value as if it was #defined.
// If you want to have the demo autodetect then make the pointer not const and change it. 
// That will cost in runtime though.
const struct VerticalPositions * const verticalPositions = &PALPositions;

void SetScreenMode() { return;}