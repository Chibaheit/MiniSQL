#include "../src/index/index.h"
#include <map>
#include <ctime>

int defaultNumBlocks = 100, defaultBlockSize = 256;

int range = 100000;
int R() {
    return abs(((rand()<<15)^rand()) % range);
}

map<int, int> D;
void random_insert(Index *index, int T) {
    while(T--){
        int x, y;
        x = R() % 500, y = R() % 500;
        //debug("%d %d\n", x, y);
        PValue val(new Int(x));
        if (D.find(x) == D.end()) {
            D[x] = y;
            assert(index->insert(val, y) == true);
            //index->print();
        } else assert(index->insert(val, y) == false);
    }
}

void random_erase(Index *index, int T) {
    //index->print();
    while(T--){
        int x = R() % 500;
        auto it = D.lower_bound(x);
        if (it != D.end()) {
            PValue val(new Int(it->first));
            D.erase(it->first);
            assert(index->erase(val) == 1);
            //index->print();
        } else assert(index->erase(PValue(new Int(x))) == 0);
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
    srand(seed);
    range = R() % 1000000 + 1;
    debug("seed = %u, range = %d\n", seed, range);
    debug("=== Creating index file...\n");
    Index *index = new Index("index.txt~", Type(INTTYPE, 4));
    debug("=== Creating index file...OK!\n");
    debug("=== Operating...\n");
    int X=R()%99+1, Y=100-X, T=5000;
    if(X<Y)swap(X,Y);
    for(int i=0; i<T; ++i) {
        random_insert(index, X);
        random_erase(index, Y);
        check(index);
    }
    delete index;
    Buffer::flush();
    index = new Index("index.txt~");
    for(int i=0; i<T; ++i) {
        random_insert(index, Y);
        random_erase(index, X);
        check(index);
    }
    debug("=== Operating...OK!\n");
    //index->print();
    Buffer::flush();
    debug("##########Index is OK!###########\n\n");
}
