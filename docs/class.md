## CommonTypedef
```cpp
typedef unsigned int Size;
typedef vector<Size> Sizes;
typedef vector<string>& Strings;
# All capital in enum name
# In common, we should use all cappital in naming enum type,
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


class attributeDetail {
public:
  attributeType type;
  # attribute type
  int length;
  # if type == CHARTYPE, it must have length
  bool unique;
  # it is unique or not
  bool primary;
  # it is primary key or not
  attributeDetail(attributeType type, int length = 0, bool unique = false, bool primary = false) : type(type), length(length), unique(unique), primary(primary) {};
  # constructor
};

class valueDetail {
public:
  attributeType type;
  # value type
  string value;
  # all the value saved as string
  valueDetail(attributeType type, string value) : type(type), value(value) {};
  # constructor
};

class queryDetail {
public:
  queryType type;
  # query type
  valueDetail val;
  # the value of the restraint
  queryDetail(queryType type, valueDetail val) : queryType(type), valueDetail(value) {};
  # constructor
};

```

## Interpreter


## API
```cpp
bool createTable(string tableName, vector<attributeDetail> attributeList);

bool dropTable(string tableName);

bool createIndex(string tableName, string attributeName, Strings indexList);

bool dropIndex(string indexName);

bool select(string tableName, vector<queryDetail> queryList);

bool insert(string tableName, vector<valueDetail> valueList);

bool delete(string tableName, vector<queryDetial> queryList);
```

## Catalog Manager

```cpp
/*
the first three is for interpreter to check whether a  table, a attribute, an index exist(if existing, return true)
so that the interpreter can judge whether a SQL sentence is right

the last four is for API to get size or other useful information of a table or attribute
so that the API can do optimize
*/
bool checkTableExist(string tableName);

bool checkAttributeExist(string tableName, string attributeName);

bool checkIndexExist(string indexName);
// for drop index
Size getTableSize(string tableName);

Sizes getAttributeSize(string tableName, Strings attributeName);

Strings getIndexList(string tableName,Strings attributeName);

Strings getAttributeType(string tableName);

```

## Record Manager

```cpp
/*
the all seven will return a bool to show whether the operation is successful
*/
bool createTable(string tableName, vector<attributeDetail> attributeList);

bool dropTable(string tableName);

bool select(string tableName, vector<queryDetial> queryList);

bool insert(string tableName, vector<valueDetail> valueList);

bool delete(string tableName, vector<queryDetial> queryList);
```

## Index Manager
## Buffer Manager
## DB Files
