#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <memory>

using namespace std;

#define DEBUG
#ifdef DEBUG
    #define debug(args...) fprintf(stderr, args)
#else
    #define debug(args...)
#endif

typedef unsigned int Size;
typedef vector<Size> Sizes;
typedef vector<string>& Strings;

enum attributeType {
    INTTYPE,
    CHARTYPE,
    FLOATTYPE
};

enum queryType {
    LESS,
    LESSANDEQUAL,
    EQUAL,
    NOTEQUAL,
    MOREANDEQUAL,
    MORE
};

class Schema{
public:
    Strings attributeName;
    vector<attributeType> attrType;
    vector<Size> attributeSize;
    Schema();
};

class Attribute{
public:
    string value;
    attributeType type;
    Attribute();
};

class Table{
public:
    vector<Attribute> attributes;
    Table();
};

class attributeDetail {
public:
    attributeType type;
    int length;
    bool unique;
    bool primary;
    attributeDetail(attributeType type, int length = 0, bool unique = false, bool primary = false) : type(type), length(length), unique(unique), primary(primary) {};
    /* type: attribute type
     * length: if type == CHARTYPE, it must have length
     * unique: it is unique or not
     * primary: it is primary key or not
     * attribute type
     * attributeDetail: constructor
    */
};

class Value {
public:
    virtual ~Value();
    virtual bool operator==(const Value &rhs) const=0;
    virtual bool operator<(const Value &rhs) const=0;
    virtual void memoryCopy(void *dest) const=0;
	virtual Size size() const=0;
	virtual void print(ostream &out) const=0;
};

ostream &operator<<(ostream &out, const Value &val);

template<class T>
class SingleValue: public Value {
private:
	T data;
public:
	SingleValue(T x): data(x) {}
    SingleValue(const char *mem): data(*(const T*)mem) {}
	virtual ~SingleValue();
	virtual bool operator==(const Value &rhs) const;
	virtual bool operator<(const Value &rhs) const;
	virtual Size size() const;
    virtual void memoryCopy(void *dest) const;
	virtual void print(ostream &out) const;
};
typedef SingleValue<int> Int;
typedef SingleValue<float> Float;
typedef SingleValue<string> String;
typedef shared_ptr<Value> PValue;

class Type {
private:
    attributeType m_type;
    unsigned m_size;
public:
    static Type intType, floatType;
    Type(attributeType type, unsigned size): m_type(type), m_size(size) {}
    Type(): m_size(-1) {}
    bool isValid() const {
        return ~m_size;
    }
    attributeType getType() const {
        assert(isValid());
        return m_type;
    }
    // create an instance of type from memory
    Value *create(const char *mem) const {
        assert(isValid());
        switch (m_type) {
            case INTTYPE: return new Int(mem);
            case FLOATTYPE: return new Float(mem);
            case CHARTYPE: return new String(mem);
            default: assert(0 && "unknown type");
        }
    }
    // the number of bytes
    unsigned size() const {
        return m_size;
    }
};

class valueDetail {
public:
    attributeType type;
    string value;
    valueDetail(attributeType type, string value) : type(type), value(value) {};
    /* type: value type
     * value: all the value saved as string
     * valueDetail: constructor
    */
};

class queryDetail {
public:
    queryType type;
    valueDetail val;
    int targetPosition;
    queryDetail(queryType type, valueDetail val) : type(type), val(val) {};
    /* type: query type
     * val: the value of the restraint
     * targetPosition: the attribute position in the table.
     * queryDetail: constructor
    */
};

#endif
