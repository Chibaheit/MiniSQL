# Catalog
1. Catalog Manager负责管理数据库的所有模式信息，为了实现较好的管理，对class Catalog使用了单例模式
2. Catalog Manager主要将表的模式信息提供给API和interpreter层，提供的信息包括以下几类：
	- 某一表是否已经存在
	- 某一表上的某一属性是否存在
	- 某一索引是否存在
	- 获取某一表的某一属性上的所有的索引名
	- 获取属性的类型
  同时，为了保证catalog所记录的信息的有效性，每一次涉及对表的模式信息的修改的操作都需调用catalog对应的接口，如：创建一个表、删除一个表、创建索引、删除索引等。
3. 在信息的存储上，每一个表的模式信息都单独地存在一个名为<tableName>.catalog的文件中，每个catalog文件的信息格式如下：
	- 第一个字节存改模式信息的有效性，即该表是否仍存在，存在为0x01，不存在为0x00
	- 第二字节开始，依次存每一个属性的属性名（字符串形式，一个字符一字节，以0结尾）、该属性的属性类型（integer类型、float类型、char类型，分别存为0x40,0x41,0x42）、该属性的属性长度（由于长度小于256，故用一字节存）、是否unique、是否primary以及该字段上的所有索引名
	- 同一属性上的所有信息见以一个0分隔，不同属性以两个0分隔，模式信息结束用三个0标识，以此方便读取
4. 由于不同表上不能有同名索引，且数据库有drop Index命令，因此还需专门将所有index的信息存入这个文件中（index.cat），此文件中的数据格式为<indexName1>'0'<tableName1>'00'<indexName2>'0'<tableName2>'00'...
	以此将index建立在哪个表上的信息存下来
5. Catalog类有一个tableName和一个存有对应表的所有模式信息的子类schema做为成员变量。当需要读取模式信息，或者对模式信息进行操作时，先比较当前要读取或修改的模式信息所属的表名与已存在Catalog中的tableName是否相同，相同则直接使用其信息，不同则从对应的catalog文件中读取新的模式信息。
6. 为了方便存储和读取，模式信息的存取都以整个表为单位，而考虑到表的模式信息的数据量不会很大，因此也完全在可接受范围内。
7. 当对模式信息的修改操作结束后，将模式信息存入对应文件。
	另一种更高效的实现方式：当需要操作模式信息的表与之前在catalog中存在的模式信息不同的情况下，在读取新的模式信息前把旧的模式信息存入文件，这样可以减少将数据存入磁盘的频率。
	但是两者差距不大，前者也完全在可接受范围内，因此采用了前者。 
8. Catalog类提供的public方法清单以及备注如下：

```cpp
    static Catalog &getInstance() {
        static Catalog instance;
        return instance;
    }

    // check whether a table(tableName) exists
    // return true if existing
    bool checkTableExist(string tableName);
    // check whether a attribute(attributeName) on the table(tableName) exists
    // return true if existing
    int checkAttributeExist(string tableName, string attributeName);
    // check whether a index(indexName) exists
    // return true if existing
    // return false if not existing
   bool checkIndexExist(string indexName);
    // check whether a index(indexName) on the table(tableName) exists
    // return true if existing
    // return false in the following 2 cases:
     // 1.the table(tableName) is nonexistent
     // 2.the index(indexName) on table(tableName) exists
    // !!!!
    // So if you want it actually check whether the index exist
    // please assume the table is existent before use it
    // !!!!
    bool checkIndexExist(string tableName, string indexName);
    // get all the index on an attribute(attributeName) of table(tableName)
    // the indexName will be store in indexList(references)
    // return true if the operation is successful
    // return false in the following 2 cases:
     // 1.the table(tableName) is nonexistent
     // 2.the attribute(attributeName) is nonexistent on table(tableName)
    bool getIndexList(string tableName, string attributeName, Strings indexList);
    // create and store the schema of a table(tableName) with specific schema(attributeList)
    // return true if the operation is successful
    // return false if it fails(because this table has already existed)
    bool createTable(string tableName, vector<AttributeDetail> &attributeList);
    // update the schema according to : create a index(indexName) on an attribute(attributeName) on a table(indexName)
    // return true if the operation is successful
    // return false in the following 3 cases:
     // 1.the table(tableName) is nonexistent
     // 2.the attribute(attributeName) is nonexistent
     // 2.the index(indexName) is already existent on table(tableName)
    bool createIndex(string tableName, string attributeName, string indexName);
    // delete the schema of the table(tableA)
    // return true if the operation is successful
    // return false if the table was nonexistent before dropping
    bool dropTable(string tableName);
    // update the schema according to : delete a index(indexName) on a table(tableName)
    // return true if the operation is successful
    // return false in the following 2 cases:
     // 1.the table(tableName) is nonexistent
     // 2.there is not a index named indexName on table(tableName)
    bool dropIndex(string tableName, string indexName);
    // update the schema according to : delete a index(indexName)
    // return true if the operation is successful
    // return false if there is not a index named indexName on table(tableName)
    bool dropIndex(string indexName);
    // return the size of the attribute(attributeName) on the table(tableName)
    int getAttributeSize(string tableName, string attributeName);
    // return the total sizes of all the attribute on the table(tableName)
    int getAttributesTotalSize(string tableName);
    // return type list of table
    vector<attributeType> getAttributeType(string tableName, string queryName = "");
    // return primary or unique position
    vector<int> getPrimaryOrUniquePosition(string tableName);

``` 