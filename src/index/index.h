#ifndef INDEX_H
#define INDEX_H

// Use Index class to manage an index.

#include "../common.h"
#include "../buffer/buffer.h"
#include <utility>

// debugging
#ifndef DEBUG
    #define DEBUG
    #undef debug
    #define debug(args...) fprintf(stderr, args)
#endif

const int szInt = sizeof(int);

class Node {
private:
    Type m_key_type;
    unsigned n, m_unit;
    void init() {
        m_unit = m_key_type.size() + szInt;
        n = (m_block.size() - szInt*3) / m_unit;
        assert(n >= 3);
    }
public:
    Block &m_block;
    static unsigned LEAF, ROOT, EMPTY;
    // open a Node Block
    Node(Block *block, Type key_type):
        m_block(*block), m_key_type(key_type) {
        init();
    }
    // initialize an empty Node Block
    void initialize(unsigned _mask) {
        set(MASK, _mask);
        set(SIZE, 0);
        set(NEXT, 0);
    }
    unsigned num() const {return n;}
    enum offset {MASK, SIZE, NEXT};
    void set(offset id, unsigned x) const {
        m_block[(int)id-1] = x;
        m_block.setDirty();
    }
    unsigned get(offset id) const {
        return m_block[(int)id-1];
    }
    unsigned mask() const {return m_block[-1];}
    unsigned size() const {return m_block[-2];}
    unsigned next() const {return m_block[-3];}
    // get the ith key
    PValue getKey(int i) const {
        assert(i>=0 && i<size());
        return m_key_type.create(
            m_block.constData() + m_unit * i + szInt
        );
    }
    // set the ith key
    void setKey(int i, PValue val) const {
        assert(i>=0 && i<size());
        val->memoryCopy(m_block.data() + m_unit * i + szInt);
    }
    // get the ith pointer
    unsigned getPtr(int i) const {
        assert(i>=0 && i<size());
        return *(unsigned *)(m_block.constData() + m_unit * i);
    }
    // set the ith pointer
    void setPtr(int i, unsigned ptr) const {
        assert(i>=0 && i<size());
        *(unsigned *)(m_block.data() + m_unit * i) = ptr;
    }
    // get the ith pair
    pair<PValue, unsigned> getPair(int i) const {
        return make_pair(getKey(i), getPtr(i));
    }
    // set the ith pair
    void setPair(int i, pair<PValue, unsigned> pr) const {
        setKey(i, pr.first);
        setPtr(i, pr.second);
    }
    // shift elements in [begin, size()) to [begin + shamt, size() + shamt)
    void shiftRight(unsigned begin, unsigned shamt) const {
        assert(begin >= 0 && begin + shamt < size());
        set(Node::SIZE, size() + shamt);
        for (unsigned i = size() - shamt - 1, j = size() - 1; i >= begin; --i, --j) {
            setPtr(j, getPtr(i));
            setKey(j, getKey(i));
        }
    }
    // erase entries with index in range [begin, end)
    void erase(unsigned begin, unsigned end) const {
        assert(begin >= 0 && begin < end && end <= size());
        for (; end < size(); ++begin, ++end) {
            setPtr(begin, getPtr(end));
            setKey(begin, getKey(end));
        }
        set(Node::SIZE, begin);
    }
    // erase entry with index begin
    void erase(unsigned begin) const {
        erase(begin, begin + 1);
    }
    // find the index of the first entry with key larger than val
    unsigned upper_bound(PValue val) const {
        unsigned L = 0, R = size();
        while (L < R) {
            unsigned M = L + (R - L) / 2;
            PValue vmid = getKey(M);
            if (*val < *vmid) R = M;
            else L = M + 1;
        }
        return L;
    }
    // find the index of the first entry with key equal or larger than val
    unsigned lower_bound(PValue val) const {
        unsigned L = 0, R = size();
        while (L < R) {
            unsigned M = L + (R - L) / 2;
            PValue vmid = getKey(M);
            if (!(*vmid < *val)) R = M;
            else L = M + 1;
        }
        return L;
    }
    // find the child val is in, -1 means not found
    unsigned find(PValue val) const {
        return upper_bound(val) - 1;
    }
    // insert (val, ptr) into node
    // returns whether successful
    bool insert(PValue val, unsigned ptr) const;
    bool insert(pair<PValue, unsigned> pr) const {
        return insert(pr.first, pr.second);
    }

    // split the node when inserting (val ptr) half to node des
    void split(PValue val, unsigned ptr, Node &des) const;
    // erase key from the node, returns whether the key is erased
    bool erase(PValue val) const {
        unsigned pos = find(val);
        if (pos == -1) return 0;
        PValue vtmp = getKey(pos);
        bool erased = 0;
        if (*vtmp == *val) {
            erase(pos);
            erased = 1;
        }
        return erased;
    }
    void print() const {
        #ifdef DEBUG
        debug(
            "Node %u, root: %u, leaf: %u\n",
            m_block.index(),
            mask()&Node::ROOT,
            mask()&Node::LEAF
        );
        for (unsigned i = 0; i < size(); ++i) {
            cerr<<*getKey(i)<<' '<<getPtr(i)<<endl;
        }
        debug("\n");
        #endif
    }
};

