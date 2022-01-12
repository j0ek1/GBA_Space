#include <gba_interrupt.h>		// for interrupt handling
#include <gba_systemcalls.h>	// for VBlankIntrWait()
#include <stdlib.h>     // for rand()
#include <time.h>       // for time()


typedef unsigned int uint32;
typedef unsigned short uint16;

typedef struct Collision // structure to hold positional variables for calculating collisions
{
	uint16 x11;
	uint16 y11;
	uint16 x21;
	uint16 y21;
	uint16 x31;
	uint16 y31;
	uint16 x41;
	uint16 y41;
	uint16 x51;
	uint16 y51;
	uint16 x61;
	uint16 y61;
	uint16 x12;
	uint16 y12;
	uint16 x22;
	uint16 y22;
	uint16 x32;
	uint16 y32;
	uint16 x42;
	uint16 y42;
	uint16 x52;
	uint16 y52;
	uint16 x62;
	uint16 y62;

} Collision;

extern void collFunction(void* c);

// frequency of notes used
enum Notes { note_a = 1750, note_asharp = 1486, note_b = 1517, note_c = 1574, note_d = 1602, note_dh = 1825, note_f = 1673, note_g = 1714, note_gsharp = 1732};

// order of notes in the song
uint16 song[64] = {
	note_d, note_d, note_dh, 0, note_a, 0, 0, note_gsharp, 0, note_g, 0, note_f, 0, note_d, note_f, note_g,
	note_c, note_c, note_dh, 0, note_a, 0, 0, note_gsharp, 0, note_g, 0, note_f, 0, note_d, note_f, note_g,
	note_b, note_b, note_dh, 0, note_a, 0, 0, note_gsharp, 0, note_g, 0, note_f, 0, note_d, note_f, note_g,
	note_asharp, note_asharp, note_dh, 0, note_a, 0, 0, note_gsharp, 0, note_g, 0, note_f, 0, note_d, note_f, note_g
};

// define input keys
#define BUTTON_A	(1 << 0)
#define BUTTON_B	(1 << 1)
#define RIGHT	(1 << 4)
#define LEFT	(1 << 5)
#define UP	(1 << 6)
#define DOWN	(1 << 7)

