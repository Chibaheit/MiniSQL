//
//  catalog.hpp
//  MiniSQL
//
//  Created by scn3 on 15/10/30.
//  Copyright © 2015年 scn3. All rights reserved.
//

#ifndef catalog_hpp
#define catalog_hpp

#include <unordered_map>
#include "../common.h"

#ifndef info_type
#define info_type
#define ATTRNAME    0
#define ATTRTYPE    1
#define ATTRLEN     2
#define ATTRUNIQUE  3
#define ATTRPRIMARY 4
#define INDEXNAME   5
#endif

class Catalog {
private:
    typedef vector<string> strings;
    class AttrDetail{
    public:
        string attrName;
        attributeType attrType;
        int length;
        bool unique;
        bool primary;
        strings indexList;
    };
    class Schema {
    public:
        string tableName;
        vector<AttrDetail> attrDetailList;
    };
    typedef int infoType;

    Schema schema;

    bool loadSchema(string tableName);

    bool storeSchema();

    void save(string str, int &blockIndex, int &offset);

    void save(char ch, int &blockIndex, int &offset);

    void save2(string str, int &blockIndex, int &offset);

    void save2(char ch, int &blockIndex, int &offset);

    string loadStr(int &blockIndex, int &offset);

    void clearSchema();

    Catalog() {clearSchema(); }

    ~Catalog() {clearSchema(); }

    Catalog(const Catalog &);

    Catalog &operator=(const Catalog &);
public:
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

    vector<string> getAttributeName(string tableName);
};

#endif /* catalog_hpp */
