#ifndef COMMON_H_
#define COMMON_H_

enum {
	SCREEN_WIDTH = 320,
	SCREEN_HEIGHT = 240,
};

enum dpad_bits {
	DPAD_LEFT = 1,
	DPAD_RIGHT = 2,
	DPAD_DOWN = 4,
	DPAD_UP = 8,
	DPAD_BUTTON = 16,
};

extern unsigned dpad_state;

extern const char *make_path(const char *name, const char *ext);

class font;
extern font *medium_font;

#endif /* COMMON_H_ */
