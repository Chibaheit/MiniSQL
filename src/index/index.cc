#include "index.h"
#include "../buffer/buffer.h"

unsigned Node::LEAF = 1, Node::ROOT = 2;

// create an empty index file by key of type type
Index::Index(const string &filepath, const Type &type): m_type(type) {
    m_file = Buffer::getFile(filepath);
    IndexHeader header(Buffer::access(m_file, 0, true));
    header.initialize(type);
    Node root(Buffer::access(m_file, 1), type);
    root.initialize(Node::LEAF | Node::ROOT);
}

// open an existing index file
Index::Index(const string &filepath) {
    m_file = Buffer::getFile(filepath);
    IndexHeader header(Buffer::access(m_file, 0, true));
    m_type = header.getKeyType();
}

bool Node::insert(Value *val, unsigned ptr) {
    if (size() == n) return false;
    ++size();
    for (unsigned i = size()-1; ~i; --i) {
        Value *key = getKey(i-1);
        assert(!(*val == *key) && "ERROR: inserting duplicate key!");
        if (i && *val < *key) {
            setPtr(i, getPtr(i-1));
            setKey(i, getKey(i-1));
        } else {
            setPtr(i, ptr);
            setKey(i, val);
            delete key;
            break;
        }
        delete key;
    }
    delete val;
    return true;
}

void Node::split(Value *val, unsigned ptr, Node &des) {
    assert(size() == n);
    Value *vtmp, *key = getKey(size() - 1);
    unsigned ptmp;
    des.mask() = mask() &= ~ROOT;
    if (*val < *key) {
        vtmp = key;
        ptmp = getPtr(size() - 1);
        --size();
        insert(val, ptr);
        delete val;
    } else {
        vtmp = val;
        ptmp = ptr;
        delete key;
    }
    unsigned begin = (size() + 1) / 2;
    des.size() = n + 1 - begin;
    for (unsigned i = begin; i < n; ++i) {
        des.setPtr(i - begin, getPtr(i));
        des.setKey(i - begin, getKey(i));
    }
    des.setPtr(n - begin, ptmp);
    des.setKey(n - begin, vtmp);
    des.next() = next();
    size() = begin;
    next() = des.m_block.index();
    delete vtmp;
}
