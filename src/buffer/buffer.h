#ifndef BUFFER_H
#define BUFFER_H

#include "../common.h"
#include <unordered_map>

// debug finished
#ifdef DEBUG
    #undef DEBUG
    #undef debug
    #define debug(args...)
#endif

// defaultNumBlocks: the number of blocks in buffer, at least 1
// defaultBlockSize: the size of a single block, 4096 for production use
extern int defaultNumBlocks, defaultBlockSize;

class Buffer;
class Block {
private:
    Size m_size, m_offset;
    char *m_data;
    FILE *m_file;
    bool m_dirty, m_pinned, m_recent;
    bool writeBack() {
        if (m_dirty) {
            assert(fseek(m_file, m_offset, SEEK_SET)==0);
            fwrite(m_data, 1, m_size, m_file);
            m_dirty = 0;
            return 1;
        }
        return 0;
    }
    void open(FILE *file, unsigned offset, unsigned size);
    Block(FILE *file, Size offset, Size size): m_dirty(0), m_file(0) {
        assert(file);
        debug("Opening file with offset %u size %u\n", offset, size);
        m_data = new char[size];
        assert(m_data);
        #ifdef DEBUG
        memset(m_data, 'z', size);
        strcpy(m_data, "Hello, World!");
        #endif
        open(file, offset, size);
    }
    void setRecent(bool recent) {
        m_recent = recent;
    }
    bool isRecent() const {
        return m_recent;
    }
    friend Buffer;
public:
    // returns block index
    unsigned index() const {
        return m_offset / m_size;
    }
    // returns block size, default is 4096
    Size size() const {
        return m_size;
    }
    // retrieve constant pointer to data
    const char *constData() {
        m_recent = true;
        return m_data;
    }
    // retrieve pointer to data
    char *data() {
        m_recent = m_dirty = true;
        return m_data;
    }
    void setDirty() {
        m_recent = m_dirty = true;
    }
    // get integer reference at pos
    // negative pos is allowed and treated as indexing from the end
    // Note this method set neither the m_recent nor m_dirty flag
    // set them manually calling setDirty()
    unsigned &operator[](int pos) {
        char *addr = m_data + pos * sizeof(int);
        if (pos < 0) addr += m_size;
        assert(m_data <= addr);
        assert(addr <= m_data + size() - sizeof(int));
        return *(unsigned *)addr;
    }
    // automatic write back when destroyed
    ~Block() {
        debug("Block %p %u deconstructing...\n", m_file, m_offset);
        writeBack();
        delete [] m_data;
        debug("Block %p %u deconstructed.\n", m_file, m_offset);
    }
    void pin(bool pinned) {
        m_pinned = pinned;
    }
    // check whether the block is pinned in the buffer
    bool isPinned() const {
        return m_pinned;
    }
};

class Buffer {
private:
    Block **m_blocks;
    unsigned m_size, m_block_size, m_now;
    unordered_map<FILE *, unordered_map<unsigned, unsigned>> m_dictionary;
    unordered_map<string, FILE *> m_files;
    Buffer(unsigned size, unsigned block_size);
    class Singleton {
    public:
        static Buffer *initBuffer();
    };
    static Buffer *inst;
    unsigned getNewBlock(FILE *file, unsigned offset,
                         unsigned blockSize, bool pinned);
    Block *m_access(FILE *file, unsigned offset, bool pinned = false);
    void printOpenedBlocks() const {
        #ifdef DEBUG
        for (auto &u: m_dictionary) {
            debug("File %p:", u.first);
            for (auto &v: u.second) {
                debug(" (%u %u)", v.first, v.second);
            }
            debug("\n");
        }
        debug("\n");
        #endif
    }
    friend Block;
    // get file handle from buffer
    FILE *getFileHandle(const string &filePath);

public:
    ~Buffer();
    // access the blockIndex^th block in file denoted by filePath
    // each block is of size BLOCK_SIZE bytes a.k.a. char[BLOCK_SIZE]
    // pin the block in buffer according to pinned
    static Block *access(const string &filePath, unsigned blockIndex,
                         bool pinned = false);
    // identify file by FILE *file instead of filePath
    // use getFilehandle method to get this file
    static Block *access(FILE *file, unsigned blockIndex, bool pinned = false);
    // static wrapper function
    static FILE *getFile(const string &filePath);
    // flush all blocks to files
    static void flush();
};

#endif
