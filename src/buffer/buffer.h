#ifndef BUFFER_H
#define BUFFER_H

#include "../common.h"
#include <unordered_map>

class Buffer {
private:
    Block **m_blocks;
    unsigned m_size, m_block_size, m_now;
    unordered_map<string, pair<FILE *, unordered_map<unsigned, unsigned>>> m_dictionary;
    Buffer(unsigned size, unsigned block_size=4096);
    class Singleton {
    public:
        static Buffer *initBuffer();
    };
    static Buffer *inst;
    unsigned getNewBlock(FILE *file, unsigned blockIndex,
                         unsigned blockSize, bool pinned);
    Block *m_access(const string &filePath, unsigned blockIndex,
                  bool pinned = false);

public:
    ~Buffer();
    // access the blockIndex^th block in file denoted by filePath
    // each block is of size 4kB a.k.a. char[4096]
    // pin the block in buffer according to pinned
    static Block *access(const string &filePath, unsigned blockIndex,
                         bool pinned = false);
    // flush all blocks to files
    static void flush();
};

#endif
