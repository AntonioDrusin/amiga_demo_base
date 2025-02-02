#include <exec/types.h>

#pragma once
extern void OpenLibraries();
extern void CloseLibraries();
extern void TakeSystem();
extern void FreeSystem();
extern void SetInterruptHandler(APTR interrupt);

extern APTR SystemIrq;