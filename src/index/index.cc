#include "index.h"
#include "../buffer/buffer.h"

unsigned Node::LEAF = 1, Node::ROOT = 2, Node::EMPTY = 4;

// create an empty index file by key of type type
Index::Index(const string &filepath, const Type &type): m_type(type) {
    debug("Creating an index on %s with type size = %u\n", filepath.c_str(), type.size());
    m_file = Buffer::getFile(filepath);
    IndexHeader header(Buffer::access(m_file, 0, true));
    header.initialize(type);
    Node root = getNode(1);
    root.initialize(Node::LEAF | Node::ROOT);
    debug("Number of children = %u\n", root.num());
    debug("Index created.\n");
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
    set(SIZE, size() + 1);
    for (unsigned i = size()-1; ~i; --i) {
        assert(!i || !(*val == *getKey(i-1)) && "ERROR: inserting duplicate key!");
        if (i && *val < *getKey(i-1)) {
            setPair(i, getPair(i-1));
        } else {
            setPair(i, make_pair(val, ptr));
            break;
        }
    }
    return true;
}

//#define DEBUG_NODE_SPLIT
void Node::split(PValue val, unsigned ptr, Node &des) const {
#ifdef DEBUG_NODE_SPLIT
    debug("\nBefore splitting:\n");
    print();
#endif
    assert(size() == n);
    PValue vtmp = NULL, key = getKey(size() - 1);
    unsigned ptmp;
    if (*val < *key) {
        vtmp = key;
        ptmp = getPtr(size() - 1);
        set(SIZE, size() - 1);
        insert(val, ptr);
    } else {
        vtmp = val;
        ptmp = ptr;
    }
    unsigned begin = n / 2 + 1;
    des.set(SIZE, n + 1 - begin);
    for (unsigned i = begin; i < n; ++i) {
        des.setPtr(i - begin, getPtr(i));
        des.setKey(i - begin, getKey(i));
    }
    des.setPtr(n - begin, ptmp);
    des.setKey(n - begin, vtmp);
    des.set(NEXT, next());
    set(SIZE, begin);
    set(NEXT, des.m_block.index());
#ifdef DEBUG_NODE_SPLIT
    debug("After splitting:\n");
    print();
    des.print();
#endif
}

// insert val into the Index
// returns whether insertion is successful (There isn't a duplicate key)
bool Index::insert(PValue val, unsigned ptr) {
    bool ret = false;
    insert(getHeader().root(), val, ptr, ret);
    return ret;
}

