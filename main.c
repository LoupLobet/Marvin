#include <stdlib.h>
#include <stdio.h>

#include "buffer.h"
#include "draw.h"


int
main(int argc, char *argv[])
{
    Buffer *buf;
    CLine *cline;
    Draw *drw;
    int nline;

    if ((drw = draw_create("marvin", 600, 600)) == NULL) {
        perror("cannot init draw");
        exit(1);
    }
    
    if ((buf = buffer_openfile("./Makefile")) == NULL) {
        perror("cannot open file");
        exit(1);
    }
    nline = 0;
    for (cline = buf->head; cline != NULL; cline = cline->next) {
        printf("%d: %s", nline++, cline->data);
    }
    if ((buf->rdr = bufferrender_create(buf->head, buf->tail, 0, buf->tail->len)) == NULL) {
        perror("cannot create buffer render");
        exit(1);
    }
    if ((buf->rdr = buffer_setrdr(buf, buf->rdr)) == NULL) {
        perror("cannot set buffer render");
        exit(1);
    }
    draw_run(drw);
    printf("gapline = %d, gapcol = %d\n", buf->rdr->gbuf->gapline, buf->rdr->gbuf->gapcol);

    
    return 0;
}
