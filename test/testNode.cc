#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 3, defaultBlockSize = 4096;

int main() {
    unsigned seed = time(0);
    debug("seed = %u\n", seed);
    srand(seed);
    debug("=== Creating index file...\n");
    Index index("node.txt~", Type(INTTYPE, 4));
    debug("=== Creating index file...OK!\n");
    for (int i = 0; i < 100; ++i) {
        assert(index.getNewBlock() == i+2);
        unsigned id = rand()%(i+2);
        //debug("Accessing id = %u\n", id);
        Buffer::access("node.txt~", id);
    }
    Buffer::flush();
}
