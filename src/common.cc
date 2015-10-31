#include "common.h"

ostream &operator<<(ostream &out, const Value &val){
	val.print(out);
	return out;
}

Value::~Value(){}

template <class T>
bool SingleValue<T>::operator==(const Value &rhs) const {
	auto r = dynamic_cast<const SingleValue<T>&>(rhs);
	return data == r.data;
}

template <class T>
bool SingleValue<T>::operator<(const Value &rhs) const {
	auto r = dynamic_cast<const SingleValue<T>&>(rhs);
	return data < r.data;
}


template <class T>
unsigned SingleValue<T>::size() const {
    return sizeof(T);
}

template <class T>
void SingleValue<T>::memoryCopy(void *dest) const {
    memcpy(dest, &data, size());
}

template <class T>
void SingleValue<T>::print(ostream &out) const {
	out << data;
}

/*
template <>
String::SingleValue(const char *mem) {
	data = mem;
}
*/

template <class T>
SingleValue<T>::~SingleValue(){}

template<>
Size String::size() const {
    return data.size();
}

template<>
void String::memoryCopy(void *dest) const {
    strcpy((char*)dest, data.data());
}

String __string_template("Hello, World!");
Float __float_template(1.0);
Int __int_template(1);
