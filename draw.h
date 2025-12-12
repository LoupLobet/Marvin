#ifndef _DRAW_H_
#define _DRAW_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct Draw Draw;

struct Draw {
	SDL_Renderer *renderer;
	SDL_Window *window;
	TTF_Font *font;
};

Draw	*draw_create(char *, int, int);
void	 draw_run(Draw *);
void	 draw_event(Draw *);
SDL_Rect	 draw_str(Draw *, char *, SDL_Point *, SDL_Color);

#endif
