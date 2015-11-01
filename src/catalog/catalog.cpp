//
//  catalog.cpp
//  MiniSQL
//
//  Created by scn3 on 15/10/30.
//  Copyright © 2015年 scn3. All rights reserved.
//

#include "catalog.hpp"
#include "../buffer/buffer.h"

//const int defaultBlockSize = 4096;
//extern int defaultBlockSize;

bool Catalog::loadSchema(string tableName) {
    if (schema.tableName == tableName)
        return true;
    
    string fileName = tableName + ".catalog";
    if (access(fileName.c_str(), 0) == -1)
        return false;
    
    clearSchema();
    schema.tableName = tableName;
    string str = "";
    infoType curInfoType = ATTRNAME;
    AttrDetail *curAttr;
    int zeroCount = 0;  // 连续的0的个数，一个0表示字符串结束，两个0表示attr结束（包括其index），三个0表示table结束；
    
    int blockIndex = 0;
    while (true) {
        Block *block = Buffer::access(fileName, blockIndex++);
        const char *data = block->constData();
        
        for (int i = 0; i < block->size(); i++) {
            char ch = data[i];
            if (ch == 1) {
                if (++zeroCount == 1)
                switch (curInfoType) {
                    case ATTRNAME: {
                        curAttr = new AttrDetail;
                        curAttr->attrName = str;
                        break;
                    }
                    case ATTRTYPE: {
                        if (str == "0")
                            curAttr->attrType = INTTYPE;
                        else if (str == "1")
                            curAttr->attrType = CHARTYPE;
                        else if (str == "2")
                            curAttr->attrType = FLOATTYPE;
                        else return false;
                        break;
                    }
                    case ATTRLEN: {
                        curAttr->length = str.at(0);
                        break;
                    }
                    case ATTRUNIQUE: {
                        curAttr->unique = str == "1";
                        break;
                    }
                    case ATTRPRIMARY: {
                        curAttr->primary = str == "1";
                        break;
                    }
                    case INDEXNAME: {
                        curAttr->indexList.push_back(str);
                    }
                    default:
                        break;
                }
                if (zeroCount > 2) {
                    schema.attrDetailList.push_back(*curAttr);
                    return true;
                }
                str.clear();
            }
            else {
                if (zeroCount == 2) {   // 一个Attr结束，到一个新的attr，则第一个信息类型是Attr名
                    curInfoType = ATTRNAME;
                    schema.attrDetailList.push_back(*curAttr);
                }
                else if (zeroCount == 1) {
                    if (curInfoType < INDEXNAME)    // 如果当前信息不是索引名信息，则切换到下一信息类型
                        curInfoType++;
                }
                zeroCount = 0;
                str += ch;
            }
        }
    }
    
    return true;
}

bool Catalog::storeSchema() {
    if (schema.tableName.empty())
        return false;
    
    string fileName = schema.tableName + ".catalog";
    int blockIndex = 0;
    int offset = 0;
//    Block *block = Buffer::access(fileName, blockIndex++);
//    char *data = block->data();
//    string str;
    
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++) {
//        str = itr->attrName;
//        if (str.size() < defaultBlockSize - offset) {
//            for (int pos = 0; offset < defaultBlockSize; offset++, pos++)
//                data[offset] = str.at(pos);
//        }
        save(itr->attrName, blockIndex, offset);
        save('\1', blockIndex, offset);
        if (itr->attrType == INTTYPE)
            save('0', blockIndex, offset);
        else if (itr->attrType == CHARTYPE)
            save('1', blockIndex, offset);
        else save('2', blockIndex, offset);
        save('\1', blockIndex, offset);
        char ch = itr->length;
        string str = "";
        str += ch;
        save(str, blockIndex, offset);
        save('\1', blockIndex, offset);
        if (itr->unique)
            save('1', blockIndex, offset);
        else save('0', blockIndex, offset);
        save('\1', blockIndex, offset);
        if (itr->primary)
            save('1', blockIndex, offset);
        else save('0', blockIndex, offset);
        save('\1', blockIndex, offset);
        save('\1', blockIndex, offset);
    }
    save('\1', blockIndex, offset);
    return true;
}

void Catalog::save(string str, int &blockIndex, int &offset) {
    for (int i = 0; i < str.size(); i++)
        save(str.at(i), blockIndex, offset);
//    string fileName = schema.tableName + ".catalog";
//    Block *block = Buffer::access(fileName, blockIndex);
//    char *data = block->data();
//    if (str.size() <= defaultBlockSize - offset)
//        for (int pos = 0; pos < str.size(); pos++, offset++)
//            data[offset] = str.at(pos);
//    else {
//        int pos = 0;
//        for (; offset < defaultBlockSize; pos++, offset++)
//            data[offset] = str.at(pos);
//        Block *block2 = Buffer::access(fileName, ++blockIndex);
//        char *data2 = block2->data();
//        for (offset = 0; pos < str.size(); pos++, offset++)
//            data2[offset] = str.at(pos);
//    }
}

void Catalog::save(char ch, int &blockIndex, int &offset) {
    if (offset == defaultBlockSize) {
        offset = 0;
        blockIndex++;
    }
    Block *block = Buffer::access(schema.tableName + ".catalog", blockIndex);
    char *data = block->data();
    data[offset++] = ch;
}

