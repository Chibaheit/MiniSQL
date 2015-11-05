#include "../catalog/catalog.hpp"
#include "../buffer/buffer.h"

int main() {
    Catalog *testCatlog = &Catalog::getInstance();
    string tableName1("tableA");
    string tableName2("tableB");
    
    // checkTableExist before tableA created
    if (testCatlog->checkTableExist(tableName1))
        cout << "table(" <<tableName1 << ") exists" << endl;
    else cout << "table(" << tableName1 << ") does not exist" << endl;
    
    // creatTable
    AttributeDetail attr1("attrA", INTTYPE, 4, true, true);
    AttributeDetail attr2("attrB", FLOATTYPE, 4, true, false);
    AttributeDetail attr3("attrC", CHARTYPE, 3, false, false);
    vector<AttributeDetail> attributeList;
    attributeList.push_back(attr1);
    attributeList.push_back(attr2);
    if (testCatlog->createTable(tableName1, attributeList))  // create tableA firstly
        cout << "create table(" << tableName1 << ") successfully" << endl;
    else cout << "fail to create table(" << tableName1 << ")" << endl;
    if (testCatlog->createTable(tableName1, attributeList))  // create tableA again
        cout << "create table(" << tableName1 << ") successfully" << endl;
    else cout << "fail to create table(" << tableName1 << ")" << endl;
    attributeList.push_back(attr3);
    if (testCatlog->createTable(tableName2, attributeList))  // create tableB
        cout << "create table(" << tableName2 << ") successfully" << endl;
    else cout << "fail to create table(" << tableName2 << ")" << endl;
    
    // checkTableExist again
    if (testCatlog->checkTableExist(tableName1))  // check whether tableA exists after it was created
        cout << "table(" <<tableName1 << ") exists" << endl;
    else cout << "table(" << tableName1 << ") does not exist" << endl;
    if (testCatlog->checkTableExist(tableName2))  // check whether tableB exists
        cout << "table(" <<tableName2 << ") exists" << endl;
    else cout << "table(" << tableName2 << ") does not exist" << endl;
    
    // checkAttributeExist
    if (testCatlog->checkAttributeExist(tableName1, "attrA"))  // check a existent attribute
        cout << "attribute(attrA) exists" << endl;
    else cout << "attribute(attrA) does not exist" << endl;
    if (testCatlog->checkAttributeExist(tableName1, "attrD"))  // check a nonexistent attribute
        cout << "attribute(attrD) exists" << endl;
    else cout << "attribute(attrD) does not exist" << endl;
    
    // createIndex
    if (testCatlog->createIndex(tableName1, "attrA", "indexA"))
        cout << "create index(indexA) successfully" << endl;
    else cout << "fail to create index(indexA)" << endl;
    if (testCatlog->createIndex(tableName1, "attrA", "indexA"))  // after attrA created
        cout << "create index(indexA) successfully" << endl;
    else cout << "fail to create index(indexA)" << endl;
    if (testCatlog->createIndex(tableName1, "attrD", "indexB"))  // create on a illegal attribute
        cout << "create index(indexA) successfully" << endl;
    else cout << "fail to create index(indexA)" << endl;
    if (testCatlog->createIndex("tableC", "attrA", "indexA"))  // create on a illegal table
        cout << "create index(indexA) successfully" << endl;
    else cout << "fail to create index(indexA)" << endl;
    if (testCatlog->createIndex(tableName1, "attrA", "indexB"))
        cout << "create index(indexB) successfully" << endl;
    else cout << "fail to create index(indexB)" << endl;
    
    // checkIndexExist
    if (testCatlog->checkIndexExist(tableName1, "indexA"))
        cout << "indexA exists" << endl;
    else cout << "indexA does not exist" << endl;
    if (testCatlog->checkIndexExist(tableName1, "indexC"))
        cout << "indexC exists" << endl;
    else cout << "indexC does not exist" << endl;
    if (testCatlog->checkIndexExist("tableC","indexA"))
        cout << "indexA exists" << endl;
    else cout << "indexA does not exist" << endl;
    
    // getIndexList
    vector<string> indexList;
    if (!testCatlog->getIndexList("tableA", "attrA", indexList))
        cout << "fail to getIndexList" << endl;
    else {
        cout << "IndexList: ";
        for (vector<string>::iterator itr = indexList.begin(); itr != indexList.end(); itr++)
            cout << *itr << " ";
        cout << endl;
    }
    if (!testCatlog->getIndexList("tableD", "attrA", indexList))
        cout << "fail to getIndexList" << endl;
    else {
        cout << "IndexList: ";
        for (vector<string>::iterator itr = indexList.begin(); itr != indexList.end(); itr++)
            cout << *itr << " ";
        cout << endl;
    }
    
    // dropIndex
    if (testCatlog->dropIndex("tableD", "indexA"))
         cout << "drop index successfully" << endl;
    else cout << "fail to drop index" << endl;
    if (testCatlog->dropIndex("tableA", "indexC"))
        cout << "drop index successfully" << endl;
    else cout << "fail to drop index" << endl;
    if (testCatlog->dropIndex("tableA", "indexA"))
        cout << "drop index successfully" << endl;
    else cout << "fail to drop index" << endl;
    
    // dropTable
    if (testCatlog->dropTable("tableA"))
        cout << "drop table successfully" << endl;
    else cout << "fail to drop table" << endl;
    if (testCatlog->dropTable("tableA"))
        cout << "drop table successfully" << endl;
    else cout << "fail to drop table" << endl;
    
    Buffer::flush();
    return 0;
}