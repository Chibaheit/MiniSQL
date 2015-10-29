#ifndef INDEX_H
#define INDEX_H

#include "../common.h"
#include "../buffer/buffer.h"
#include <utility>

class Index {
private:
    attributeDetail m_attr;
    FILE *m_file;
public:
    // open an existing index file
    Index(const string &filepath);

    // create an empty index file by key: attr
    Index(const string &filepath, const attributeDetail &attr);

    class Iterator {
        Iterator operator++();
        Iterator operator--();
        bool operator==(const Iterator &rhs) const;
        Value *key() const;
        unsigned value() const;
    };

    // follow stl standard
    Iterator begin() const;
    Iterator end() const;

    // insert val into the Index
    // returning end() means insertion failed
    Iterator insert(Value *val);

    // find the first position where key is not less than val
    // returning end() means all keys are less than val
    Iterator lower_bound(Value *val);

    // find the first position where key is larger than val
    // returning end() means all keys are not larger than val
    Iterator upper_bound(Value *val);

    // erase the key equal to val
    // returns the number of nodes erased
    unsigned erase(Value *val);

    // erase the node denoted by the Iterator
    // returns the number of nodes erased
    unsigned erase(const Iterator &it);
};

#endif