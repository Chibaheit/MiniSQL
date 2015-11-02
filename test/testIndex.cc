#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 10, defaultBlockSize = 4096;

int main() {
    debug("=== Creating index file...");
    Index index("index.txt~", Type(INTTYPE, 4));
    debug("OK!\n");
    debug("=== Inserting...");
    map<int, int> D;
    int T=1;
    while(T--){
        int x, y;
        debug("before insert\n");
        x = rand(), y = rand();
        if (D.find(x) == D.end()) {
            debug("before insert2\n");
            cout<<"test"<<endl;
            D[x] = y;
            cout<<"test"<<endl;
            fflush(stdout);
            debug("before insert3\n");
            fflush(stderr);
            //index.insert(PValue(new Int(x)), y);
        }
    }
    debug("OK!\n");
    debug("=== Iterating...");
    auto it = index.begin();
    for (auto u: D) {
        auto tmp = it.pair();
        assert(*tmp.first == *PValue(new Int(u.first)));
        assert(tmp.second == u.second);
        ++it;
    }
    assert(it == index.end());
    debug("OK\n");
}