int main(void) {

	// required to enable vBlank interrupts
	irqInit();
	irqEnable(IRQ_VBLANK);

	uint32 frame = 0;
	bool gameOver = false;

	// pointer to the memory that controls the display options
	uint16* DISPLAYCONTROL = (uint16*)0x4000000;
	DISPLAYCONTROL[0] = ((1 << 8) | (1 << 9) | (1 << 10) | (1 << 12)); // turn BG layer 0-2 and obj on

	// pointer to BG palette info
	uint16* BGPALETTE = (uint16*)0x5000000;
	BGPALETTE[0] = ((0 << 0) | (0 << 5) | (0 << 10));	// RGB, values 0-31: palette 0, colour 0 is the BG colour
	BGPALETTE[(1 * 16) + 1] = ((31 << 0) | (31 << 5) | (31 << 10)); // white
	BGPALETTE[(2 * 16) + 1] = ((15 << 0) | (15 << 5) | (15 << 10)); // white/grey

	BGPALETTE[(3 * 16) + 1] = ((0 << 0) | (31 << 5) | (31 << 10)); // light blue

	uint32* BGTILES = (uint32*)0x6000000;
	BGTILES[(1 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28)); // star pattern 1
	BGTILES[(1 * 8) + 1] = ((0 << 0) | (1 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(1 * 8) + 2] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(1 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(1 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(1 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(1 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(1 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(2 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28)); // star pattern 2
	BGTILES[(2 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(2 * 8) + 2] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(2 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(2 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(2 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(2 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(2 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(3 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28)); // star pattern 3
	BGTILES[(3 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(3 * 8) + 2] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(3 * 8) + 3] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(3 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(3 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(3 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(3 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(4 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28)); // star pattern 4
	BGTILES[(4 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(4 * 8) + 2] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(4 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(4 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(4 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(4 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(4 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(9 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28)); // number 0
	BGTILES[(9 * 8) + 1] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(9 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(9 * 8) + 3] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(9 * 8) + 4] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(9 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(9 * 8) + 6] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(9 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(10 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28)); // number 1
	BGTILES[(10 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(10 * 8) + 2] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(10 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(10 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(10 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(10 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(10 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(11 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28)); // number 2
	BGTILES[(11 * 8) + 1] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(11 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(11 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(11 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(11 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(11 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(11 * 8) + 7] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (1 << 24) | (0 << 28));

	BGTILES[(12 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28)); // number 3
	BGTILES[(12 * 8) + 1] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(12 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(12 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(12 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(12 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(12 * 8) + 6] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(12 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(13 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28)); // number 4
	BGTILES[(13 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(13 * 8) + 2] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(13 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(13 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(13 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(13 * 8) + 6] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(13 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(14 * 8) + 0] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (1 << 24) | (0 << 28)); // number 5
	BGTILES[(14 * 8) + 1] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(14 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(14 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(14 * 8) + 4] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(14 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(14 * 8) + 6] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(14 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(15 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28)); // number 6
	BGTILES[(15 * 8) + 1] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(15 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(15 * 8) + 3] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(15 * 8) + 4] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(15 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(15 * 8) + 6] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(15 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(16 * 8) + 0] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (1 << 24) | (0 << 28)); // number 7
	BGTILES[(16 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(16 * 8) + 2] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(16 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(16 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(16 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(16 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(16 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(17 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28)); // number 8
	BGTILES[(17 * 8) + 1] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(17 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(17 * 8) + 3] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	BGTILES[(17 * 8) + 4] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(17 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(17 * 8) + 6] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(17 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	BGTILES[(18 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28)); // number 9
	BGTILES[(18 * 8) + 1] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(18 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(18 * 8) + 3] = ((0 << 0) | (0 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(18 * 8) + 4] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(18 * 8) + 5] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(18 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (0 << 28));
	BGTILES[(18 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));

	
	uint16* MAPMEM = (uint16*)0x6004000;
	//MAPMEM[0] = ((1 << 0) | (1 << 12)); // tile num | pal num

	uint16* BG0 = (uint16*)0x4000008; // BG layer 0 settings
	BG0[0] = ((0 << 0) | (0 << 2) | (8 << 8)); // priority | character | screen

	uint16* BG1 = (uint16*)0x400000A; // BG layer 1 settings
	BG1[0] = ((1 << 0) | (0 << 2) | (9 << 8)); // priority | character | screen

	uint16* BG2 = (uint16*)0x400000C; // BG layer 2 settings
	BG2[0] = ((2 << 0) | (0 << 2) | (10 << 8)); // priority | character | screen

	uint16 sPattern = 1;
	uint16 xStar = 0;
	uint16 xStar1 = 0;
	uint16 yStar = 0;
	uint16 yStar1 = 0;

	uint32 score = 0;
	uint16 scoreTimer = 0;
	uint16 digit5;
	uint16 digit4;
	uint16 digit3;
	uint16 digit2;
	uint16 digit1;

	// display initial score
	MAPMEM[0] = ((9 << 0) | (3 << 12));
	MAPMEM[1] = ((9 << 0) | (3 << 12));
	MAPMEM[2] = ((9 << 0) | (3 << 12));
	MAPMEM[3] = ((9 << 0) | (3 << 12));
	MAPMEM[4] = ((9 << 0) | (3 << 12));

	// display bg1 stars
	for (yStar = 0; yStar < 20; yStar++) // collumn 0 to 20
	{
		
		for (xStar = 0; xStar < 32; xStar++) // row 0 to 30
		{
			sPattern = rand() % 4 + 1; // get random star pattern
			MAPMEM[1024 + ((yStar * 32) + xStar)] = ((sPattern << 0) | (1 << 12));
		}
	}

	// display bg2 stars
	for (yStar1 = 0; yStar1 < 20; yStar1++) // collumn 0 to 20
	{

		for (xStar1 = 0; xStar1 < 32; xStar1++) // row 0 to 30
		{
			sPattern = rand() % 4 + 1; // get random star pattern
			MAPMEM[2048 + ((yStar1 * 32) + xStar1)] = ((sPattern << 0) | (2 << 12));
		}
	}

	// sound controls
	uint16* SOUND_MASTER = (uint16*)0x4000084;
	SOUND_MASTER[0] = (1 << 7);
	uint16* SOUND_MIX = (uint16*)0x4000082;
	SOUND_MIX[0] = (2 << 0);
	uint16* SOUND_VOLUMES = (uint16*)0x4000080;
	SOUND_VOLUMES[0] = ((4 << 0) | (4 << 4) | (1 << 9) | (1 << 13));
	uint16* SOUND2_SETTINGS = (uint16*)0x4000068;
	SOUND2_SETTINGS[0] = ((0 << 0) | (2 << 6) | (0 << 8) | (4 << 12));
	uint16* SOUND2_FREQ = (uint16*)0x400006C;

	uint16 songSpeed = 8;
	uint16 currentNote = 0;
	uint16 currentFrame = 0;

	uint16* OBJPALETTE = (uint16*)0x5000200;
	// rocket palette
	OBJPALETTE[(1 * 16) + 1] = ((24 << 0) | (4 << 5) | (4 << 10)); // dark red
	OBJPALETTE[(1 * 16) + 2] = ((31 << 0) | (31 << 5) | (31 << 10)); // white
	OBJPALETTE[(1 * 16) + 3] = ((6 << 0) | (6 << 5) | (6 << 10)); // dark grey
	OBJPALETTE[(1 * 16) + 4] = ((31 << 0) | (10 << 5) | (4 << 10)); // orange
	OBJPALETTE[(1 * 16) + 5] = ((31 << 0) | (18 << 5) | (0 << 10)); // yellow/orange

	// meteor palette
	OBJPALETTE[(2 * 16) + 1] = ((12 << 0) | (7 << 5) | (4 << 10)); // dark brown
	OBJPALETTE[(2 * 16) + 2] = ((15 << 0) | (9 << 5) | (5 << 10)); // light brown
	OBJPALETTE[(2 * 16) + 3] = ((10 << 0) | (10 << 5) | (10 << 10)); // grey

	uint32* OBJTILES = (uint32*)0x6010000;
	OBJTILES[(1 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (3 << 20) | (3 << 24) | (3 << 28)); // left side of rocket
	OBJTILES[(1 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (1 << 20) | (3 << 24) | (3 << 28));
	OBJTILES[(1 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (4 << 16) | (3 << 20) | (3 << 24) | (2 << 28));
	OBJTILES[(1 * 8) + 3] = ((1 << 0) | (4 << 4) | (5 << 8) | (5 << 12) | (3 << 16) | (3 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(1 * 8) + 4] = ((1 << 0) | (4 << 4) | (5 << 8) | (5 << 12) | (3 << 16) | (3 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(1 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (4 << 16) | (3 << 20) | (3 << 24) | (2 << 28));
	OBJTILES[(1 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (1 << 16) | (1 << 20) | (3 << 24) | (3 << 28));
	OBJTILES[(1 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (3 << 20) | (3 << 24) | (3 << 28));

	OBJTILES[(2 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28)); // right side of rocket
	OBJTILES[(2 * 8) + 1] = ((3 << 0) | (3 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	OBJTILES[(2 * 8) + 2] = ((2 << 0) | (3 << 4) | (3 << 8) | (3 << 12) | (3 << 16) | (3 << 20) | (3 << 24) | (0 << 28));
	OBJTILES[(2 * 8) + 3] = ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (3 << 28));
	OBJTILES[(2 * 8) + 4] = ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (3 << 28));
	OBJTILES[(2 * 8) + 5] = ((2 << 0) | (3 << 4) | (3 << 8) | (3 << 12) | (3 << 16) | (3 << 20) | (3 << 24) | (0 << 28));
	OBJTILES[(2 * 8) + 6] = ((3 << 0) | (3 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	OBJTILES[(2 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));

	OBJTILES[(4 * 8) + 0] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (1 << 28)); // top left of meteor
	OBJTILES[(4 * 8) + 1] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (1 << 24) | (2 << 28));
	OBJTILES[(4 * 8) + 2] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(4 * 8) + 3] = ((0 << 0) | (1 << 4) | (1 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(4 * 8) + 4] = ((0 << 0) | (1 << 4) | (2 << 8) | (2 << 12) | (1 << 16) | (3 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(4 * 8) + 5] = ((0 << 0) | (1 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (1 << 24) | (2 << 28));
	OBJTILES[(4 * 8) + 6] = ((1 << 0) | (1 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(4 * 8) + 7] = ((1 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (2 << 28));

	OBJTILES[(5 * 8) + 0] = ((1 << 0) | (1 << 4) | (1 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28)); // top right of meteor
	OBJTILES[(5 * 8) + 1] = ((2 << 0) | (2 << 4) | (1 << 8) | (1 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	OBJTILES[(5 * 8) + 2] = ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	OBJTILES[(5 * 8) + 3] = ((2 << 0) | (2 << 4) | (1 << 8) | (2 << 12) | (2 << 16) | (1 << 20) | (1 << 24) | (0 << 28));
	OBJTILES[(5 * 8) + 4] = ((3 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (1 << 24) | (1 << 28));
	OBJTILES[(5 * 8) + 5] = ((2 << 0) | (2 << 4) | (2 << 8) | (3 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (1 << 28));
	OBJTILES[(5 * 8) + 6] = ((1 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (1 << 20) | (2 << 24) | (1 << 28));
	OBJTILES[(5 * 8) + 7] = ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (1 << 28));

	OBJTILES[(36 * 8) + 0] = ((1 << 0) | (2 << 4) | (3 << 8) | (2 << 12) | (1 << 16) | (3 << 20) | (2 << 24) | (2 << 28)); // bottom left of meteor
	OBJTILES[(36 * 8) + 1] = ((1 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(36 * 8) + 2] = ((1 << 0) | (1 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(36 * 8) + 3] = ((0 << 0) | (1 << 4) | (2 << 8) | (1 << 12) | (2 << 16) | (2 << 20) | (1 << 24) | (2 << 28));
	OBJTILES[(36 * 8) + 4] = ((0 << 0) | (1 << 4) | (2 << 8) | (2 << 12) | (3 << 16) | (2 << 20) | (2 << 24) | (2 << 28));
	OBJTILES[(36 * 8) + 5] = ((0 << 0) | (0 << 4) | (1 << 8) | (1 << 12) | (2 << 16) | (2 << 20) | (2 << 24) | (3 << 28));
	OBJTILES[(36 * 8) + 6] = ((0 << 0) | (0 << 4) | (0 << 8) | (1 << 12) | (1 << 16) | (1 << 20) | (1 << 24) | (2 << 28));
	OBJTILES[(36 * 8) + 7] = ((0 << 0) | (0 << 4) | (0 << 8) | (0 << 12) | (0 << 16) | (0 << 20) | (1 << 24) | (1 << 28));

	OBJTILES[(37 * 8) + 0] = ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (3 << 16) | (2 << 20) | (1 << 24) | (1 << 28)); // bottom right of meteor
	OBJTILES[(37 * 8) + 1] = ((1 << 0) | (2 << 4) | (1 << 8) | (2 << 12) | (2 << 16) | (2 << 20) | (1 << 24) | (0 << 28));
	OBJTILES[(37 * 8) + 2] = ((2 << 0) | (2 << 4) | (2 << 8) | (3 << 12) | (2 << 16) | (2 << 20) | (1 << 24) | (0 << 28));
	OBJTILES[(37 * 8) + 3] = ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (1 << 20) | (1 << 24) | (0 << 28));
	OBJTILES[(37 * 8) + 4] = ((2 << 0) | (2 << 4) | (2 << 8) | (2 << 12) | (2 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	OBJTILES[(37 * 8) + 5] = ((2 << 0) | (2 << 4) | (1 << 8) | (2 << 12) | (1 << 16) | (1 << 20) | (0 << 24) | (0 << 28));
	OBJTILES[(37 * 8) + 6] = ((2 << 0) | (2 << 4) | (2 << 8) | (1 << 12) | (1 << 16) | (0 << 20) | (0 << 24) | (0 << 28));
	OBJTILES[(37 * 8) + 7] = ((1 << 0) | (1 << 4) | (1 << 8) | (1 << 12) | (0 << 16) | (0 << 20) | (0 << 24) | (0 << 28));

	// positional values and timers
	short xPos = 50;
	short yPos = 80;
	short meteorX1 = 240;
	short meteorX2 = 240;
	short meteorX3 = 240;
	short meteorX4 = 240;
	short meteorX5 = 240;
	short meteorX6 = 240;
	short meteorY1 = 16;
	short meteorY2 = 32;
	short meteorY3 = 48;
	short meteorY4 = 64;
	short meteorY5 = 80;
	short meteorY6 = 96;
	uint16 timer1 = 0;
	uint16 timer2 = 0;
	uint16 timer3 = 0;
	uint16 timer4 = 0;
	uint16 timer5 = 0;
	uint16 timer6 = 0;

	uint16* OAM = (uint16*)0x7000000;
	// rocket
	OAM[(0 * 4) + 0] = ((yPos << 0) | (1 << 14)); // y | OBJ shape
	OAM[(0 * 4) + 1] = ((xPos << 0) | (0 << 14)); // x | OBJ size
	OAM[(0 * 4) + 2] = ((1 << 0) | (1 << 12)); // tile num | palette num

	// meteor 1
	OAM[(1 * 4) + 0] = ((meteorY1 << 0) | (0 << 14)); // y | OBJ shape
	OAM[(1 * 4) + 1] = ((meteorX1 << 0) | (1 << 14)); // x | OBJ size
	OAM[(1 * 4) + 2] = ((4 << 0) | (2 << 12)); // tile num | palette num
	// meteor 2
	OAM[(2 * 4) + 0] = ((meteorY2 << 0) | (0 << 14)); // y | OBJ shape
	OAM[(2 * 4) + 1] = ((meteorX2 << 0) | (1 << 14)); // x | OBJ size
	OAM[(2 * 4) + 2] = ((4 << 0) | (2 << 12)); // tile num | palette num
	// meteor 3
	OAM[(3 * 4) + 0] = ((meteorY3 << 0) | (0 << 14)); // y | OBJ shape
	OAM[(3 * 4) + 1] = ((meteorX3 << 0) | (1 << 14)); // x | OBJ size
	OAM[(3 * 4) + 2] = ((4 << 0) | (2 << 12)); // tile num | palette num
	// meteor 4
	OAM[(4 * 4) + 0] = ((meteorY4 << 0) | (0 << 14)); // y | OBJ shape
	OAM[(4 * 4) + 1] = ((meteorX4 << 0) | (1 << 14)); // x | OBJ size
	OAM[(4 * 4) + 2] = ((4 << 0) | (2 << 12)); // tile num | palette num
	// meteor 5
	OAM[(5 * 4) + 0] = ((meteorY5 << 0) | (0 << 14)); // y | OBJ shape
	OAM[(5 * 4) + 1] = ((meteorX5 << 0) | (1 << 14)); // x | OBJ size
	OAM[(5 * 4) + 2] = ((4 << 0) | (2 << 12)); // tile num | palette num
	// meteor 6
	OAM[(6 * 4) + 0] = ((meteorY6 << 0) | (0 << 14)); // y | OBJ shape
	OAM[(6 * 4) + 1] = ((meteorX6 << 0) | (1 << 14)); // x | OBJ size
	OAM[(6 * 4) + 2] = ((4 << 0) | (2 << 12)); // tile num | palette num

	uint16* BG1XSCROLL = (uint16*)0x4000014;
	uint16 xScroll0 = 0;

	uint16* BG2XSCROLL = (uint16*)0x4000018;
	uint16 xScroll1 = 0;
	bool shouldScroll = true;

	Collision coll;
	
	volatile uint16* INPUT = (volatile uint16*)0x4000130; // keypad input memory

	// GBA docs are here:	https://mgba-emu.github.io/gbatek/

	while (1)
	{					
		frame++;
		scoreTimer++;
		timer1++;
		timer2++;
		timer3++;
		timer4++;
		timer5++;
		timer6++;

		// music loop
		if (!gameOver)
		{
			if (currentFrame == 0)
			{
				// cycle through notes of the song
				uint16 theNote = song[currentNote];
				if (theNote > 0)
				{
					SOUND2_FREQ[0] = ((theNote << 0) | (1 << 14) | (1 << 15));
				}
				currentNote++;
				currentNote %= 64;
			}
			// controls the speed of the song
			currentFrame++;
			currentFrame %= songSpeed;
		}
		
		if (scoreTimer > 59)
		{
			score++;
			// divide score into 5 digits for display
			digit5 = score / 10000 % 10;
			digit4 = score / 1000 % 10;
			digit3 = score / 100 % 10;
			digit2 = score / 10 % 10;
			digit1 = score % 10;

			// display score
			if (!gameOver)
			{
				MAPMEM[0] = (((digit5 + 9) << 0) | (3 << 12));
				MAPMEM[1] = (((digit4 + 9) << 0) | (3 << 12));
				MAPMEM[2] = (((digit3 + 9) << 0) | (3 << 12));
				MAPMEM[3] = (((digit2 + 9) << 0) | (3 << 12));
				MAPMEM[4] = (((digit1 + 9) << 0) | (3 << 12));
			}		
			scoreTimer = 0;
		}

		if (frame > 1 && !gameOver) // handles background scrolling
		{
			BG1XSCROLL[0] = xScroll0;
			xScroll0++;
			if (xScroll0 > 255)
			{
				xScroll0 = 0;
			}
			BG2XSCROLL[0] = xScroll1;
			if (shouldScroll) // bg1 scrolls every other frame for parallax effect
			{
				xScroll1++;
			}
			shouldScroll = !shouldScroll;
			if (xScroll1 > 255)
			{
				xScroll1 = 0;
			}
		}

		if (!gameOver)
		{
			short randomY = 1;
			short prevY = 1;

			// meteor1 y
			if (timer1 > 119) // meteor crosses screen every 120 frames
			{
				prevY = randomY;
				do
				{
					randomY = rand() % 9 + 1; // new y value is randomly selected every time meteor crosses screen
				} while (randomY == prevY);
				meteorY1 = randomY * 16;
				OAM[(1 * 4) + 0] = ((meteorY1 << 0) | (0 << 14)); // update y position
				timer1 = 0; // reset timer
			}
			// meteor1 x
			meteorX1 = meteorX1 - 2; // meteor constantly moving from right to left
			if (meteorX1 < 1) // once meteor reaches end of screen on left
			{
				meteorX1 = 240; // reset position to the right
			}
			OAM[(1 * 4) + 1] = ((meteorX1 << 0) | (1 << 14)); // update x position

			// meteor2 y
			if (timer2 > 139)
			{
				prevY = randomY;
				do
				{
					randomY = rand() % 9 + 1; // new y value is randomly selected every time meteor crosses screen
				} while (randomY == prevY);
				meteorY2 = randomY * 16;
				OAM[(2 * 4) + 0] = ((meteorY2 << 0) | (0 << 14)); // update y position
				timer2 = 20;
			}
			// meteor2 x
			if (timer2 > 19)
			{
				meteorX2 = meteorX2 - 2;
				if (meteorX2 < 1)
				{
					meteorX2 = 240;
				}
				if (frame > 1440 + 20)
				{
					OAM[(2 * 4) + 1] = ((meteorX2 << 0) | (1 << 14));
				}
				else
				{
					OAM[(2 * 4) + 1] = ((240 << 0) | (1 << 14));
				}
			}

			// meteor3 y
			if (timer3 > 159)
			{
				prevY = randomY;
				do
				{
					randomY = rand() % 9 + 1; // new y value is randomly selected every time meteor crosses screen
				} while (randomY == prevY);
				meteorY3 = randomY * 16;
				OAM[(3 * 4) + 0] = ((meteorY3 << 0) | (0 << 14)); // update y position
				timer3 = 40;
			}
			// meteor3 x
			if (timer3 > 39)
			{
				meteorX3 = meteorX3 - 2;
				if (meteorX3 < 1)
				{
					meteorX3 = 240;
				}
				if (frame > 720 + 40)
				{
					OAM[(3 * 4) + 1] = ((meteorX3 << 0) | (1 << 14));
				}
				else
				{
					OAM[(3 * 4) + 1] = ((240 << 0) | (1 << 14));
				}
			}

			// meteor4 y
			if (timer4 > 179)
			{
				prevY = randomY;
				do
				{
					randomY = rand() % 9 + 1; // new y value is randomly selected every time meteor crosses screen
				} while (randomY == prevY);
				meteorY4 = randomY * 16;
				OAM[(4 * 4) + 0] = ((meteorY4 << 0) | (0 << 14)); // update y position
				timer4 = 60;
			}
			// meteor4 x
			if (timer4 > 59)
			{
				meteorX4 = meteorX4 - 2;
				if (meteorX4 < 1)
				{
					meteorX4 = 240;
				}
				if (frame < 720 + 60 || frame > 1440 + 60)
				{
					OAM[(4 * 4) + 1] = ((meteorX4 << 0) | (1 << 14));
				}
				else
				{
					OAM[(4 * 4) + 1] = ((240 << 0) | (1 << 14));
				}
			}

			// meteor5 y
			if (timer5 > 199)
			{
				prevY = randomY;
				do
				{
					randomY = rand() % 9 + 1; // new y value is randomly selected every time meteor crosses screen
				} while (randomY == prevY);
				meteorY5 = randomY * 16;
				OAM[(5 * 4) + 0] = ((meteorY5 << 0) | (0 << 14)); // update y position
				timer5 = 80;
			}
			// meteor5 x
			if (timer5 > 79)
			{
				meteorX5 = meteorX5 - 2;
				if (meteorX5 < 1)
				{
					meteorX5 = 240;
				}
				if (frame > 720 + 80)
				{
					OAM[(5 * 4) + 1] = ((meteorX5 << 0) | (1 << 14));
				}
				else
				{
					OAM[(5 * 4) + 1] = ((240 << 0) | (1 << 14));
				}
			}

			// meteor6 y
			if (timer6 > 219)
			{
				prevY = randomY;
				do
				{
					randomY = rand() % 9 + 1; // new y value is randomly selected every time meteor crosses screen
				} while (randomY == prevY);
				meteorY6 = randomY * 16;
				OAM[(6 * 4) + 0] = ((meteorY6 << 0) | (0 << 14)); // update y position
				timer6 = 100;
			}
			// meteor6 x
			if (timer6 > 99)
			{
				meteorX6 = meteorX6 - 2;
				if (meteorX6 < 1)
				{
					meteorX6 = 240;
				}
				if (frame > 1440 + 100)
				{
					OAM[(6 * 4) + 1] = ((meteorX6 << 0) | (1 << 14));
				}
				else
				{
					OAM[(6 * 4) + 1] = ((240 << 0) | (1 << 14));
				}
			}
		}
		
		// movement
		uint16 buttonsPressed = *INPUT;
		buttonsPressed = (~buttonsPressed); // flipping binary to check for button press and not button release
		srand(rand() + buttonsPressed); // initialize random seed based on player inputs
		
		if (!gameOver)
		{
			if (buttonsPressed & RIGHT)
			{
				xPos++;
				if (xPos > 220)
				{
					xPos = 220;
				}
				OAM[(0 * 4) + 1] = ((xPos << 0) | (0 << 14));
			}
			if (buttonsPressed & LEFT)
			{
				xPos--;
				if (xPos < 1)
				{
					xPos = 1;
				}
				OAM[(0 * 4) + 1] = ((xPos << 0) | (0 << 14));
			}
			if (buttonsPressed & UP)
			{
				yPos--;
				if (yPos < 1)
				{
					yPos = 1;
				}
				OAM[(0 * 4) + 0] = ((yPos << 0) | (1 << 14));
			}
			if (buttonsPressed & DOWN)
			{
				yPos++;
				if (yPos > 151)
				{
					yPos = 151;
				}
				OAM[(0 * 4) + 0] = ((yPos << 0) | (1 << 14));
			}
		}

		// collision tests
		coll.x11 = meteorX1;
		coll.y11 = meteorY1;
		coll.x21 = meteorX2;
		coll.y21 = meteorY2;
		coll.x31 = meteorX3;
		coll.y31 = meteorY3;
		coll.x41 = meteorX4;
		coll.y41 = meteorY4;
		coll.x51 = meteorX5;
		coll.y51 = meteorY5;
		coll.x61 = meteorX6;
		coll.y61 = meteorY6;

		collFunction(&coll); // ARM CPU THUMB Code to calculate collision boundaries

		// meteor1		
		if (xPos > coll.x11 && xPos < coll.x12 && yPos > coll.y11 && yPos < coll.y12)
		{
			gameOver = true;
		}
		// meteor2
		if ((xPos > coll.x21 && xPos < coll.x22 && yPos > coll.y21 && yPos < coll.y22) && frame > 1440 + 20)
		{
			gameOver = true;
		}
		// meteor3
		if ((xPos > coll.x31 && xPos < coll.x32 && yPos > coll.y31 && yPos < coll.y32) && frame > 720 + 40)
		{
			gameOver = true;
		}
		// meteor4
		if ((xPos > coll.x41 && xPos < coll.x42 && yPos > coll.y41 && yPos < coll.y42) && (frame < 720 + 60 || frame > 1440 + 60))
		{
			gameOver = true;
		}
		// meteor5
		if ((xPos > coll.x51 && xPos < coll.x52 && yPos > coll.y51 && yPos < coll.y52) && frame > 720 + 80)
		{
			gameOver = true;
		}
		// meteor6
		if ((xPos > coll.x61 && xPos < coll.x62 && yPos > coll.y61 && yPos < coll.y62) && frame > 1440 + 100)
		{
			gameOver = true;
		}

		// reset game
		if (gameOver && (buttonsPressed & BUTTON_A))
		{
			frame = 0;
			score = 0;
			scoreTimer = 0;
			timer1 = 0;
			timer2 = 0;
			timer3 = 0;
			timer4 = 0;
			timer5 = 0;
			timer6 = 0;
			currentNote = 0;
			currentFrame = 0;
			xPos = 50;
			yPos = 80;
			meteorX1 = 240;
			meteorX2 = 240;
			meteorX3 = 240;
			meteorX4 = 240;
			meteorX5 = 240;
			meteorX6 = 240;
			meteorY1 = 16;
			meteorY2 = 32;
			meteorY3 = 48;
			meteorY4 = 64;
			meteorY5 = 80;
			meteorY6 = 96;
			xScroll0 = 0;
			xScroll1 = 0;
			shouldScroll = true;
			// rocket
			OAM[(0 * 4) + 0] = ((yPos << 0) | (1 << 14));
			OAM[(0 * 4) + 1] = ((xPos << 0) | (0 << 14));
			OAM[(0 * 4) + 2] = ((1 << 0) | (1 << 12));
			// meteor 1
			OAM[(1 * 4) + 0] = ((meteorY1 << 0) | (0 << 14));
			OAM[(1 * 4) + 1] = ((meteorX1 << 0) | (1 << 14));
			OAM[(1 * 4) + 2] = ((4 << 0) | (2 << 12));
			// meteor 2
			OAM[(2 * 4) + 0] = ((meteorY2 << 0) | (0 << 14));
			OAM[(2 * 4) + 1] = ((meteorX2 << 0) | (1 << 14));
			OAM[(2 * 4) + 2] = ((4 << 0) | (2 << 12));
			// meteor 3
			OAM[(3 * 4) + 0] = ((meteorY3 << 0) | (0 << 14));
			OAM[(3 * 4) + 1] = ((meteorX3 << 0) | (1 << 14));
			OAM[(3 * 4) + 2] = ((4 << 0) | (2 << 12));
			// meteor 4
			OAM[(4 * 4) + 0] = ((meteorY4 << 0) | (0 << 14));
			OAM[(4 * 4) + 1] = ((meteorX4 << 0) | (1 << 14));
			OAM[(4 * 4) + 2] = ((4 << 0) | (2 << 12));
			// meteor 5
			OAM[(5 * 4) + 0] = ((meteorY5 << 0) | (0 << 14));
			OAM[(5 * 4) + 1] = ((meteorX5 << 0) | (1 << 14));
			OAM[(5 * 4) + 2] = ((4 << 0) | (2 << 12));
			// meteor 6
			OAM[(6 * 4) + 0] = ((meteorY6 << 0) | (0 << 14));
			OAM[(6 * 4) + 1] = ((meteorX6 << 0) | (1 << 14));
			OAM[(6 * 4) + 2] = ((4 << 0) | (2 << 12));
			MAPMEM[0] = ((9 << 0) | (3 << 12));
			MAPMEM[1] = ((9 << 0) | (3 << 12));
			MAPMEM[2] = ((9 << 0) | (3 << 12));
			MAPMEM[3] = ((9 << 0) | (3 << 12));
			MAPMEM[4] = ((9 << 0) | (3 << 12));
			gameOver = false;
		}

		VBlankIntrWait();

	}


}

