#include "../src/index/index.h"
#include <map>

int defaultNumBlocks = 100, defaultBlockSize = 256;

int R() {
    return (rand()<<15)^rand();
}

map<int, int> D;
void random_insert(Index *index, int T) {
    debug("=== Inserting...\n");
    while(T--){
        int x, y;
        x = R(), y = R();
        if (D.find(x) == D.end()) {
            D[x] = y;
            index->insert(PValue(new Int(x)), y);
            //index->print();
        }
    }
    debug("=== Inserting...OK!\n");
}

void check(Index *index) {
    debug("=== Iterating...\n");
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
    debug("=== Iterating...OK!\n");
}

int main() {
    unsigned seed = 1446524652;//time(0);
    debug("seed = %u\n", seed);
    srand(seed);
    debug("=== Creating index file...\n");
    Index *index = new Index("index.txt~", Type(INTTYPE, 4));
    debug("=== Creating index file...OK!\n");
    random_insert(index, 25000);
    Buffer::flush();
    delete index;
    index = new Index("index.txt~");
    check(index);
    random_insert(index, 25000);
    check(index);
    //index->print();
    debug("##########Index is OK!###########\n\n");
}
