#ifndef INDEX_H
#define INDEX_H

#include "../common.h"
#include "../buffer/buffer.h"
#include <utility>

const int szInt = sizeof(int);

class Node {
private:
    Block *m_block;
    Type m_key_type;
    bool isLeaf;
    unsigned n, unitSize;
    void init() {
        unsigned sz = block->size();
        unsigned skey = m_key_type->size();
        isLeaf = block->constData()[sz - 1];
        unitSize = skey + szInt;
        n = (m_block->size() - szInt - 1) / unitSize;
    }
public:
    // open an existing Node Block
    Node(Block *block, Type key_type):
        m_block(block), m_key_type(key_type) {
        init();
    }
    // initialize a new Node Block
    Node(Block *block, Type key_type, bool isLeaf):
        m_block(block), m_key_type(key_type), isLeaf(isLeaf) {
        block->data()[sz - 1] = isLeaf;
        *(unsigned *)block->data() = 0;
        init();
        unitSize = m_key_type->size() + szInt;
    }
    Value *getKey(int i) const {
        assert(i>=0 && i<n);
        return m_key_type.create(
            m_block->constData() + unitSize * i + szInt;
        );
    }
    unsigned getPtr(int i) const {
        assert(i>=0 && i<=n);
        const char *mem = m_block->constData() + unitSize * i;
        return *(unsigned *)mem;
    }
};

class Index {
private:
    attributeDetail m_attr;
    FILE *m_file;
public:
    // open an existing index file
    Index(const string &filepath);

    // create an empty index file by key: attr
    Index(const string &filepath, const attributeDetail &attr);

    class Iterator {
        Iterator operator++();
        bool operator==(const Iterator &rhs) const;
        Value *key() const;
        unsigned value() const;
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
