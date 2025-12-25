#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "buffer.h"
#include "draw.h"
#include "utf/utf.h"
#include "util.h"

#include "config.h"

int running = 1;

Draw *
draw_create(char *name, int w, int h)
{
	Draw *drw;

	if ((drw = malloc(sizeof(Draw))) == NULL)
		return NULL;
	if ((drw->win = SDL_CreateWindow(name,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0)) == NULL)
	{
		free(drw);
		return NULL;
	}

	drw->rdr = SDL_CreateRenderer(drw->win, -1, SDL_RENDERER_ACCELERATED);
	if (TTF_Init() == -1) {
		free(drw);
		return NULL;
	}
	// drw->font = TTF_OpenFont("/usr/share/fonts/IosevkaNerdFontMono-Regular.ttf", 25);
	drw->font = TTF_OpenFont("/usr/share/fonts/TTF/DejaVuSans.ttf", 25);
	if (!drw->font) {
		free(drw);
		return NULL;
	}
	drw->rect = (SDL_Rect){0, 0, w, h};
	return drw;
}

void
draw_run(Draw *drw)
{
	Text *txt;

	if ((txt = text_create("/home/loup/src/Marvin/Makefile", drw->rect,
	                       (ColorScheme *)&colorscheme)) == NULL)
	{
		return;
	}
	if ((txt->buf->rdr = buffer_setrdr(txt->buf, txt->buf->rdr)) == NULL) {
		return;
	}
	while (running) {
		draw_event(drw);
		/* draw background */
		int w, h;
		SDL_GetWindowSize(drw->win, &w, &h);
		SDL_SetRenderDrawColor(drw->rdr, 0, 0, 0, 0);
		SDL_RenderClear(drw->rdr);
		text_render(txt, drw);

		SDL_RenderPresent(drw->rdr);

		SDL_Delay(1/60);
	}
	TTF_CloseFont(drw->font);
	TTF_Quit();
	SDL_DestroyRenderer(drw->rdr);
	SDL_DestroyWindow(drw->win);
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
			case 'q':
				running = 0;
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
draw_renderrune(Draw *d, Rune r, SDL_Point *pt, SDL_Color color)
{
	SDL_Rect textrect = { pt->x, pt->y, 0, 0 };
	SDL_Surface *surface;
	SDL_Texture *texture;

	if ((surface = TTF_RenderGlyph32_Blended(d->font, r, color)) == NULL)
		return textrect;
	if ((texture = SDL_CreateTextureFromSurface(d->rdr, surface)) == NULL) {
		SDL_FreeSurface(surface);
		return textrect;
	}
	SDL_QueryTexture(texture, NULL, NULL, &textrect.w, &textrect.h);
	SDL_RenderCopy(d->rdr, texture, NULL, &textrect);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texture);
	return textrect;
}

Text *
text_create(char *name, SDL_Rect rect, ColorScheme *scheme)
{
	Text *txt;

	if ((txt = malloc(sizeof(Text))) == NULL)
		return NULL;
	txt->rect = rect;
	txt->scheme = scheme;
	if (name[0] == '/')
		txt->buf = buffer_openfile(name);
	else
		txt->buf = buffer_create(name);
	if (txt->buf == NULL) {
		free(txt);
		return NULL;
	}
	txt->buf->rdr = bufferrender_create(txt->buf->head, txt->buf->tail,
	                                	0, txt->buf->tail->len);
	if (txt->buf->rdr == NULL) {
		buffer_free(txt->buf);
		free(txt);
	}
	return txt;
}

void
text_render(Text *txt, Draw *drw)
{
	SDL_Rect linerect, runerect;
	SDL_Color *colors;
	int advance, i;
	Rune *r;
	
	colors = txt->scheme->colors;
	/* text background */
	SDL_SetRenderDrawColor(drw->rdr, colors[TextBg].r, colors[TextBg].g,
	                       colors[TextBg].b, colors[TextBg].a);
	SDL_RenderFillRect(drw->rdr, &txt->rect);
	/* text runes */
	SDL_SetRenderDrawColor(drw->rdr, colors[TextFg].r, colors[TextFg].g,
	                       colors[TextFg].b, colors[TextFg].a);
	linerect = (SDL_Rect){txt->rect.x, txt->rect.y, 0, 0};
	r = NULL;
	while ((r = gapbuffer_nextrune(txt->buf->rdr->gbuf, r)) != NULL) {
		TTF_GlyphMetrics(drw->font, *r, NULL, NULL, NULL, NULL, &advance);
		if (*r == '\t') {
			for (i = 0; i < 4; i++) {
				runerect = draw_renderrune(drw, (Rune)' ', &(SDL_Point){linerect.x + linerect.w,
				                           linerect.y}, colors[TextFg]); 
				linerect.w += runerect.w;
				linerect.h = MAX(linerect.h, runerect.h);
			}
			continue;
		}
		if (linerect.w + advance > txt->rect.w || *r == '\n') {
			linerect = (SDL_Rect){txt->rect.x, linerect.y + linerect.h, 0, 0};
			if (*r == '\n')
				continue;
		}
		runerect = draw_renderrune(drw, *r, &(SDL_Point){linerect.x + linerect.w, linerect.y},
		                           colors[TextFg]); 
		linerect.w += runerect.w;
		linerect.h = MAX(linerect.h, runerect.h);
	}
}

void
text_free(Text *txt)
{
	buffer_free(txt->buf);
	free(txt);
}
