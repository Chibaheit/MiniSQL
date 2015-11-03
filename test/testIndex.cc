#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 4, defaultBlockSize = 40;

int main() {
    unsigned seed = 1446524652;//time(0);
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
            index.print();
        }
    }
    debug("=== Inserting...OK!\n");
    debug("=== Iterating...\n");
    auto it = index.begin();
    for (auto u: D) {
        #define ASSERT
        #ifdef ASSERT
        assert(*it.key() == *PValue(new Int(u.first)));
        assert(it.value() == u.second);
        #else
        printf("%d %d ", u.first, u.second);
        cerr<<*it.key()<<' '<<it.value()<<endl;
        #endif
        ++it;
    }
    assert(it == index.end());
    debug("=== Iterating...OK!\n");
    index.print();
    debug("##########Index is OK!###########\n\n");
}
