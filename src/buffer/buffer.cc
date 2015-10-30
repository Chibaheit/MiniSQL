#include "buffer.h"
#include <cassert>

Buffer::Buffer(unsigned size, unsigned block_size):
    m_size(size), m_block_size(block_size), m_now(0) {
    m_blocks = new Block*[size];
    memset(m_blocks, 0, sizeof(Block*) * m_size);
}

Buffer *Buffer::Singleton::initBuffer() {
    return new Buffer(1024, 4096);
}

Buffer *Buffer::inst = Buffer::Singleton::initBuffer();

inline unsigned nxt(unsigned &x, unsigned n){
    unsigned t = x;
    x = x + 1 == n ? 0 : x + 1;
    return t;
}

unsigned Buffer::getNewBlock(FILE *file, unsigned blockIndex,
                     unsigned blockSize, bool pinned) {
    for(;; nxt(m_now, m_size)) {
        Block *&block = m_blocks[m_now];
        if (block == NULL) {
            block = new Block(file, blockIndex, blockSize);
            block->pin(pinned);
            return nxt(m_now, m_size);
        } else if (block->isRecent()) {
            block->setRecent(0);
        } else if (!block->isPinned()) {
            block->open(file, blockIndex, blockSize);
            return nxt(m_now, m_size);
        }
    }
};

Block *Buffer::access(const string &filePath,
                      unsigned blockIndex, bool pinned) {
    return inst->m_access(filePath, blockIndex, pinned);
}

Block *Buffer::m_access(const string &filePath,
                      unsigned blockIndex, bool pinned) {
    auto file = m_dictionary.find(filePath);
    if (file == m_dictionary.end()) {
        FILE *fp = fopen(filePath.c_str(), "rb+");
        if (fp == NULL) {
            assert(fp = fopen(filePath.c_str(), "wb+"));
        }
        m_dictionary[filePath].first = fp;
        file = m_dictionary.find(filePath);
    }

    auto &table = file->second.second;
    auto entry = table.find(blockIndex);
    if (entry == table.end()) {
        table[blockIndex] = getNewBlock(file->second.first, blockIndex,
                                        m_block_size, pinned);
        entry = table.find(blockIndex);
    }

    Block *block = m_blocks[entry->second];
    block->pin(pinned);
    return block;
}

void Buffer::flush() {
    delete inst;
    Singleton::initBuffer();
}

Buffer::~Buffer() {
    for (int i = 0; i < m_size; ++i)
        if (m_blocks[i]) delete m_blocks[i];
    delete [] m_blocks;
    for (auto &file: m_dictionary) {
        fclose(file.second.first);
    }
}
