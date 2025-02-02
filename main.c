#include "support/gcc8_c_support.h"
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
#include "utils/copper.h"
#include "utils/hard.h"
#include "utils/music.h"
#include "utils/screen.h"
#include "utils/takesystem.h"

//config
#define MUSIC

__attribute__((always_inline)) inline short MouseLeft(){return !((*(volatile UBYTE*)0xbfe001)&64);}	
__attribute__((always_inline)) inline short MouseRight(){return !((*(volatile UWORD*)0xdff016)&(1<<10));}

// DEMO - INCBIN
volatile short frameCounter = 0;
INCBIN(colors, "image.pal")
INCBIN_CHIP(image, "image.bpl") // load image into chipmem so we can use it without copying
INCBIN_CHIP(bob, "bob.bpl")


// put copperlist into chip mem so we can use it without copying
const UWORD copper2[] __attribute__((section (".MEMF_CHIP"))) = {
	                offsetof(struct Custom, color[0]), 0x0000,
	0x4101, 0xff00, offsetof(struct Custom, color[0]), 0x0111, // line 0x41
	0x4201, 0xff00, offsetof(struct Custom, color[0]), 0x0222, // line 0x42
	0x4301, 0xff00, offsetof(struct Custom, color[0]), 0x0333, // line 0x43
	0x4401, 0xff00, offsetof(struct Custom, color[0]), 0x0444, // line 0x44
	0x4501, 0xff00, offsetof(struct Custom, color[0]), 0x0555, // line 0x45
	0x4601, 0xff00, offsetof(struct Custom, color[0]), 0x0666, // line 0x46
	0x4701, 0xff00, offsetof(struct Custom, color[0]), 0x0777, // line 0x47
	0x4801, 0xff00, offsetof(struct Custom, color[0]), 0x0888, // line 0x48
	0x4901, 0xff00, offsetof(struct Custom, color[0]), 0x0999, // line 0x49
	0x4a01, 0xff00, offsetof(struct Custom, color[0]), 0x0aaa, // line 0x4a
	0x4b01, 0xff00, offsetof(struct Custom, color[0]), 0x0bbb, // line 0x4b
	0x4c01, 0xff00, offsetof(struct Custom, color[0]), 0x0ccc, // line 0x4c
	0x4d01, 0xff00, offsetof(struct Custom, color[0]), 0x0ddd, // line 0x4d
	0x4e01, 0xff00, offsetof(struct Custom, color[0]), 0x0eee, // line 0x4e
	0x4f01, 0xff00, offsetof(struct Custom, color[0]), 0x0fff, // line 0x4e
	0xffff, 0xfffe // end copper list
};

#ifdef MUSIC
	// Demo - Module Player - ThePlayer 6.1a: https://www.pouet.net/prod.php?which=19922
	// The Player® 6.1A: Copyright © 1992-95 Jarno Paananen
	// P61.testmod - Module by Skylord/Sector 7 
	INCBIN_CHIP(module, "testmod.p61")
#endif //MUSIC

UWORD* scroll = NULL;

static const UBYTE sinus15[] = { 
	8,8,9,10,10,11,12,12,
	13,13,14,14,14,15,15,15,
	15,15,15,15,14,14,14,13,
	13,12,12,11,10,10,9,8,
	8,7,6,5,5,4,3,3,
	2,2,1,1,1,0,0,0,
	0,0,0,0,1,1,1,2,
	2,3,3,4,5,5,6,7, 
};

static const UBYTE sinus40[] = {
	20,22,24,26,28,30,31,33,
	34,36,37,38,39,39,40,40,
	40,40,39,39,38,37,36,35,
	34,32,30,29,27,25,23,21,
	19,17,15,13,11,10,8,6,
	5,4,3,2,1,1,0,0,
	0,0,1,1,2,3,4,6,
	7,9,10,12,14,16,18,20,
};

static const UBYTE sinus32[] = {
	16,18,20,22,24,25,27,28,
	30,30,31,32,32,32,32,31,
	30,30,28,27,25,24,22,20,
	18,16,14,12,10,8,7,5,
	4,2,2,1,0,0,0,0,
	1,2,2,4,5,7,8,10,
	12,14,16,
};

static __attribute__((interrupt)) void interruptHandler() {
	custom->intreq=(1<<INTB_VERTB); custom->intreq=(1<<INTB_VERTB); //reset vbl req. twice for a4000 bug.

	// modify scrolling in copper list
	if(scroll) {
		int sin = sinus15[frameCounter & 63];
		*scroll = sin | (sin << 4);
	}

#ifdef MUSIC
	// DEMO - ThePlayer
	p61Music();
#endif
	// DEMO - increment frameCounter
	frameCounter++;
}

// set up a 320x256 lowres display
__attribute__((always_inline)) inline USHORT* screenScanDefault(USHORT* copListEnd) {
	const USHORT x=129;
	const USHORT width=320;
	const USHORT height=256;
	const USHORT y=44;
	const USHORT RES=8; //8=lowres,4=hires
	USHORT xstop = x+width;
	USHORT ystop = y+height;
	USHORT fw=(x>>1)-RES;

	*copListEnd++ = offsetof(struct Custom, ddfstrt);
	*copListEnd++ = fw;
	*copListEnd++ = offsetof(struct Custom, ddfstop);
	*copListEnd++ = fw+(((width>>4)-1)<<3);
	*copListEnd++ = offsetof(struct Custom, diwstrt);
	*copListEnd++ = x+(y<<8);
	*copListEnd++ = offsetof(struct Custom, diwstop);
	*copListEnd++ = (xstop-256)+((ystop-256)<<8);
	return copListEnd;
}


