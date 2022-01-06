#include "cachesim1.h"

// You will have access to the following globals:
// u8 block_bits, way_bits, sets_bits, address_bits;

/// Called once after each change to the defining extern values above.
/// we won't test it, but you can use it to set globals like `tag_bits` if you wish.
void global_init() {
    // optionally, add code here
}

/// update Tree-PLRU tracking bits based on an access of the given line
int newLRU(int oldLRU, int index) {
    return 0; // fix me
}

/// Given Tree-PLRU tracking bits, return index of least-recently-used line
int idxFromLRU(int lru) {
    return 0; // fix me
}

/// Given an address, return the block offset from it
int get_offset(u64 address) {
    return 0; // fix me
}
/// Given an address, return the set index from it
int get_index(u64 address) {
    return 0; // fix me
}
/// Given an address, return the tag from it
u64 get_tag(u64 address) {
    return 0; // fix me
}