// store necessary information to describe the index file including:
// key type, root node position, empty block chain's head, number of existing blocks
class IndexHeader {
private:
    Block &m_block;
public:
    IndexHeader(Block *block): m_block(*block) {
        m_block.constData();
    }
    ~IndexHeader() {m_block.pin(false);}
    enum offset {
        TYPE, KEY_SIZE, ROOT, EMPTY_HEAD, N_BLOCKS, BEGIN
    };
    unsigned get(offset id) const {
        return m_block[(unsigned)id];
    }
    void set(offset id, unsigned x) const {
        m_block[(unsigned)id] = x;
        m_block.setDirty();
    }
    unsigned type() {return get(TYPE);}
    unsigned key_size() {return get(KEY_SIZE);}
    unsigned root() {return get(ROOT);}
    unsigned emptyHead() {return get(EMPTY_HEAD);}
    unsigned nBlocks() {return get(N_BLOCKS);}
    unsigned begin() {return get(BEGIN);}
    // initialize empty file
    void initialize(Type keyType) {
        assert(m_block.size()>=6*sizeof(int));
        set(ROOT, 1);
        set(EMPTY_HEAD, 0);
        set(N_BLOCKS, 1);
        set(TYPE, keyType.getType());
        set(KEY_SIZE, keyType.size());
        set(BEGIN, 1);
    }
    // get key type from file
    Type getKeyType() {
        return Type((attributeType)type(), key_size());
    }
};

class Index {
public:
    class Iterator;
private:
    Type m_type;
    FILE *m_file;
    pair<PValue , unsigned> insert(unsigned x, PValue val, unsigned ptr);
    bool erase(unsigned x, PValue val);
    bool adjust(unsigned x, unsigned pos);
    bool merge(unsigned p0, unsigned p1);
    Iterator upper_bound(unsigned x, PValue val);
    Iterator lower_bound(unsigned x, PValue val);
public:
    IndexHeader getHeader() const {
        return IndexHeader(Buffer::access(m_file, 0));
    }

    Block &getBlock(unsigned blockIndex, bool pinned = false) const {
        return *Buffer::access(m_file, blockIndex, pinned);
    }

    // erase block
    void eraseBlock(unsigned blockIndex) {
        Node node = getNode(blockIndex);
        IndexHeader header = getHeader();
        node.set(Node::NEXT, header.emptyHead());
        node.set(Node::MASK, Node::EMPTY);
        header.set(IndexHeader::EMPTY_HEAD, blockIndex);
    }

    // a helper method to get a node from given block
    Node getNode(Block &block) const {
        return Node(&block, m_type);
    }

    // a helper method to get a node from given block index
    Node getNode(unsigned blockIndex, bool pinned = false) const {
        return getNode(getBlock(blockIndex, pinned));
    }

    // get an empty new block's index
    unsigned getNewBlock() const {
        unsigned head = getHeader().emptyHead();
        debug("Getting new block, empty head = %u\n", head);
        unsigned ret = 0;
        if (head) {
            unsigned nxt = getNode(head).next();
            getHeader().set(IndexHeader::EMPTY_HEAD, nxt);
            ret = head;
        } else {
            ret = getHeader().nBlocks() + 1;
            getHeader().set(IndexHeader::N_BLOCKS, ret);
        }
        debug("New block = %u\n", ret);
        return ret;
    }

    // open an existing index file
    Index(const string &filepath);

    // create an empty index file by key of type type
    Index(const string &filepath, const Type &type);

    class Iterator {
    private:
        Index &index;
        unsigned i, j; // block i, position j
        Node node() const {
            return Node(Buffer::access(index.m_file, i), index.m_type);
        }
    public:
        Iterator(Index &index, unsigned i, unsigned j): index(index), i(i), j(j) {}
        Iterator &operator++() {
            if (i==0) return *this;
            Node u = node();
            if (++j >= u.size()) {
                i = u.next();
                j = 0;
            }
            return *this;
        }
        bool operator==(const Iterator &rhs) const {
            return i == rhs.i && j == rhs.j;
        }
        PValue key() const {
            return node().getKey(j);
        }
        unsigned value() const {
            return node().getPtr(j);
        }
    };

    // follow stl standard
    Iterator begin() {
        return Iterator(*this, getHeader().begin(), 0);
    }
    Iterator end() {
        return Iterator(*this, 0, 0);
    }

    // insert val into the Index
    void insert(PValue val, unsigned ptr);

    // find the last position where key is not larger than val
    // returning end() means all keys are larger than val
    Iterator upper_bound(PValue val) {
        return upper_bound(getHeader().root(), val);
    }

    // find the last position where key is less than val
    // returning end() means all keys are not larger than val
    Iterator lower_bound(PValue val) {
        return lower_bound(getHeader().root(), val);
    }

    // erase the key equal to val
    // returns the number of nodes erased
    unsigned erase(PValue val);

    // erase the key denoted by the Iterator
    // returns the number of keys erased
    unsigned erase(const Iterator &it);

    // for debug use only, print the info of the whole tree
    void print(unsigned blockIndex=0) {
        #ifdef DEBUG
        if (blockIndex == 0) {
            debug("\n======= B+ Tree ======\n");
            blockIndex = getHeader().root();
        }
        Node u = getNode(blockIndex);
        u.print();
        if (~u.mask() & Node::LEAF) {
            for (unsigned i = 0; i < u.size(); ++i) {
                assert(u.getPtr(i)!=blockIndex);
                print(u.getPtr(i));
            }
        }
        #endif
    }
};

#endif
