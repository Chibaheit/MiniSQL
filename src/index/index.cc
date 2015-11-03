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

bool Node::insert(Value *val, unsigned ptr) const {
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
    }
    return true;
}

void Node::split(Value *val, unsigned ptr, Node &des) const {
    assert(size() == n);
    Value *vtmp, *key = getKey(size() - 1);
    unsigned ptmp;
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
}

// insert val into the Index
// returning end() means insertion failed
void Index::insert(Value *val, unsigned ptr) {
    insert(getHeader().root(), val, ptr);
}

pair<Value *, unsigned> Index::insert(unsigned x, Value *val, unsigned ptr) {
    Node node = getNode(getBlock(x));
    pair<Value *, unsigned> ret = make_pair((Value *)NULL, 0);
    if (~node.mask() & Node::LEAF) {
        unsigned pos = node.find(val);
        assert(~pos);
        auto tmp = insert(node.getPtr(pos), val, ptr);
        if (tmp.second) {
            val = tmp.first;
            ptr = tmp.second;
        } else return ret;
    }
    Node u = getNode(getBlock(x, true));
    if (!node.insert(val, ptr)) {
        unsigned y = getNewBlock();
        Node v = getNode(getBlock(y, true));
        bool root = false;
        if (u.mask() & Node::ROOT) {
            u.mask() ^= Node::ROOT;
            root = true;
        }
        v.initialize(u.mask());
        u.split(val, ptr, v);
        if (root) {
            unsigned z = getNewBlock();
            Node w = getNode(getBlock(z));
            w.initialize(Node::ROOT);
            w.insert(u.getKey(0), x);
            w.insert(v.getKey(0), y);
        } else ret = make_pair(v.getKey(0), y);
        v.m_block.pin(false);
    }
    u.m_block.pin(false);
    return ret;
}

Index::Iterator Index::upper_bound(unsigned x, Value *val) {
    Node node = getNode(getBlock(x));
    unsigned pos = node.find(val);
    if (pos == -1) return Iterator(*this, 0, 0);
    if (node.mask() & Node::LEAF) return Iterator(*this, x, pos);
    return upper_bound(node.getPtr(pos), val);
}

Index::Iterator Index::lower_bound(unsigned x, Value *val) {
    Node node = getNode(getBlock(x));
    unsigned pos = node.lower_bound(val) - 1;
    if (pos == -1) return Iterator(*this, 0, 0);
    if (node.mask() & Node::LEAF) return Iterator(*this, x, pos);
    return lower_bound(node.getPtr(pos), val);
}