#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 5, defaultBlockSize = 40;

int R() {
    return abs((rand()<<8)^rand());
}

map<int, int> D;
void random_insert(Index *index, int T) {
    while(T--){
        int x, y;
        x = R() % 500, y = R() % 500;
        //debug("%d %d\n", x, y);
        if (D.find(x) == D.end()) {
            D[x] = y;
            index->insert(PValue(new Int(x)), y);
            //index->print();
        }
    }
}

void random_erase(Index *index, int T) {
    //index->print();
    while(T--){
        int x = R() % 500;
        auto it = D.lower_bound(x);
        if (it != D.end()) {
            D.erase(it->first);
            index->erase(PValue(new Int(it->first)));
            index->print();
        }
    }
}

void check(Index *index) {
    auto it = index->begin();
    for (auto u: D) {
        assert(!(it == index->end()));
        #define ASSERT
        #ifdef ASSERT
        assert(*it.key() == *PValue(new Int(u.first)));
        assert(it.value() == u.second);
        #else
        fprintf(stderr, "%d %d ", u.first, u.second);
        cerr<<*it.key()<<' '<<it.value()<<endl;
        #endif
        ++it;
    }
    assert(it == index->end());
}

int main() {
    unsigned seed = time(0);
    debug("seed = %u\n", seed);
    srand(seed);
    debug("=== Creating index file...\n");
    Index *index = new Index("index.txt~", Type(INTTYPE, 4));
    debug("=== Creating index file...OK!\n");
    debug("=== Operating...\n");
    int X=10, Y=5;
    for(int i=0; i<1; ++i) {
        random_insert(index, X);
        random_erase(index, Y);
        check(index);
    }
    Buffer::flush();
    delete index;
    index = new Index("index.txt~");
    for(int i=0; i<1; ++i) {
        random_insert(index, Y);
        random_erase(index, X);
        check(index);
    }
    debug("=== Operating...OK!\n");
    //index->print();
    debug("##########Index is OK!###########\n\n");
}
