#include <psx.h>
#include <stdio.h>

#include "panic.h"
#include "game.h"
#include "common.h"

unsigned dpad_state;

static volatile bool vblank = false;

namespace gfx {

int
get_texture_page(int width, int height)
{
	static int last_page = 5;

	int page = last_page;
	last_page += (width + 63)/64;

	return page;
}

}

void
panic(const char *fmt, ...)
{
	char buf[256];

	va_list ap;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	printf("panic: %s\n", buf);

	for (;;)
		;
}

const char *
make_path(const char *name, const char *ext)
{
	static char buf[80];
	sprintf(buf, "cdrom:\\%s.%s;1", name, ext);
	return buf;
}

static void
vblank_handler()
{
	vblank = true;
}

static void
dpad_state_update(void)
{
	unsigned short pad;

	PSX_ReadPad(&pad, 0);

	dpad_state = 0;
	if (pad & PAD_UP) dpad_state |= DPAD_UP;
	if (pad & PAD_DOWN) dpad_state |= DPAD_DOWN;
	if (pad & PAD_LEFT) dpad_state |= DPAD_LEFT;
	if (pad & PAD_RIGHT) dpad_state |= DPAD_RIGHT;
	if (pad & PAD_CROSS) dpad_state |= DPAD_BUTTON;
}

void
initialize(void)
{
printf("initialize\n");
	PSX_Init();
printf("after PSX_Init\n");

	GsInit();
	GsClearMem();

	GsSetVideoMode(320, 240, VMODE_PAL);
	SetVBlankHandler(vblank_handler);

	game_initialize();

	dpad_state = 0;
}

static void
main_loop(void)
{
	int cur_buf = 0;

	for (;;) {
		vblank = 0;
		while (!vblank)
			;

		cur_buf ^= 1;
		GsSetDispEnvSimple(0, cur_buf ? 0 : 256);
		GsSetDrawEnvSimple(0, cur_buf ? 256 : 0, 320, 240);

		dpad_state_update();
		game_update();

		game_redraw();

		while (GsIsDrawing())
			;
	}
}

extern "C" int printf(const char *fmt, ...);

int
main(int argc, char *argv[])
{
printf("main\n");
	initialize();
	main_loop();

	return 0;
}
