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
#include "common.h"

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
    
    bool checkTableExist(string tableName);
    
    bool checkAttributeExist(string tableName, string attributeName);
    
    bool checkIndexExist(string tableName, string indexName);
    
    bool getIndexList(string tableName, string attributeName, Strings &indexList);
    
    void createTable(string tableName, vector<attributeDetail> attributeList);
    
    bool createIndex(string tableName, string attributeName, string indexName);
};

#endif /* catalog_hpp */
