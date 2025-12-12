#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "draw.h"

int running = 1;

Draw *
draw_create(char *name, int w, int h)
{
	Draw *drw;

	if ((drw = malloc(sizeof(Draw))) == NULL)
		return NULL;
	if ((drw->window = SDL_CreateWindow(name,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0)) == NULL)
	{
		free(drw);
		return NULL;
	}

	drw->renderer = SDL_CreateRenderer(drw->window, -1, SDL_RENDERER_ACCELERATED);
	if (TTF_Init() == -1) {
		free(drw);
		return NULL;
	}
	drw->font = TTF_OpenFont("/Users/loup/Library/Fonts/Iosevka-Medium.ttf", 25);
	if (!drw->font) {
		free(drw);
		return NULL;
	}
	return drw;
}

void
draw_run(Draw *drw)
{
	while (running) {
		draw_event(drw);
		/* draw background */
		int w, h;
		SDL_GetWindowSize(drw->window, &w, &h);
		SDL_SetRenderDrawColor(drw->renderer, 0, 0, 0, 0);
		SDL_RenderClear(drw->renderer);

		SDL_RenderPresent(drw->renderer);

		SDL_Delay(1/60);
	}
	TTF_CloseFont(drw->font);
	TTF_Quit();
	SDL_DestroyRenderer(drw->renderer);
	SDL_DestroyWindow(drw->window);
	SDL_Quit();
}

void
draw_event(Draw *d)
{
	SDL_Event e;

	while(SDL_PollEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			running = 0;
			break;
		case SDL_KEYDOWN:
			printf("KeyDown: %d\n", e.key.keysym.sym);
			switch (e.key.keysym.sym) {
			case SDLK_LEFT:
				break;
			case SDLK_RIGHT:
				break;
			case SDLK_UP:
				break;
			case SDLK_DOWN:
				break;
			case SDLK_BACKSPACE:
				break;
			case '\n':
				break;
			case SDLK_RETURN:
				break;
			default:
			    break;
		    }
		default:
			;
			break;
		}
	}
}

SDL_Rect
draw_str(Draw *d, char *text, SDL_Point *pt, SDL_Color color)
{
	SDL_Rect textrect = { pt->x, pt->y, 0, 0 };
	SDL_Surface *surface;
	SDL_Texture *texture;

	if ((surface = TTF_RenderText_Blended(d->font, text, color)) == NULL)
		return textrect;
	if ((texture = SDL_CreateTextureFromSurface(d->renderer, surface)) == NULL) {
		SDL_FreeSurface(surface);
		return textrect;
	}
	SDL_QueryTexture(texture, NULL, NULL, &textrect.w, &textrect.h);
	SDL_RenderCopy(d->renderer, texture, NULL, &textrect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	return textrect;
}
