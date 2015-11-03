#include "buffer.h"
#include <cassert>

Buffer::Buffer(unsigned size, unsigned block_size):
    m_size(size), m_block_size(block_size), m_now(0) {
    m_blocks = new Block*[size];
    memset(m_blocks, 0, sizeof(Block*) * m_size);
}

Buffer *Buffer::Singleton::initBuffer() {
    debug("Initializing buffer with num blocks = %d block size = %d ...",
          defaultNumBlocks, defaultBlockSize);
    Buffer *buffer = new Buffer(defaultNumBlocks, defaultBlockSize);
    debug("OK\n");
    return buffer;
}

Buffer *Buffer::inst = Buffer::Singleton::initBuffer();

inline unsigned nxt(unsigned &x, unsigned n){
    unsigned t = x;
    x = x + 1 == n ? 0 : x + 1;
    return t;
}

unsigned Buffer::getNewBlock(FILE *file, unsigned offset,
                     unsigned blockSize, bool pinned) {
    for(;1; nxt(m_now, m_size)) {
        Block *&block = m_blocks[m_now];
        if (block == NULL) {
            debug("Empty block %d found\n", m_now);
            block = new Block(file, offset, blockSize);
            block->pin(pinned);
            return nxt(m_now, m_size);
        } else if (block->isRecent()) {
            //debug("Block %d is not recent any more.\n", m_now);
            block->setRecent(0);
        } else if (!block->isPinned()) {
            debug("Replace block %d.\n", m_now);
            block->open(file, offset, blockSize);
            block->pin(pinned);
            return nxt(m_now, m_size);
        }
    }
    assert(0);
};

Block *Buffer::access(const string &filePath,
                      unsigned blockIndex, bool pinned) {
    debug("Accessing \"%s\" %d\n", filePath.c_str(), blockIndex);
    FILE *fp = inst->getFileHandle(filePath);
    unsigned offset = blockIndex * inst->m_block_size;
    return inst->m_access(fp, offset, pinned);
}

Block *Buffer::access(FILE *file, unsigned blockIndex, bool pinned) {
    unsigned offset = blockIndex * inst->m_block_size;
    return inst->m_access(file, offset, pinned);
}

FILE *Buffer::getFileHandle(const string &filePath) {
    auto file = m_files.find(filePath);
    FILE *fp = NULL;
    if (file == m_files.end()) {
        if ((fp = fopen(filePath.c_str(), "rb+")) == NULL) {
            fp = fopen(filePath.c_str(), "wb+");
        }
        debug("File %s: %p\n", filePath.c_str(), fp);
        m_files[filePath] = fp;
        m_dictionary[fp];
    } else fp = file->second;
    assert(fp);
    return fp;
}

FILE *Buffer::getFile(const string &filePath) {
    return inst->getFileHandle(filePath);
}

Block *Buffer::m_access(FILE *fp, unsigned offset, bool pinned) {
    auto &table = m_dictionary.find(fp)->second;
    auto entry = table.find(offset);
    if (entry == table.end()) {
        auto tmp = table.insert(make_pair(
            offset,
            getNewBlock(
                fp,
                offset,
                m_block_size,
                pinned
            )
        ));
        assert(tmp.second);
        entry = tmp.first;
    }

    Block *block = m_blocks[entry->second];
    block->pin(pinned);

    inst->printOpenedBlocks();

    return block;
}

void Buffer::flush() {
    delete inst;
    Singleton::initBuffer();
}

Buffer::~Buffer() {
    debug("Buffer deconstructing...\n");
    for (int i = 0; i < m_size; ++i)
        if (m_blocks[i]) delete m_blocks[i];
    delete [] m_blocks;
    for (auto &file: m_files) {
        fclose(file.second);
    }
    debug("Buffer deconstructed.\n");
}

void Block::open(FILE *file, unsigned offset, unsigned size) {
    if (m_file) {
        writeBack();
        debug("%p %u\n", m_file, m_offset);
        assert(Buffer::inst->m_dictionary[m_file].erase(m_offset));
    }
	m_file = file;
	m_offset = offset;
	m_size = size;
	m_recent = true;
	m_dirty = m_pinned = false;
	assert(fseek(file, offset, SEEK_SET)==0);
	fread(m_data, 1, size, file);
}
