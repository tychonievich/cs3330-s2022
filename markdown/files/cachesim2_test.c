#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cachesim2.h"

u8 block_bits, way_bits, sets_bits, address_bits;
static int verbosity = 1;

/**
 * An abstraction of RAM, used as a testing stub. Just prints to stdout,
 * doesn't actually remember anything.
 */
void writeToRAM(u64 address, size_t bytes, const u8* data) {
    printf("RAM[%lx..%lx] = [", address, (address-1+bytes));
    for(size_t i=0; i<bytes; i+=1) printf(" %02X", data[i]);
    printf(" ]\n");
}

static u8 _RAM_multiplier = 37;
/**
 * An abstraction of RAM, used as a testing stub. Uses a fixed randomish
 * sequence of bytes instead of any actual model of RAM.
 */
void readFromRAM(u64 address, size_t bytes, u8* data) {
    printf("... = RAM[%lx..%lx]\n", address, (address-1+bytes));
    for(size_t i=0; i<bytes; i+=1) {
        u64 a = (address + i)*_RAM_multiplier;
        a += (a>>8)*_RAM_multiplier;
        a += (a>>16)*_RAM_multiplier;
        a += (a>>32)*_RAM_multiplier;
        data[i] = (u8)a;
    }
}



/// Display the full cache
void showCache(FILE *out, Cache *c) {
    for(size_t si = 0; si < c->sets; si+=1) {
        if (c->way > 1) fprintf(out, "┌─ set%3zx ─ PLRU %4x ─┬──┈\n", si, c->plru[si]);
        for(size_t li = 0; li < c->way; li+=1) {
            Line line = getLine(c, si, li);
            fprintf(out, "│ %d %d %16zx │", line.meta->live, line.meta->dirty, line.meta->tag);
            for(size_t bi = 0; bi < c->blocksize; bi+=1)
                fprintf(out, " %02x", line.block[bi]);
            fprintf(out, "\n");
        }
    }
}

/**
 * A test file consists of one or more configuration lines followed by 0 or more accesses.
 * 
 * The configuration line has the form
 * 
 * 1. `cb` for writeback or `ct` for writethrough
 * 2. a base-10 integer, the block size bits
 * 3. a base-10 integer, the way bits
 * 4. a base-10 integer, the set count bits
 * 5. a base-10 integer, the bits per address. This must be the same for every configuration line.
 * 
 * The last configuration line is the L1 cache; the one before it the L2, etc.
 * 
 * Each access line consists of
 * 
 * 1. a two-character action code, consisting of `r` or `w` for read or write
 *    and `1` for a byte, `b` for a block, or `8` for a u64.
 * 2. an address in hexadecimal
 * 3. if writing, values to write: a byte in hex, or a long in hex, or a sequence of bytes in hex
 */