pair<PValue , unsigned> Index::insert(unsigned x, PValue val, unsigned ptr, bool &success) {
    Node node = getNode(x);
    #ifdef DEBUG_INDEX_INSERT
        cerr<<"insert("<<x<<", "<<*val<<", "<<ptr<<")"<<endl;
        node.print();
    #endif
    pair<PValue , unsigned> ret = make_pair(PValue(NULL), 0);
    if (~node.mask() & Node::LEAF) {
        unsigned pos = node.find(val);
        if (pos == -1) {
            pos = 0;
            node.setKey(0, val);
        }
        auto tmp = insert(node.getPtr(pos), val, ptr, success);
        if (tmp.second) {
            val = tmp.first;
            ptr = tmp.second;
        } else return ret;
    } else {
        unsigned pos = node.find(val);
        if (~pos && *node.getKey(pos) == *val) {
            success = false;
            return ret;
        } else success = true;
    }
    Node u = getNode(x, true);
    if (!u.insert(val, ptr)) {
        unsigned y = getNewBlock();
        Node v = getNode(y, true);
        bool root = false;
        if (u.mask() & Node::ROOT) {
            u.set(Node::MASK, u.mask() & ~Node::ROOT);
            root = true;
        }
        v.initialize(u.mask());
        u.split(val, ptr, v);
        if (root) {
            unsigned z = getNewBlock();
            Node w = getNode(z, true);
            w.initialize(Node::ROOT);
            getHeader().set(IndexHeader::ROOT, z);
            w.insert(u.getKey(0), x);
            w.insert(v.getKey(0), y);
            w.m_block.pin(false);
        } else ret = make_pair(v.getKey(0), y);
        v.m_block.pin(false);
    }
    u.m_block.pin(false);
    #ifdef DEBUG_INDEX_INSERT
    if (ret.second) {
        cerr<<"RET: "<<*ret.first<<' '<<ret.second<<endl;
    }
    #endif
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

//#define DEBUG_INDEX_ERASE
// returns whether additional adjustments needed
// success returns whether erasing succeeded
bool Index::erase(unsigned x, PValue val, bool &success) {
    Node node = getNode(x);
    #ifdef DEBUG_INDEX_ERASE
        cerr<<"erase("<<x<<", "<<*val<<")"<<endl;
        node.print();
    #endif
    unsigned pos = node.find(val);
    bool ret = false;
    if (pos == -1) return ret;
    if (node.mask() & Node::LEAF) {
        if (*node.getKey(pos) == *val) {
            node.erase(pos);
            ret = true;
            success = true;
        } else success = false;
    } else {
        bool tmp = erase(node.getPtr(pos), val, success);
        if (tmp) {
            ret = adjust(x, pos);
        }
    }
    #ifdef DEBUG_INDEX_ERASE
    cerr<<"RET: "<<ret<<endl;
    node.print();
    #endif
    return ret;
}

// returns whether additional adjust need to take place
bool Index::adjust(unsigned x, unsigned pos) {
    Node u = getNode(x, true);
    Node v = getNode(u.getPtr(pos));
    PValue orig_key = u.getKey(0);
    bool ret = false;
    if (v.size() < (v.num()+1)/2) {
        unsigned pos2;
        if (pos) pos2 = pos--;
        else pos2 = pos + 1;
        unsigned v_id = u.getPtr(pos), w_id = u.getPtr(pos2);
        Node v = getNode(v_id, true);
        Node w = getNode(w_id, true);
        if (merge(v_id, w_id)) {
            v.m_block.pin(true);
            u.erase(pos2);
            u.setKey(pos, v.getKey(0));
            ret = true;
            if ((u.mask() & Node::ROOT) && u.size() <= 1) {
                eraseBlock(x);
                v.set(Node::MASK, v.mask() | Node::ROOT);
                getHeader().set(IndexHeader::ROOT, v_id);
                ret = false;
            }
            v.m_block.pin(false);
        } else {
            u.setKey(pos, v.getKey(0));
            u.setKey(pos2, w.getKey(0));
            v.m_block.pin(false);
            w.m_block.pin(false);
        }
        u.m_block.pin(false);
    } else {
        u.setKey(pos, v.getKey(0));
        u.m_block.pin(false);
    }
    if (!(*orig_key == *u.getKey(0))) ret = true;
    return ret;
}

//#define DEBUG_INDEX_MERGE
// returns whether merging two blocks into one was successful
bool Index::merge(unsigned p0, unsigned p1) {
    Node u = getNode(p0, true);
    Node v = getNode(p1, true);
#ifdef DEBUG_INDEX_MERGE
    debug("Before merge:\n");
    u.print();
    v.print();
#endif
    if (u.size() + v.size() <= u.num()) {
        for (unsigned i = 0; i < v.size(); ++i) {
            u.insert(v.getPair(i));
        }
        u.set(Node::NEXT, v.next());
        v.m_block.pin(false);
        eraseBlock(v.m_block.index());
    #ifdef DEBUG_INDEX_MERGE
        debug("After merge:\n");
        u.print();
    #endif
        u.m_block.pin(false);
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
    #ifdef DEBUG_INDEX_MERGE
        debug("After merge:\n");
        u.print();
        v.print();
    #endif
        u.m_block.pin(false);
        v.m_block.pin(false);
        return false;
    }
}
