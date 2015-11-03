#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 10, defaultBlockSize = 4096;

int main() {
    debug("=== Creating index file...\n");
    Index index("node.txt~", Type(INTTYPE, 4));
    debug("=== Creating index file...OK!\n");
    Buffer::flush();
}
