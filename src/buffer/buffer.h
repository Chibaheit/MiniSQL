#include "../common.h"
#include <unordered_map>

class Buffer{
private:
    Block **m_blocks;
    unsigned m_size, m_block_size, m_now;
    unordered_map<string, unordered_map<unsigned, unsigned>> m_dictionary;
    Buffer(unsigned size, unsigned block_size=4096):
        m_size(size), m_block_size(block_size), m_now(0) {
        m_blocks = new Block*[size];
        memset(m_blocks, 0, sizeof(Block*) * m_size);
    }
    class Singleton {
    public:
        static Buffer *initBuffer();
    };

public:
    static Buffer *buffer;
    ~Buffer() {
        for (int i = 0; i < m_size; ++i)
            if (m_blocks[i]) delete m_blocks[i];
        delete [] m_blocks;
    }
    Block *access(const string &filePath, unsigned blockIndex);
};
