#include "index.h"
#include "../buffer/buffer.h"

unsigned Node::LEAF = 1, Node::ROOT = 2, Node::EMPTY = 4;

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

// returns whether insert operation failed
bool Node::insert(PValue val, unsigned ptr) const {
    if (size() == n) return false;
    ++size();
    for (unsigned i = size()-1; ~i; --i) {
        PValue key = getKey(i-1);
        assert(!(*val == *key) && "ERROR: inserting duplicate key!");
        if (i && *val < *key) {
            setPtr(i, getPtr(i-1));
            setKey(i, getKey(i-1));
        } else {
            setPtr(i, ptr);
            setKey(i, val);
            break;
        }
    }
    return true;
}

void Node::split(PValue val, unsigned ptr, Node &des) const {
    assert(size() == n);
    PValue vtmp, key = getKey(size() - 1);
    unsigned ptmp;
    if (*val < *key) {
        vtmp = key;
        ptmp = getPtr(size() - 1);
        --size();
        insert(val, ptr);
    } else {
        vtmp = val;
        ptmp = ptr;
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
void Index::insert(PValue val, unsigned ptr) {
    insert(getHeader().root(), val, ptr);
}

pair<PValue , unsigned> Index::insert(unsigned x, PValue val, unsigned ptr) {
    Node node = getNode(x);
    pair<PValue , unsigned> ret = make_pair(PValue(NULL), 0);
    if (~node.mask() & Node::LEAF) {
        unsigned pos = node.find(val);
        assert(~pos);
        auto tmp = insert(node.getPtr(pos), val, ptr);
        if (tmp.second) {
            val = tmp.first;
            ptr = tmp.second;
        } else return ret;
    }
    Node u = getNode(x, true);
    if (!node.insert(val, ptr)) {
        unsigned y = getNewBlock();
        Node v = getNode(y, true);
        bool root = false;
        if (u.mask() & Node::ROOT) {
            u.mask() ^= Node::ROOT;
            root = true;
        }
        v.initialize(u.mask());
        u.split(val, ptr, v);
        if (root) {
            unsigned z = getNewBlock();
            Node w = getNode(z);
            w.initialize(Node::ROOT);
            w.insert(u.getKey(0), x);
            w.insert(v.getKey(0), y);
        } else ret = make_pair(v.getKey(0), y);
        v.m_block.pin(false);
    }
    u.m_block.pin(false);
    return ret;
}

Index::Iterator Index::upper_bound(unsigned x, PValue val) {
    Node node = getNode(x);
    unsigned pos = node.find(val);
    if (pos == -1) return Iterator(*this, 0, 0);
    if (node.mask() & Node::LEAF) return Iterator(*this, x, pos);
    return upper_bound(node.getPtr(pos), val);
}

Index::Iterator Index::lower_bound(unsigned x, PValue val) {
    Node node = getNode(x);
    unsigned pos = node.lower_bound(val) - 1;
    if (pos == -1) return Iterator(*this, 0, 0);
    if (node.mask() & Node::LEAF) return Iterator(*this, x, pos);
    return lower_bound(node.getPtr(pos), val);
}

// returns whether additional adjustments needed
bool Index::erase(unsigned x, PValue val) {
    Node node = getNode(x);
    unsigned pos = node.find(val);
    bool ret = false;
    if (pos == -1) return ret;
    if (node.mask() & Node::LEAF) {
        PValue key = node.getKey(pos);
        if (*key == *val) {
            node.erase(pos);
            ret = true;
        }
    } else {
        bool tmp = erase(node.getPtr(pos), val);
        if (tmp) {
            ret = adjust(x, pos);
        }
    }
    return ret;
}

// returns whether additional adjust need to take place
bool Index::adjust(unsigned x, unsigned pos) {
    Node u = getNode(x);
    Node v = getNode(u.getPtr(pos));
    PValue orig_key = u.getKey(0);
    bool ret = false;
    if (v.size() < (v.num()+1)/2) {
        unsigned pos2;
        if (pos) pos2 = pos--;
        else pos2 = pos + 1;
        unsigned v_id = u.getPtr(pos), w_id = u.getPtr(pos2);
        Node v = getNode(v_id);
        Node w = getNode(w_id);
        if (merge(pos, pos2)) {
            u.erase(pos2);
            u.setKey(pos, v.getKey(0));
            ret = true;
            if ((u.mask() & Node::ROOT) && u.size() <= 1) {
                eraseBlock(x);
                v.mask() |= Node::ROOT;
                getHeader().root() = x;
                ret = false;
            }
        } else {
            u.setKey(pos, v.getKey(0));
            u.setKey(pos2, w.getKey(0));
        }
    } else {
        u.setKey(pos, v.getKey(0));
    }
    if (!(*orig_key == *u.getKey(0))) ret = true;
    return ret;
}

bool Index::merge(unsigned p0, unsigned p1) {
    Node u = getNode(p0);
    Node v = getNode(p1);
    if (u.size() + v.size() <= u.num()) {
        for (unsigned i = 0; i < v.size(); ++i) {
            u.insert(v.getPair(i));
        }
        eraseBlock(v.m_block.index());
        return true;
    } else {
        unsigned lnum = (u.size() + v.size() + 1) / 2;
        if (u.size() > lnum) {
            v.shiftRight(0, u.size() - lnum);
            for (unsigned i = lnum; i < u.size(); ++i) {
                v.setPair(i - lnum, u.getPair(i));
            }
            u.erase(lnum, u.size());
        } else { //u.size() <= lnum
            unsigned offset = u.size();
            for (unsigned i = u.size(); i < lnum; ++i) {
                u.insert(v.getPair(i - offset));
            }
            v.erase(0, lnum - offset);
        }
        return false;
    }
}
