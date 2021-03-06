#include "../src/buffer/buffer.h"
#include <cassert>
#define NUM 5

int defaultNumBlocks = 2, defaultBlockSize = 4096;

const char *s[] = {
    "test.txt",
    "test2.txt",
    "test3.txt"
};

char data[1123];
void testFile() {
    FILE *fp = fopen("test.txt", "rb+");
    assert(fseek(fp, 100, SEEK_SET)==0);
    //fwrite(s[0], 1, sizeof("test.txt"), fp);
    fread(data, 1, 100, fp);
    printf("data: %s\n", data);
    fclose(fp);
}
int main() {
    //testFile();
    for(int i=0; i<NUM; ++i) {
        debug("Access %d: ", i);
        Block *block = Buffer::access(s[i%3], i);
        debug("OK!\n");
        char *p = block->data();
        debug("Address = %p\n", p);
        fill(p, p+defaultBlockSize, 'z');
        p[i]=i+'0';
    }
    debug("\n");
    Buffer::flush();

    for(int i=0; i<NUM; ++i) {
        Block *block = Buffer::access(s[i%3], i);
        const char *p = block->constData();
        debug("Address = %p\n", p);
        assert(p[i]==i+'0');
    }
    Buffer::flush();

    debug("\n");
    for(int i=0; i<NUM; ++i) {
        Block *block = Buffer::access(s[i%3], i);
        const char *p = block->constData();
        assert(p[i]==i+'0');
    }
    assert(Buffer::access(s[0], NUM)->constData()[0]==0);
    Buffer::flush();
    printf("Buffer: OK!\n\n");
}
