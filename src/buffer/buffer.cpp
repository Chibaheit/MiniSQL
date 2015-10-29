#include "buffer.h"

Buffer *Buffer::Singleton::initBuffer(){
    return new Buffer(1024, 4096);
}

Buffer *Buffer::instance = Buffer::Singleton::initBuffer();