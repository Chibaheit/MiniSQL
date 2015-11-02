#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 10, defaultBlockSize = 4096;

map<int, int> D;

int main() {
    Index index("index.txt~", Type(INTTYPE, 4));
    int T=1;
    while(T--){
        int x, y;
        x = rand(), y = rand();
        if (D.find(x) == D.end()) {
            D[x] = y;
            index.insert(PValue(new Int(x)), y);
        }
    }
    auto it = index.begin();
    for (auto u: D) {
        auto tmp = it.pair();
        assert(*tmp.first == *PValue(new Int(u.first)));
        assert(tmp.second == u.second);
        ++it;
    }
    assert(it == index.end());
}