void Catalog::clearSchema() {
    schema.tableName.clear();
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++) {
        vector<string> ().swap(itr->indexList);
    }
    vector<AttrDetail> ().swap(schema.attrDetailList);
}

bool Catalog::checkTableExist(string tableName) {
    if (schema.tableName == tableName)
        return true;
    return loadSchema(tableName);
}

bool Catalog::checkAttributeExist(string tableName, string attributeName) {
    if (!loadSchema(tableName))
        return false;
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++) {
        if (itr->attrName == attributeName)
            return true;
    }
    return false;
    /*
    int blockIndex = 0;
    while (true) {
        Block *block = Buffer::access(tableName + ".catalog", blockIndex++);
        const char *data = block->constData();
        bool attrFlag = false;
        int zeroCount = 0;  // 连续的0的个数，一个0表示字符串结束，两个0表示attr结束（包括其index），三个0表示table结束；
        string str;
        
        for (int i = 0; i < block->size(); i++) {
            char ch = data[i];
            if (ch == 0) {
                zeroCount++;
                if (zeroCount == 3)
                    return false;
            }
            else {
                if (zeroCount == 2)
                    attrFlag = false;
                else if (zeroCount == 1) {
                    if (!attrFlag) {
                        attrFlag = true;
                        if (str == attributeName)
                            return true;
                    }
                }
                zeroCount = 0;
                if (!attrFlag)
                    str += ch;
            }
        }
    }
     */
}

bool Catalog::checkIndexExist(string tableName, string indexName) {
    if (!loadSchema(tableName))
        return false;
    for (vector<AttrDetail>::iterator aitr = schema.attrDetailList.begin(); aitr != schema.attrDetailList.end(); aitr++) {
        vector<string> curIndexList = aitr->indexList;
        for (vector<string>::iterator sitr = curIndexList.begin(); sitr != curIndexList.end(); sitr++) {
            if (*sitr == indexName)
                return true;
        }
    }
    return false;
    /*
    int blockIndex = 0;
    while (true) {
        Block *block = Buffer::access(tableName + ".catalog", blockIndex++);
        const char *data = block->constData();
        bool attrFlag = false;
        int zeroCount = 0;  // 连续的0的个数，一个0表示字符串结束，两个0表示attr结束（包括其index），三个0表示table结束；
        string str;
        
        for (int i = 0; i < block->size(); i++) {
            char ch = data[i];
            if (ch == 0) {
                zeroCount++;
                if (zeroCount == 3)
                    return false;
            }
            else {
                if (zeroCount == 2)
                    attrFlag = false;
                else if (zeroCount == 1) {
                    if (!attrFlag)
                        attrFlag = true;
                    else if (str == indexName)
                        return true;
                }
                zeroCount = 0;
                if (attrFlag)
                    str += ch;
            }
        }
    }
     */
}

bool Catalog::getIndexList(string tableName, string attributeName, Strings indexList) {
    indexList.clear();
    if (!loadSchema(tableName))
        return false;
    for (vector<AttrDetail>::iterator aitr = schema.attrDetailList.begin(); aitr != schema.attrDetailList.end(); aitr++)
        if (aitr->attrName == attributeName) {
            indexList = aitr->indexList;
            return true;
        }
    return false;
    /*
    if (!checkTableExist(tableName))
        return false;
    vector<string> strs;
    int blockIndex = 0;
    while (true) {
        Block *block = Buffer::access(tableName + ".catalog", blockIndex++);
        const char *data = block->constData();
        bool attrFlag = false;
        bool matchAttrSuccessfully = false;
        int zeroCount = 0;  // 连续的0的个数，一个0表示字符串结束，两个0表示attr结束（包括其index），三个0表示table结束；
        string str;
        
        for (int i = 0; i < block->size(); i++) {
            char ch = data[i];
            if (ch == 0) {
                zeroCount++;
                if (zeroCount == 3)   // 3个0，结束
                    return true;
            }
            else {
                if (zeroCount == 2) {
                    if (matchAttrSuccessfully)  // 该attr的index已全部push入indexList
                        return true;
                    attrFlag = false;
                }
                else if (zeroCount == 1) {
                    if (!attrFlag) {
                        attrFlag = true;
                        if (str == attributeName)
                            matchAttrSuccessfully = true;
                    }
                    else if (matchAttrSuccessfully) {
                        indexList.push_back(str);
                    }
                }
                zeroCount = 0;
                if (!attrFlag)
                    str += ch;
            }
        }
    }
    return false;
     */
}

void Catalog::createTable(string tableName, vector<attributeDetail> attributeList) {
    clearSchema();
    schema.tableName = tableName;
    for (vector<attributeDetail>::iterator itr = attributeList.begin(); itr != attributeList.end(); itr++) {
        AttrDetail attrDetail;
        attrDetail.attrName = itr->name;
        attrDetail.attrType = itr->type;
        if (itr->type == INTTYPE || itr->type == FLOATTYPE)
            attrDetail.length = 4;
        else attrDetail.length = itr->length;
        attrDetail.unique = itr->unique;
        attrDetail.primary = itr->primary;
        schema.attrDetailList.push_back(attrDetail);
    }
    storeSchema();
}

bool Catalog::createIndex(string tableName, string attributeName, string indexName) {
    if (!loadSchema(tableName))
        return false;
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++)
        if (itr->attrName == attributeName)
            itr->indexList.push_back(indexName);
    storeSchema();
    return true;
}