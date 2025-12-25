#include "SDL2/SDL.h"

#include "draw.h"

const ColorScheme colorscheme = {
    .colors = {
        [CursorBg] = HEX_COL(0xeeee9e),
        [CursorFg] = HEX_COL(0x000000),
        [TextBg] = HEX_COL(0xffffec),
        [TextFg] = HEX_COL(0x000000),
    },
};

