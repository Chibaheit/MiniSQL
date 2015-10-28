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
  queryDetail(queryType type, valueDetail val) : queryType(type), valueDetail(value) {};
  /* type: query type
   * val: the value of the restraint
   * targetPosition: the attribute position in the table.
   * queryDetail: constructor
  */
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

bool deleteAttribute(string tableName, vector<queryDetial> queryList);
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

bool deleteAttribute(string tableName, vector<queryDetial> queryList);
```

## Index Manager
## Buffer Manager
## DB Files
