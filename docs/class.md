## CommonTypedef
~~~cpp
typedef unsigned int Size;
typedef vector<Size> Sizes;
typedef vector<string>& Strings;
~~~
## Interpreter


## API
~~~cpp
bool createTable(string tableName, vector<attributeDetail>& attributeList, int primaryKeyPosition = -1)

bool dropTable(string tableName);

bool createIndex(string tableName, string attributeName, Strings indexList);

bool dropIndex(string indexName);

bool select(string tableName, vector<queryDetial> queryList);

bool insert(string tableName, vector<valueDetail> valueList, vector<int> position);

bool delete(string tableName, vector<queryDetial> queryList);
~~~
## Catalog Manager

~~~cpp
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

vector<string> getIndexList(string tableName,Strings attributeName);

vector<string> getAttributeType(string tableName);

~~~
## Record Manager

~~~cpp
/*
the all seven will return a bool to show whether the operation is successful
*/
bool createTable(string tableName, vector<attributeDetail>& attributeList, int primaryKeyPosition = -1)

bool dropTable(string tableName);

bool select(string tableName, vector<queryDetial> queryList);

bool insert(string tableName, vector<valueDetail> valueList, vector<int> position);

bool delete(string tableName, vector<queryDetial> queryList);
~~~
## Index Manager
## Buffer Manager
## DB Files