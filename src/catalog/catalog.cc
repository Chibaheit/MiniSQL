//
//  catalog.cpp
//  MiniSQL
//
//  Created by scn3 on 15/10/30.
//  Copyright © 2015年 scn3. All rights reserved.
//

#include "catalog.h"
#include "../buffer/buffer.h"

bool Catalog::loadSchema(string tableName) {
    if (schema.tableName == tableName)
        return true;
    
    string fileName = tableName + ".catalog";
//    if (Buffer::exists(fileName))
//        return false;
//    if (access(fileName.c_str(), 0) == -1)
//        return false;
    Block *block = Buffer::access(fileName, 0);
    const char *data = block->constData();
    if (data[0] == 0)
        return false;
    
    clearSchema();
    schema.tableName = tableName;
    string str = "";
    infoType curInfoType = ATTRNAME;
    AttrDetail *curAttr;
    int zeroCount = 0;  // 连续的0的个数，一个0表示字符串结束，两个0表示attr结束（包括其index），三个0表示table结束；
    
    int blockIndex = 0;
    int fromOffset;
    while (true) {
        if (blockIndex == 0)
            fromOffset = 1;
        else fromOffset = 0;
        Block *block = Buffer::access(fileName, blockIndex++);
        const char *data = block->constData();
        for (int i = fromOffset; i < block->size(); i++) {
            char ch = data[i];
            if (ch == 0) {
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
    int offset = 1;
    Block *block = Buffer::access(fileName, 0);
    char *data = block->data();
    data[0] = 1;
    
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++) {
        save(itr->attrName, blockIndex, offset);
        save('\0', blockIndex, offset);
        if (itr->attrType == INTTYPE)
            save('0', blockIndex, offset);
        else if (itr->attrType == CHARTYPE)
            save('1', blockIndex, offset);
        else save('2', blockIndex, offset);
        save('\0', blockIndex, offset);
        char ch = itr->length;
        string str = "";
        str += ch;
        save(str, blockIndex, offset);
        save('\0', blockIndex, offset);
        if (itr->unique)
            save('1', blockIndex, offset);
        else save('0', blockIndex, offset);
        save('\0', blockIndex, offset);
        if (itr->primary)
            save('1', blockIndex, offset);
        else save('0', blockIndex, offset);
        save('\0', blockIndex, offset);
        save('\0', blockIndex, offset);
    }
    save('\0', blockIndex, offset);
    return true;
}

void Catalog::save(string str, int &blockIndex, int &offset) {
    for (int i = 0; i < str.size(); i++)
        save(str.at(i), blockIndex, offset);
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
}

bool Catalog::createTable(string tableName, vector<AttributeDetail> &attributeList) {
    clearSchema();
    if (checkTableExist(tableName))
        return false;
    schema.tableName = tableName;
    for (vector<AttributeDetail>::iterator itr = attributeList.begin(); itr != attributeList.end(); itr++) {
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
    return true;
}

bool Catalog::createIndex(string tableName, string attributeName, string indexName) {
    if (!loadSchema(tableName))
        return false;
    if (!checkAttributeExist(tableName, attributeName))
        return false;
    if (checkIndexExist(tableName, indexName))
        return false;
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++)
        if (itr->attrName == attributeName)
            itr->indexList.push_back(indexName);
    storeSchema();
    return true;
}

bool Catalog::dropTable(string tableName) {
    if (!checkTableExist(tableName))
        return false;
    Block *block = Buffer::access(tableName + ".catalog", 0);
    char *data = block->data();
    data[0] = 0;
    tableName = "";
    clearSchema();
    return true;
}

bool Catalog::dropIndex(string tableName, string indexName) {
    if (!loadSchema(tableName))
        return false;
    if (!checkIndexExist(tableName, indexName))
        return false;
    for (vector<AttrDetail>::iterator aitr = schema.attrDetailList.begin(); aitr != schema.attrDetailList.end(); aitr++) {
        for (vector<string>::iterator sitr = aitr->indexList.begin(); sitr != aitr->indexList.end();) {
            if (*sitr == indexName) {
                aitr->indexList.erase(sitr++);
                return true;
            }
            else sitr++;
        }
    }
    return storeSchema();
}

int Catalog::getAttributeSize(string tableName, string attributeName) {
    loadSchema(tableName);
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++)
        if (itr->attrName == attributeName)
            return itr->length;
    return 4;
}

int Catalog::getAttributesTotalSize(string tableName) {
    int sum = 0;
    loadSchema(tableName);
    for (vector<AttrDetail>::iterator itr = schema.attrDetailList.begin(); itr != schema.attrDetailList.end(); itr++)
        sum += itr->length;
    return sum;
}
