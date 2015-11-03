#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 10, defaultBlockSize = 80;

int main() {
    unsigned seed = time(0);
    debug("seed = %u\n", seed);
    srand(seed);
    debug("=== Creating index file...\n");
    Index index("index.txt~", Type(INTTYPE, 4));
    debug("=== Creating index file...OK!\n");
    debug("=== Inserting...\n");
    map<int, int> D;
    int T=20;
    while(T--){
        int x, y;
        x = rand() % 500, y = rand() % 500;
        if (D.find(x) == D.end()) {
            D[x] = y;
            index.insert(PValue(new Int(x)), y);
        }
    }
    debug("=== Inserting...OK!\n");
    debug("=== Iterating...\n");
    auto it = index.begin();
    for (auto u: D) {
        printf("%d %d ", u.first, u.second);
        /*
        assert(*it.key() == *PValue(new Int(u.first)));
        assert(it.value() == u.second);
        */
        cerr<<*it.key()<<' '<<it.value()<<endl;
        ++it;
    }
    assert(it == index.end());
    debug("=== Iterating...OK!\n");
    debug("##########Index is OK!###########\n\n");
}
