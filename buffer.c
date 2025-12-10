#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "utf/utf.h"

Buffer *
buffer_create(char *name)
{
    Buffer *buf;

    if ((buf = malloc(sizeof(Buffer))) == NULL)
        return NULL;
    if ((buf->name = strdup(name)) == NULL) {
        free(buf);
        return NULL;
    }
    buf->head = NULL;
    buf->tail = NULL;
    buf->rdr = NULL;
    return buf;
}

Buffer *
buffer_openfile(char *path)
{
    CLine *head, *tail, *l;
    Buffer *buf;
    FILE *fp;
    char c;

    if ((fp = fopen(path, "r")) == NULL) {
        return NULL;
    }
    if ((head = cline_create()) == NULL) {
        fclose(fp);
        return NULL;
    }
    /* Split the file in lines */
    l = head;
    tail = head;
    while (fread(&c, 1, 1, fp)) {
        if (!cline_append(l, c)) {
            cline_freenext(head);
            fclose(fp);
            return NULL;
        }
        if (c == '\n') {
            if ((l->next = cline_create()) == NULL) {
                cline_freenext(head);
                fclose(fp);
                return NULL;
            }
            l->next->prev = l;
            l = l->next;
            tail = l;
        }
    }
    fclose(fp);
    if ((buf = buffer_create(path)) == NULL) {
        cline_freenext(head);
        return NULL;
    }
    buf->head = head;
    buf->tail = tail;
    return buf;
}

BufferRender *
buffer_setrdr(Buffer *buf, BufferRender *rdr)
{
	Rune runeline[2048];
	int i, k, n;
	CLine *p;
	int linelen, runelen;

	for (p = rdr->fromline; p != NULL && p->prev != rdr->toline; p = p->next) {
		n = 0;
		i = 0;
		linelen = p->len;
		if (p == rdr->fromline) /* trim left first line */
			i = rdr->fromcol;
		if (p == rdr->toline) /* trim right last line */
			linelen = rdr->tocol;
		while (i < linelen) {
			runelen = chartorune(runeline + n, p->data + i);
			n++;
			i += runelen;
			if (n == sizeof(runeline)/sizeof(Rune) || i == linelen) {
				k = gapbuffer_insbefore(rdr->gbuf, runeline, n);
				/* gapbuffer reallocation went wrong for some reason */
				if (k != n) {
					free(rdr);
					gapbuffer_free(rdr->gbuf);
					return NULL;
				}
				n = 0;
			}
		}
	}
	return rdr;
}

BufferRender *
bufferrender_create(CLine *fromline, CLine *toline, int fromcol, int tocol)
{
	BufferRender *rdr;

	if ((rdr = malloc(sizeof(BufferRender))) == NULL)
		return NULL;
	rdr->fromline = fromline;
	rdr->toline = toline;
	rdr->fromcol = fromcol;
	rdr->tocol = tocol;
	rdr->gbuf = gapbuffer_create();
	return rdr;
}

void
buffer_free(Buffer *buf)
{
    cline_freenext(buf->head);
    free(buf->name);
    free(buf);
}

char *
cline_append(CLine *cline, char c)
{
    CLine *grownline;

    if (cline->len == cline->cap) {
        if ((grownline = cline_grow(cline)) == NULL) {
            return 0;
        }
        cline = grownline;
    }
    cline->data[cline->len] = c;
    cline->len++;
    return cline->data + cline->len;
}

CLine *
cline_create(void)
{
    CLine *cline;

    if ((cline = malloc(sizeof(CLine))) == NULL)
        return NULL;
    cline->next = NULL;
    cline->prev = NULL;
    cline->cap = CLINE_DEFAULT_CAP;
    cline->len = 0;
    if ((cline->data = malloc(cline->cap)) == NULL) {
        free(cline);
        return NULL;
    }
    return cline;
}

void
cline_free(CLine *cline)
{
    free(cline->data);
    free(cline);
}

void
cline_freenext(CLine *cline)
{
    CLine *next;

    next = cline->next;
    free(cline->data);
    free(cline);
    if (next != NULL)
        cline_freenext(next);
}

CLine *
cline_grow(CLine *cline)
{
    long newcap;
    char *newdata;

    newcap = cline->cap * CLINE_GROWTH_RATE;
    if ((newdata = realloc(cline->data, newcap)) == NULL)
        return NULL;
    cline->data = newdata;
    cline->cap = newcap;
    return cline;
}


GapBuffer *
gapbuffer_create(void)
{
	GapBuffer *gbuf;
	
	if ((gbuf = malloc(sizeof(GapBuffer))) == NULL)
		return NULL;
	gbuf->cap = GAPBUFFER_DEFAULT_CAP;
	if ((gbuf->bob = malloc(gbuf->cap)) == NULL) {
		free(gbuf);
		return NULL;
	}
	gbuf->len = 0;
	gbuf->gapsize = gbuf->cap;
	gbuf->eob = gbuf->bob + gbuf->cap - 1;
	gbuf->bog = gbuf->bob;
	gbuf->eog = gbuf->eob;
    gbuf->gapline = 0;
    gbuf->gapcol = 0;
	return gbuf;
}

