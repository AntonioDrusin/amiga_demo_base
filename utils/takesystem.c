#include "../support/gcc8_c_support.h"
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <exec/execbase.h>
#include <graphics/gfxmacros.h>
#include <hardware/custom.h>
#include <hardware/dmabits.h>
#include <hardware/intbits.h>
#include "hard.h"
#include "screen.h"

struct ExecBase *SysBase;
struct DosLibrary *DOSBase;
struct GfxBase *GfxBase;

//backup
static UWORD SystemInts;
static UWORD SystemDMA;
static UWORD SystemADKCON;
static volatile APTR VBR=0;
APTR SystemIrq;
 
struct View *ActiView;

static APTR GetVBR(void) {
    APTR vbr = 0;
    UWORD getvbr[] = { 0x4e7a, 0x0801, 0x4e73 }; // MOVEC.L VBR,D0 RTE

    if (SysBase->AttnFlags & AFF_68010) 
        vbr = (APTR)Supervisor((ULONG (*)())getvbr);

    return vbr;
}

void SetInterruptHandler(APTR interrupt) {
    *(volatile APTR*)(((UBYTE*)VBR)+0x6c) = interrupt;
}

APTR GetInterruptHandler() {
    return *(volatile APTR*)(((UBYTE*)VBR)+0x6c);
}

void OpenLibraries() {
	SysBase = *((struct ExecBase**)4UL);
	custom = (struct Custom*)0xdff000;

	// We will use the graphics library only to locate and restore the system copper list once we are through.
	GfxBase = (struct GfxBase *)OpenLibrary((CONST_STRPTR)"graphics.library",0);
	if (!GfxBase) 
		Exit(0);

	// used for printing
	DOSBase = (struct DosLibrary*)OpenLibrary((CONST_STRPTR)"dos.library", 0);
	if (!DOSBase)
        Exit(0);    
}

void CloseLibraries() {
    CloseLibrary((struct Library*)DOSBase);
	CloseLibrary((struct Library*)GfxBase);
}

void TakeSystem() {
    Forbid();
    //Save current interrupts and DMA settings so we can restore them upon exit. 
    SystemADKCON=custom->adkconr;
    SystemInts=custom->intenar;
    SystemDMA=custom->dmaconr;
    ActiView=GfxBase->ActiView; //store current view

    LoadView(0);
    WaitTOF();
    WaitTOF();

    WaitVbl();
    WaitVbl();

    OwnBlitter();
    WaitBlit();	
    Disable();
    
    custom->intena=0x7fff;//disable all interrupts
    custom->intreq=0x7fff;//Clear any interrupts that were pending
    
    custom->dmacon=0x7fff;//Clear all DMA channels

    //set all colors black
    for(int a=0;a<32;a++)
        custom->color[a]=0;

    WaitVbl();
    WaitVbl();

    VBR=GetVBR();
    SystemIrq=GetInterruptHandler(); //store interrupt register
}

void FreeSystem() { 
    WaitVbl();
    WaitBlit();
    custom->intena=0x7fff;//disable all interrupts
    custom->intreq=0x7fff;//Clear any interrupts that were pending
    custom->dmacon=0x7fff;//Clear all DMA channels

    //restore interrupts
    SetInterruptHandler(SystemIrq);

    /*Restore system copper list(s). */
    custom->cop1lc=(ULONG)GfxBase->copinit;
    custom->cop2lc=(ULONG)GfxBase->LOFlist;
    custom->copjmp1=0x7fff; //start coppper

    /*Restore all interrupts and DMA settings. */
    custom->intena=SystemInts|0x8000;
    custom->dmacon=SystemDMA|0x8000;
    custom->adkcon=SystemADKCON|0x8000;

    WaitBlit();	
    DisownBlitter();
    Enable();

    LoadView(ActiView);
    WaitTOF();
    WaitTOF();

    Permit();
}

static UWORD demoIntena;
static UWORD demoDmacon;
static APTR vblankHandler;

void* doynaxdepack(const void* input, void* output) { // returns end of output data, input needs to be 16-bit aligned!
    register volatile const void* _a0 ASM("a0") = input;
    register volatile       void* _a1 ASM("a1") = output;
    __asm volatile (
        "movem.l %%d0-%%d7/%%a2-%%a6,-(%%sp)\n"
        "jsr _doynaxdepack_vasm\n"
        "movem.l (%%sp)+,%%d0-%%d7/%%a2-%%a6"
    : "+rf"(_a0), "+rf"(_a1)
    :
    : "cc", "memory");
    return (void*)_a1;
}
