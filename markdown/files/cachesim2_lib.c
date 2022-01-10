#include "cachesim2.h"
#include <stdlib.h>

Line getLine(Cache *c, size_t setindex, size_t linenumber) {
    Line ans;
    ans.meta = c->_line_md + (setindex * c->way + linenumber);
    ans.block = c->_blocks + ((setindex * c->way + linenumber)*c->blocksize);
    return ans;
}

void makeCache(Cache *answer, size_t sets, size_t way, size_t blocksize) {
    answer->blocksize = blocksize;
    answer->sets = sets;
    answer->way = way;
    answer->plru = calloc(sets, sizeof(u16));
    answer->_line_md = calloc(sets*way, sizeof(LineMetadata));
    answer->_blocks = calloc(sets*way*blocksize, sizeof(u8));
    answer->isWriteback = 0;
    answer->backing = 0;
}

void freeCache(Cache *c) {
    if (c->plru) {
        free(c->plru); c->plru = 0; 
        free(c->_line_md); c->_line_md = 0; 
        free(c->_blocks); c->_blocks = 0; 
    }
    c->sets = 0;
}