int
gapbuffer_delafter(GapBuffer *gbuf, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		if (gbuf->eog == gbuf->eob)
			return i;
		gbuf->eog++;
		gbuf->gapsize++;
		gbuf->len--;
	}
	return i;
}


int
gapbuffer_delbefore(GapBuffer *gbuf, int n)
{
    Rune *r;
	int i;

	for (i = 0; i < n; i++) {
		if (gbuf->bog == gbuf->bob)
			return i;
		gbuf->gapcol--;
		if (*(gbuf->bog - 1) == (Rune)'\n') {
		    gbuf->gapline--;
		    /*
		     * We need to explore the gap buffer to find the begining
		     * of the line, in order to compute the new gapcol.
		     */
		    r = gbuf->bog - 1;
		    for (gbuf->gapcol = 0; r >= gbuf->bob; gbuf->gapcol++)
		        r -= 1;
		}
		gbuf->bog--;
		gbuf->gapsize++;
		gbuf->len--;
	}
	return i;
}

void
gapbuffer_free(GapBuffer *gbuf)
{
	free(gbuf->bob);
	free(gbuf);
}

int
gapbuffer_insafter(GapBuffer *gbuf, Rune *v, int n)
{
	int i;

	for (i = n - 1; i >= 0; i--) {
		*gbuf->eog = v[i];
		gbuf->eog--;
		gbuf->gapsize--;
		gbuf->len++;
		if (gbuf->gapsize <= 1) {
			if (!gapbuffer_grow(gbuf))
				return i;
		}
	}
	return i;
}

int
gapbuffer_insbefore(GapBuffer *gbuf, Rune *v, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		*gbuf->bog = v[i];
		gbuf->gapcol++;
		printf("v[i] = <%d>\n", v[i]);
		if (v[i] == (Rune)'\n') {
		    gbuf->gapline++;
		    gbuf->gapcol = 0;
		}
		gbuf->bog++;
		gbuf->gapsize--;
		gbuf->len++;
		if (gbuf->gapsize <= 1) {
			if (!gapbuffer_grow(gbuf))
				return i;
		}
	}
	return i;
}

int
gapbuffer_mvbackward(GapBuffer *gbuf, int n)
{
    Rune *r;
	int moved;

	moved = 0;
	while (moved != n) {
		if (gbuf->bog == gbuf->bob)
			return moved;
		gbuf->gapcol--;
		if (*(gbuf->bob - 1) == (Rune)'\n') {
		    gbuf->gapline--;
		    /*
		     * We need to explore the gap buffer to find the begining
		     * of the line, in order to compute the new gapcol.
		     */
		    r = gbuf->bog - 1;
		    for (gbuf->gapcol = 0; r >= gbuf->bob; gbuf->gapcol++)
		        r -= 1;
		}
		*gbuf->eog = *(gbuf->bog - 1);
		gbuf->bog--;
		gbuf->eog--;
		moved++;
	}
	return moved;
}

int
gapbuffer_mvforward(GapBuffer *gbuf, int n)
{
	int moved;

	moved = 0;
	while (moved != n) {
		if (gbuf->eog == gbuf->eob)
			return moved;
		gbuf->gapcol++;
		if (gbuf->eog[1] == (Rune)'\n') {
		    gbuf->gapline++;
		    gbuf->gapcol = 0;
		}
		*gbuf->bog = *(gbuf->eog + 1);
		gbuf->bog++;
		gbuf->eog++;
		moved++;
	}
	return moved;
}

int
gapbuffer_grow(GapBuffer *gbuf)
{
	Rune *new;
	long leftsize, newcap, rightsize;
	long gapsize;

	newcap = (gbuf->cap < 1 ? 2 : gbuf->cap * 2);
	leftsize = gbuf->bog - gbuf->bob;
	rightsize = gbuf->eob - gbuf->eog;
	if ((new = realloc(gbuf->bob, newcap)) == NULL)
		return 0;
	gapsize = newcap - (gbuf->cap - gbuf->gapsize);
	if (gbuf->eog != gbuf->eob)
		memcpy(new + leftsize + gapsize, new + leftsize + gbuf->gapsize, rightsize);
	gbuf->bob = new;
	gbuf->bog = gbuf->bob + leftsize;
	gbuf->eog = gbuf->bog + gapsize - 1;
	gbuf->eob = new + newcap - 1;
	gbuf->cap = newcap;
	gbuf->gapsize = gapsize;
	/* buf->len  = buf->len */
	return newcap;
}
