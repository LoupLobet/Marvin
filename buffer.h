#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdio.h>

#include "utf/utf.h"

#define CLINE_DEFAULT_CAP 32
#define CLINE_GROWTH_RATE 2
#define GAPBUFFER_DEFAULT_CAP 128
#define GAPBUFFER_GROWTH_RATE 2

typedef struct Buffer Buffer;
typedef struct BufferRender BufferRender;
typedef struct CLine CLine;
typedef struct GapBuffer GapBuffer;

/*
 * A Buffer is a double linked list of lines of utf-8 bytes.
 * When loading a file from the disk in a Buffer, it needs
 * to be splitted in lines accordingly. The Buffer lines are
 * only modified when they go out scope of the BufferRender(er).
 * NB.: End of line '\n' are not excluded from the line bytes.
 */
struct Buffer {
    char *name;
    CLine *head, *tail;
    BufferRender *rdr;
};

Buffer    *buffer_create(char *);
Buffer    *buffer_openfile(char *);

/*
 * A BufferRender is a copy of the Buffer CLines that are currently
 * displayed (or inside the dot if the dot goes out of the screen)
 * as unicode codepoints. For easier text manipulation, it stores
 * lines as a Rune gap buffer with an underlying table of pointers
 * that points to lines.
 * When text is edited on the screen, modifications are applied
 * inside of the BufferRender lines. When a line goes out of
 * the renderer range, the new version of the line is converted
 * back CLine an written at the write stop in the Buffer.
 */
struct BufferRender {
    GapBuffer *gbuf;
    CLine *fromline, *toline;
    int fromcol, tocol;
};

struct CLine {
    long cap;
    long len;
    char *data;
    CLine *prev, *next;
};

char    *cline_append(CLine *, char);
CLine   *cline_create(void);
void     cline_free(CLine *);
void     cline_freenext(CLine *);
CLine   *cline_grow(CLine *);

/*
 * https://en.wikipedia.org/wiki/Gap_buffer
 * In addition to the usual Rune buffer, the GabBuffer struct has,
 * and maintain the current column and line of the gap.
 */
struct GapBuffer {
	Rune *bob;
	Rune *bog;
	Rune *eob;
	Rune *eog;
	long gapsize;
	long cap;
	long len;
    int gapline, gapcol;
};

GapBuffer    *gapbuffer_create(void);
int      gapbuffer_delafter(GapBuffer *, int);
int	     gapbuffer_delbefore(GapBuffer *, int);
void	 gapbuffer_free(GapBuffer *);
int	     gapbuffer_insafter(GapBuffer *, Rune *, int);
int	     gapbuffer_insbefore(GapBuffer *, Rune *, int);
int      gapbuffer_mvbackwardnr(GapBuffer *, int, Rune);
int      gapbuffer_mvforwardnr(GapBuffer *, int, Rune);
int	     gapbuffer_grow(GapBuffer *);

#endif