int main() {	
	SetScreenMode(); // This does nothing at the moment. See screen.c

	KPrintF("Hello debugger from Amiga!\n");
	Write(Output(), (APTR)"Hello console!\n", 15);

	Delay(50);

	warpmode(1);
	// TODO: precalc stuff here
#ifdef MUSIC
	if(p61Init(module) != 0)
		KPrintF("p61Init failed!\n");
#endif
	warpmode(0);

	TakeSystem();
	WaitVbl();

	char* test = (char*)AllocMem(2502, MEMF_ANY);
	memset(test, 0xcd, 2502);
	memclr(test + 2, 2502 - 4);
	FreeMem(test, 2502);

	USHORT* copper1 = (USHORT*)AllocMem(1024, MEMF_CHIP);
	USHORT* copPtr = copper1;

	// register graphics resources with WinUAE for nicer gfx debugger experience
	debug_register_bitmap(image, "image.bpl", 320, 256, 5, debug_resource_bitmap_interleaved);
	debug_register_bitmap(bob, "bob.bpl", 32, 96, 5, debug_resource_bitmap_interleaved | debug_resource_bitmap_masked);
	debug_register_palette(colors, "image.pal", 32, 0);
	debug_register_copperlist(copper1, "copper1", 1024, 0);
	debug_register_copperlist(copper2, "copper2", sizeof(copper2), 0);

	copPtr = screenScanDefault(copPtr);
	//enable bitplanes	
	*copPtr++ = offsetof(struct Custom, bplcon0);
	*copPtr++ = (0<<10)/*dual pf*/|(1<<9)/*color*/|((5)<<12)/*num bitplanes*/;
	*copPtr++ = offsetof(struct Custom, bplcon1);	//scrolling
	scroll = copPtr;
	*copPtr++ = 0;
	*copPtr++ = offsetof(struct Custom, bplcon2);	//playfied priority
	*copPtr++ = 1<<6;//0x24;			//Sprites have priority over playfields

	const USHORT lineSize=320/8;

	//set bitplane modulo
	*copPtr++=offsetof(struct Custom, bpl1mod); //odd planes   1,3,5
	*copPtr++=4*lineSize;
	*copPtr++=offsetof(struct Custom, bpl2mod); //even  planes 2,4
	*copPtr++=4*lineSize;

	// set bitplane pointers
	const UBYTE* planes[5];
	for(int a=0;a<5;a++)
		planes[a]=(UBYTE*)image + lineSize * a;
	copPtr = copSetPlanes(0, copPtr, planes, 5);

	// set colors
	for(int a=0; a < 32; a++)
		copPtr = copSetColor(copPtr, a, ((USHORT*)colors)[a]);

	// jump to copper2
	*copPtr++ = offsetof(struct Custom, copjmp2);
	*copPtr++ = 0x7fff;

	custom->cop1lc = (ULONG)copper1;
	custom->cop2lc = (ULONG)copper2;
	custom->dmacon = DMAF_BLITTER;//disable blitter dma for copjmp bug
	custom->copjmp1 = 0x7fff; //start coppper
	custom->dmacon = DMAF_SETCLR | DMAF_MASTER | DMAF_RASTER | DMAF_COPPER | DMAF_BLITTER;

	// DEMO
	SetInterruptHandler((APTR)interruptHandler);
	custom->intena = INTF_SETCLR | INTF_INTEN | INTF_VERTB;
#ifdef MUSIC
	custom->intena = INTF_SETCLR | INTF_EXTER; // ThePlayer needs INTF_EXTER
#endif

	custom->intreq=(1<<INTB_VERTB);//reset vbl req

	while(!MouseLeft()) {
		Wait10();
		int f = frameCounter & 255;

		// clear
		WaitBlit();
		custom->bltcon0 = A_TO_D | DEST;
		custom->bltcon1 = 0;
		custom->bltadat = 0;
		custom->bltdpt = (UBYTE*)image + 320 / 8 * 200 * 5;
		custom->bltdmod = 0;
		custom->bltafwm = custom->bltalwm = 0xffff;
		custom->bltsize = ((56 * 5) << HSIZEBITS) | (320/16);

		// blit
		for(short i = 0; i < 16; i++) {
			const short x = i * 16 + sinus32[(frameCounter + i) % sizeof(sinus32)] * 2;
			const short y = sinus40[((frameCounter + i) * 2) & 63] / 2;
			UBYTE* src = (UBYTE*)bob + 32 / 8 * 10 * 16 * (i % 6);

			WaitBlit();
			custom->bltcon0 = 0xca | SRCA | SRCB | SRCC | DEST | ((x & 15) << ASHIFTSHIFT); // A = source, B = mask, C = background, D = destination
			custom->bltcon1 = ((x & 15) << BSHIFTSHIFT);
			custom->bltapt = src;
			custom->bltamod = 32 / 8;
			custom->bltbpt = src + 32 / 8 * 1;
			custom->bltbmod = 32 / 8;
			custom->bltcpt = custom->bltdpt = (UBYTE*)image + 320 / 8 * 5 * (200 + y) + x / 8;
			custom->bltcmod = custom->bltdmod = (320 - 32) / 8;
			custom->bltafwm = custom->bltalwm = 0xffff;
			custom->bltsize = ((16 * 5) << HSIZEBITS) | (32/16);
		}

		// WinUAE debug overlay test
		debug_clear();
		debug_filled_rect(f + 100, 200*2, f + 400, 220*2, 0x0000ff00); // 0x00RRGGBB
		debug_rect(f + 90, 190*2, f + 400, 220*2, 0x000000ff); // 0x00RRGGBB
		debug_text(f+ 130, 209*2, "This is a WinUAE debug overlay", 0x00ff00ff);
	}

#ifdef MUSIC
	p61End();
#endif

	// END
	FreeSystem();
	CloseLibraries();
}
