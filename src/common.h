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

class AttributeDetail {
public:
    string name;
    attributeType type;
    int length;
    bool unique;
    bool primary;
    AttributeDetail(string name, attributeType type, int length = 0, bool unique = false, bool primary = false) : name(name), type(type), length(length), unique(unique), primary(primary) {};
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
    SingleValue(T x): data(x) {};
    SingleValue(const char *mem): data(*(const T*)mem) {};
    virtual ~SingleValue();
    virtual bool operator==(const Value &rhs) const;
    virtual bool operator<(const Value &rhs) const;
    virtual Size size() const;
    virtual void memoryCopy(void *dest) const;
    virtual void print(ostream &out) const;
    virtual T getData() const {return data;}
};
typedef SingleValue<int> Int;
typedef SingleValue<float> Float;
typedef SingleValue<string> String;
typedef shared_ptr<Value> PValue;

template<>
Size String::size() const;

template<>
void String::memoryCopy(void *dest) const;

template<>
inline String::SingleValue(const char *mem){
    data = mem;
}

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
    PValue create(const char *mem) const {
        assert(isValid());
        switch (m_type) {
            case INTTYPE: return PValue(new Int(mem));
            case FLOATTYPE: return PValue(new Float(mem));
            case CHARTYPE: return PValue(new String(mem));
            default: assert(0 && "unknown type");
        }
    }
    // the number of bytes
    unsigned size() const {
        return m_size;
    }
};

class ValueDetail {
public:
    attributeType type;
    string value;
    ValueDetail(attributeType type, string value) : type(type), value(value) {};
    /* type: value type
     * value: all the value saved as string
     * valueDetail: constructor
    */
};

typedef vector<ValueDetail> Tuple;
typedef vector<Tuple> Table;

class QueryDetail {
public:
    queryType type;
    ValueDetail val;
    int targetPosition;
    QueryDetail(queryType type, ValueDetail val, int targetPosition) : type(type), val(val), targetPosition(targetPosition) {};
    /* type: query type
     * val: the value of the restraint
     * targetPosition: the attribute position in the table.
     * queryDetail: constructor
    */
};

#endif
