#include <stdio.h>

#include "buffer.h"

int
main(int argc, char *argv[])
{
    Buffer *buf;
    CLine *cline;
    int nline;

    if ((buf = buffer_openfile("./Makefile")) == NULL)
        perror("cannot open file");
    nline = 0;
    for (cline = buf->head; cline != NULL; cline = cline->next) {
        printf("%d: %s", nline++, cline->data);
    }
    return 0;
}
