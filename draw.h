#ifndef _DRAW_H_
#define _DRAW_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "buffer.h"
#include "utf/utf.h"

#define HEX_COL(hex) \
	{ .r = (int)((hex>>16)&0xff),\
      .g = (int)((hex>>8)&0xff),\
      .b = (int)((hex)&0xff),\
      .a = 0, }\

enum {
	CursorBg,
	CursorFg,
	TextBg,
	TextFg,
	LastCol,
};

typedef struct Draw Draw;
typedef struct Text Text;
typedef struct ColorScheme ColorScheme;

struct ColorScheme {
	SDL_Color colors[LastCol];
};

struct Draw {
	SDL_Rect rect;
	SDL_Renderer *rdr;
	SDL_Window *win;
	TTF_Font *font;
};

Draw	*draw_create(char *, int, int);
void	 draw_run(Draw *);
void	 draw_event(Draw *);
SDL_Rect	 draw_renderrune(Draw *, Rune, SDL_Point *, SDL_Color);

struct Text {
	Buffer *buf;
	SDL_Rect rect;
	ColorScheme *scheme;
};

Text	*text_create(char *, SDL_Rect, ColorScheme *);
void	 text_free(Text *);
void 	 text_render(Text *, Draw *);

#endif