void runTestFile(FILE *src, FILE *dst) {
    Cache *c = 0;
    u8 *buffer = 0;
    while (!feof(src)) {
        char code, fun;
        if (2 != fscanf(src, "%c%c ", &code, &fun)) break;
        if (code == 'c') {
            Cache *tmp = calloc(1, sizeof(Cache));
            if (4 != fscanf(src, "%hhu %hhu %hhu %hhu\n", &block_bits, &way_bits, &sets_bits, &address_bits)) {
                fprintf(stderr, "Error: `c%c` missing one or more sizes\n", fun);
                break;
            }
            global_init();
            makeCache(tmp, 1uL<<sets_bits, 1uL<<way_bits, 1uL<<block_bits);
            tmp->backing = c;
            tmp->isWriteback = fun == 'b';
            c = tmp;
        } else if (code == 'r') {
            u64 addr;
            if (1 != fscanf(src, "%lx\n", &addr)) { fprintf(stderr, "Error: `r%c` missing address\n", fun); break; }
            if (fun == '1') fprintf(dst, "read %02x from cache[%zx]\n", getByte(c, addr), addr);
            else if (fun == '8') fprintf(dst, "read %016lx from cache[%zx]\n", getLong(c, addr), addr);
            else if (fun == 'b') {
                if (!buffer) buffer = calloc(c->blocksize, sizeof(u8));
                if (addr % c->blocksize != 0) { fprintf(stderr, "Error: `rb %lx` not block-aligned\n", addr); break; }
                getBlock(c, addr, buffer, c->blocksize);
                fprintf(dst, "read [");
                for(size_t i=0; i<c->blocksize; i+=1) fprintf(dst, " %02x", buffer[i]);
                fprintf(dst, " ] from cache[%zx]\n", addr);
            }
        } else if (code == 'w') {
            u64 addr;
            if (1 != fscanf(src, "%lx", &addr)) { fprintf(stderr, "Error: `r%c` missing address\n", fun); break; }
            if (fun == '1') {
                u8 value;
                if (1 != fscanf(src, " %hhx\n", &value)) { fprintf(stderr, "Error: `w1 %lx` missing value\n", addr); break; }
                if (verbosity >= 2) fprintf(dst, "setByte(c, 0x%lx, 0x%x)\n", addr, value);
                setByte(c, addr, value);
            } else if (fun == '8') {
                u64 value;
                if (1 != fscanf(src, " %lx\n", &value)) { fprintf(stderr, "Error: `w8 %lx` missing value\n", addr); break; }
                if (verbosity >= 2) fprintf(dst, "setLong(c, 0x%lx, 0x%lx)\n", addr, value);
                setLong(c, addr, value);
            } else if (fun == 'b') {
                if (!buffer) buffer = calloc(c->blocksize, sizeof(u8));
                size_t i;
                for(i=0; i<c->blocksize; i+=1)
                    if (1 != fscanf(src, " %hhx", buffer+i)) break;
                if (addr / c->blocksize != (addr+i-1)/c->blocksize) { fprintf(stderr, "Error: `wb %lx` overlaps two blocks\n", addr); break; }
                if (verbosity >= 2) fprintf(dst, "setBlock(c, 0x%lx, buffer, 0x%zx)\n", addr, i);
                setBlock(c, addr, buffer, i);
            }
        }
        if (verbosity >= 3) showCache(dst, c);
    }
    if (verbosity >= 1 && verbosity < 3) showCache(dst, c);
    
    int level = 1;
    while(c) {
        if (verbosity >= 4) {
            fprintf(dst, "\n========== L%d cache ==========\n", level);
            showCache(dst, c);
            fprintf(dst, "========== L%d cache ==========\n", level++);
        }
        freeCache(c);
        Cache *tmp = c;
        c = c->backing;
        free(tmp);
    }
    if (buffer) free(buffer);
}


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(stderr, "USAGE: %s patternfile [outputfile] [--seed=37] [--verbosity=1]\n", argv[0]);
        return 1;
    }
    FILE *inp = stdin;
    FILE *outp = stdout;
    for(int i=1; i<argc; i+=1) {
        fprintf(stderr,"arg %d = \"%s\"\n", i, argv[i]);
        if (argv[i][0] == '-') {
            if (strncmp(argv[i],"--seed=",7) == 0) {
                _RAM_multiplier = atoi(argv[i]+7);
            } else if (strncmp(argv[i],"--verbosity=",12) == 0) {
                verbosity = atoi(argv[i]+12);
            } else {
                fprintf(stderr, "USAGE: %s patternfile [outputfile] [--seed=37] [--verbosity=1]\n", argv[0]);
                fprintf(stderr,
                    "--seed=num:  sets the RAM-reading hook\n\n"
                    "--verbosity=1:  0 = don't show cache contents at end\n"
                    "                1 = show cache contents at end\n"
                    "                2 = .. and every set___() call\n"
                    "                3 = .. and cache between each call\n\n"
                    "                4 = .. and full hierarchy at end\n\n"
                    "Example patternfile format:\n"
                    "---\n"
                    "cb 4 2 5 48\n"
                    "r1 feaf\n"
                    "r8 feaf034\n"
                    "wb feaf000 0f 1e 2d 3c 4b 5a 69 78 87 69 5a 4b 3c 2d 1e 0f\n"
                    "w8 1234feafeee fedcba9801234567\n"
                    "---\n"
                );
                return 2;
            }
        } else if (inp == stdin) inp = fopen(argv[i], "r");
        else if (outp == stdout) outp = fopen(argv[i], "w");
        else {
            fprintf(stderr, "USAGE: %s patternfile [outputfile] [--seed=37] [--verbosity=1]\n", argv[0]);
            return 1;
        }
    }
    
    runTestFile(inp, outp);
    if (inp != stdin) fclose(inp);
    if (outp != stdout) fclose(outp);
    return 0;
}
