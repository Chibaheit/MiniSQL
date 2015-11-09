# Record
1. Record Manager主要负责记录表中数据的数据文件，为API层提供如下的接口：
	- 创建一个表
	- 删除一个表
	- 插入一条记录
	- 删除一条记录
	- 筛选指定记录
2. 在信息的存储上，每一个表的记录信息都单独地存在一个名为<tableName>.record的文件中，每一个文件记录数据的格式如下：
	- 第一字节标识该表是否有效（即是否已被删除），有效为0x01，否则为0x00
	- 第2到5这四个字节存一个四字节整数，表示该表的数据尾在该文件的第几个block中（记为blockIndex）
	- 第6、7两个字节存一个二字节整数，表示该表的数据尾在该文件的某个block中的偏移（记为offset）
	- 第8、9两个字节存一个二字节整数，表示该表每一条记录的长度（定长）
	- 第10个字节存一个一字节整数，表示该表有几个属性
	- 第11个字节开始，每两个字节分别按顺序存某一属性的数据类型和长度
	- 以上信息在创建表的时候进行初始化并存入文件
	- 随后再某一位置开始存记录的具体数据，由于记录只支持定长，因此无需用分隔符分开
3. 由于每次对Record的操作都是在特定的一个表中，因此将表的名称tableName作为Record类的一个成员变量，Record类同时也包括该table对应文件的数据尾的位置、该类的所有属性的数据类型和长度等信息，在构造函数中直接将这些信息从对应文件中读取。
4. 为了方便记录的读取和修改，记录的读取和修改都以一条记录为单位。
5. 为了方便记录的删除和筛选，提供一个match的私有方法来判断记录是否匹配查询条件
6. 记录的插入：每次记录插入后都要修改blockIndex和offset的值，并在这些数据存入文件。数据的插入时，线性遍历每一条记录，判断primary或unique字段是否冲突。因此插入的时间效率不高。
7. 记录的删除：为了提升记录删除的时间效率，而同时空间代价完全能够接受，因此记录的删除采用标记删除的策略，即为每一条记录增加一个标记位，有效记录为1，无效记录为0，在记录删除时不实际地在文件中删除对应记录的数据，而且将该记录的标记位改为0.同样地，在删除整个表时，仅仅在表文件的第一位清0即可。
8. Record类提供的public方法清单以及备注如下：

```cpp
    // create a file to store the table and store some information into it for future operation
    // return true if the operation is successfuly
    // return false if attrType.size() != attrSize.size()
    // PLEASE ASSUME THE TABLE HAS NOT ALREADY BEEN CREATED(USING CATALOG)
    bool createTable(vector<attributeType> attrType, vector<int> attrSize);
    // drop table
    // return true if the operation is successful
    // return false if the table is not existent
    bool dropTable();  
    // insert the valueList with the information which attribute is primary or unique
    // return true if the operation is successful
    // return false if the operation is failed(same primary key or unique attribute as before)
    bool insert(Tuple &valueList, vector<int> primaryOrUniquePosition);
    // delete tuple with query detail
    // return the number of the record deleted
    int deleteTuple(vector<QueryDetail> &queryList);
    // select tuples with query detail
    // the result of selection is stored in table
    void select(vector<QueryDetail> &queryList, Table &table);
```