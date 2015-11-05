//
//  record.hpp
//  MiniSQL
//
//  Created by scn3 on 15/10/30.
//  Copyright © 2015年 scn3. All rights reserved.
//

#ifndef record_hpp
#define record_hpp

#include "../common.h"

class Record{
    string tableName;
    bool exist;
    int blockIndex;
    int offset;
    int tupleSize;
    int numAttr;
    int dataBegin;
    class AttrInfo {
    public:
        attributeType type;
        int size;
    }*attrInfo;
    
    void loadStatusAndInfo();
    void storeStatusAndInfo();
    void storeData(int intData, char *data, int &curOffset);
    void storeData(float floatData, char *data, int &curOffset);
    void storeData(string stringData, char *data,  int &curOffset);
    Tuple loadTuple(int fromBlockIndex, int fromOffset);
    void deleteTuple(int fromBlockIndex, int fromOffset);
    void storeTuple(Tuple &tuple, int &fromBlockIndex, int &fromOffset);
    Tuple loadLastTuple();
    void deleteLastTuple();
    bool match(Tuple &tuple, vector<QueryDetail> &queryList);
public:
    Record(string tableName);
    ~Record();
    void createTable(int numAttribute, attributeType *attrType, int *attrSize);
    
    bool dropTable();
    
    void insert(Tuple &valueList);
    
    void deleteTuple(vector<QueryDetail> &queryList);

    void select(vector<QueryDetail> &queryList, Table &table);
};

#endif /* record_hpp */
