#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>

using namespace std;

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
private:
	virtual void print(ostream &out) const;
public:
    virtual ~Value(){};
    virtual bool operator==(const Value &rhs) const=0;
    virtual bool operator<(const Value &rhs) const=0;
    virtual void memoryCopy(void *dest) const=0;
	virtual Size size() const=0;
	friend ostream &operator<<(ostream &out, const Value &val){
		val.print(out);
		return out;
	}
};

template<class T>
class SingleValue: public Value {
private:
	T data;
public:
	SingleValue(T x):data(x){}
	virtual ~SingleValue(){};
	virtual bool operator==(const Value &rhs) const {
		auto r = dynamic_cast<const SingleValue<T>&>(rhs);
		return data == r.data;
	}
	virtual bool operator<(const Value &rhs) const {
		auto r = dynamic_cast<const SingleValue<T>&>(rhs);
		return data < r.data;
	}
	virtual Size size() const {
		return sizeof(T);
	}
	virtual void memoryCopy(void *dest) const {
		memcpy(dest, &data, size());
	}
	virtual void print(ostream &out) const {
		out << data;
	}
};
typedef SingleValue<int> Int;
typedef SingleValue<float> Float;
typedef SingleValue<string> String;

template<>
Size String::size() const {
    return data.size();
}

template<>
void String::memoryCopy(void *dest) const {
    strcpy((char*)dest, data.data());
}

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
