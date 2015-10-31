#ifndef INDEX_H
#define INDEX_H

#include "../common.h"
#include "../buffer/buffer.h"
#include <utility>

const int szInt = sizeof(int);

class Node {
private:
    Block &m_block;
    Type m_key_type;
    unsigned n, m_unit;
    void init() {
        m_unit = m_key_type.size() + szInt;
        n = (m_block.size() - szInt*3) / m_unit;
        assert(n >= 3);
    }
public:
    static unsigned LEAF, ROOT;
    // open a Node Block
    Node(Block *block, Type key_type):
        m_block(*block), m_key_type(key_type) {
        init();
    }
    // initialize an empty Node Block
    void initialize(unsigned _mask) {
        mask() = _mask;
        size() = 0;
        setPtr(n, 0);
        m_block.setDirty();
    }
    unsigned num() const {return n;}
    unsigned &mask() const {return m_block[-1];}
    unsigned &size() const {return m_block[-2];}
    unsigned &next() const {return m_block[-3];}
    // get the ith key
    Value *getKey(int i) const {
        assert(i>=0 && i<size());
        return m_key_type.create(
            m_block.constData() + m_unit * i + szInt
        );
    }
    // set the ith key
    void setKey(int i, Value *val) const {
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
    // insert (val, ptr) into node
    // returns whether successful
    bool insert(Value *val, unsigned ptr);
    // split the node when inserting (val ptr) half to node des
    void split(Value *val, unsigned ptr, Node &des);
};

// store necessary information to describe the index file including:
// key type, root node position, empty block chain's head, number of existing blocks
class IndexHeader {
private:
    Block &m_block;
public:
    IndexHeader(Block *block): m_block(*block) {}
    ~IndexHeader() {m_block.pin(false);}
    unsigned &type() {return m_block[0];}
    unsigned &key_size() {return m_block[1];}
    unsigned &root() {return m_block[2];}
    unsigned &emptyHead() {return m_block[3];}
    unsigned &nBlocks() {return m_block[4];}
    unsigned &begin() {return m_block[5];}
    // initialize empty file
    void initialize(Type keyType) {
        root() = 1;
        emptyHead() = 0;
        nBlocks() = 2;
        type() = keyType.getType();
        key_size() = keyType.size();
        begin() = 1;
        m_block.setDirty();
    }
    // get key type from file
    Type getKeyType() {
        return Type((attributeType)type(), key_size());
    }
};

class Index {
private:
    Type m_type;
    FILE *m_file;
public:
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
        Value *key() const {
            return node().getKey(j);
        }
        unsigned value() const {
            return node().getPtr(j);
        }
    };

    // follow stl standard
    Iterator begin() const;
    Iterator end() const;

    // insert val into the Index
    // returning end() means insertion failed
    Iterator insert(Value *val);

    // find the first position where key is not less than val
    // returning end() means all keys are less than val
    Iterator lower_bound(Value *val);

    // find the first position where key is larger than val
    // returning end() means all keys are not larger than val
    Iterator upper_bound(Value *val);

    // erase the key equal to val
    // returns the number of nodes erased
    unsigned erase(Value *val);

    // erase the node denoted by the Iterator
    // returns the number of nodes erased
    unsigned erase(const Iterator &it);
};

#endif
