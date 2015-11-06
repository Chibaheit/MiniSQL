//
//  record.cpp
//  MiniSQL
//
//  Created by scn3 on 15/10/30.
//  Copyright © 2015年 scn3. All rights reserved.
//

#include "record.h"
#include "../buffer/buffer.h"
#include <string.h>
#include <sstream>

Record::Record(string tableName) : tableName(tableName) {
//    string fileName = tableName + ".record";
//    if (access(fileName.c_str(), 0) != -1)
        loadStatusAndInfo();
//    else exist = false;
}

Record::~Record() {}

void Record::loadStatusAndInfo() {
    Block *block = Buffer::access(tableName + ".record", 0);
    const char *data = block->constData();
    if (data[0] == 0) {
        exist = false;
        return;
    }
    exist = true;
    blockIndex = 0;
    for (int i = 1; i < 5; i++)
        blockIndex = blockIndex * 256 + data[i];
    offset = data[5] * 256 + data[6];
    tupleSize = data[7] * 256 + data[8];
    numAttr = data[9];
    attrInfo = new AttrInfo[numAttr];
    for (int i = 0; i < numAttr; i++) {
        char ch = data[8 + 2 * i];
        if (ch == 0)
            attrInfo[i].type = INTTYPE;
        else if (ch == 1)
            attrInfo[i].type = FLOATTYPE;
        else attrInfo[i].type = CHARTYPE;
        attrInfo[i].size = data[9 + 2 * i];
    }
}

void Record::storeStatusAndInfo() {
    Block *block = Buffer::access(tableName + ".record", 0);
    char *data = block->data();
    if (!exist)
        data[0] = 0;
    else {
        data[0] = 1;
        int tmp = blockIndex;
        for (int i = 4; i > 0; i--) {
            data[i] = tmp % 256;
            tmp /= 256;
        }
        data[5] = offset / 256;
        data[6] = offset % 256;
        data[7] = tupleSize / 256;
        data[8] = tupleSize % 256;
        data[9] = numAttr;
        int curOft = 10;
        for (int i = 0; i < numAttr; i++) {
            if (attrInfo[i].type == INTTYPE)
                data[curOft++] = 0;
            else if (attrInfo[i].type == FLOATTYPE)
                data[curOft++] = 1;
            else data[curOft++] = 2;
            data[curOft++] = attrInfo[i].size;
        }
    }
}

void Record::storeData(int intData, char *data, int &curOffset) {
    for (int i = curOffset + 3; i >= curOffset; i--) {
        data[i] = intData % 256;
        intData /= 256;
    }
    curOffset += 4;
}

void Record::storeData(float floatData, char *data, int &curOffset) {
    char *tmp = new char[4];
    memcpy(tmp, &floatData, 4);
    for (int i = 0; i < 4; i++)
        data[curOffset++] = tmp[i];
}

void Record::storeData(string stringData, char *data, int &curOffset) {
    for (int i = 0; i < stringData.size(); i++)
        data[curOffset++] = stringData.at(i);
}

bool Record::loadTuple(Tuple &tuple, int &fromBlockIndex, int &fromOffset) {
    Block *block = Buffer::access(tableName + ".record", fromBlockIndex);
    const char *data = block->constData();
    if (data[fromOffset] == 0) {
        fromOffset += tupleSize;
        return false;
    }
    else fromOffset++;
    for (int i = 0; i < numAttr; i++) {
        string str = "";
        for (int j = 0; j < attrInfo[i].size; j++)
            str += data[fromOffset++];
        attributeType curType = attrInfo[i].type;
        switch (curType) {
            case INTTYPE: {
                int intData = str.at(0);
                intData = intData * 256 + str.at(1);
                intData = intData * 256 + str.at(2);
                intData = intData * 256 + str.at(3);
                stringstream ss;
                ss << intData;
                tuple.push_back(ValueDetail(curType, ss.str()));
                break;
            }
            case FLOATTYPE: {
                char *tmp = new char[4];
                tmp[0] = str.at(0);
                tmp[1] = str.at(1);
                tmp[2] = str.at(2);
                tmp[3] = str.at(3);
                float *floatData = new float;
                memcpy(floatData, tmp, 4);         
                stringstream ss;
                ss << *floatData;
                tuple.push_back(ValueDetail(curType, ss.str()));
                break;
            }
            case CHARTYPE: {
                tuple.push_back(ValueDetail(curType, str));
                break;
            }
            default:
                break;
        }
    }
    return true;
}

void Record::deleteTuple(int fromBlockIndex, int fromOffset) {
    Block *block = Buffer::access(tableName + ".record", fromBlockIndex);
    char *data = block->data();
    data[fromOffset] = 0;
}

void Record::storeTuple(Tuple &tuple, int &fromBlockIndex, int &fromOffset) {
    if (fromOffset + tupleSize >= defaultBlockSize) {
        fromBlockIndex++;
        fromOffset = 0;
    }
    string fileName = tableName + ".record";
    Block *block = Buffer::access(fileName, fromBlockIndex);
    char *data = block->data();
    data[fromOffset++] = 1; 
    for (vector<ValueDetail>::iterator itr = tuple.begin(); itr != tuple.end(); itr++) {
        switch (itr->type) {
            case INTTYPE: {
                int intData = stoi(itr->value);
                storeData(intData, data, fromOffset);
                break;
            }
            case FLOATTYPE: {
                float floatData = stof(itr->value);
                storeData(floatData, data, fromOffset);
                break;
            }
            case CHARTYPE: {
                string strData = itr->value;
                storeData(strData, data, fromOffset);
                break;
            }
            default:
                break;
        }
    }
}

/*
bool Record::loadLastTuple(Tuple &tuple) {
    int curBlockIndex, curOffset;
    if (offset == 0) {
        curBlockIndex = blockIndex - 1;
        curOffset = (defaultBlockSize / tupleSize - 1) * tupleSize;
    }
    else {
        curBlockIndex = blockIndex;
        curOffset = offset - tupleSize;
    }
    return loadTuple(tuple, curBlockIndex, curOffset);
}

void Record::deleteLastTuple() {
    if (offset == 0) {
        blockIndex--;
        offset = (defaultBlockSize / tupleSize - 1) * tupleSize;
    }
    else offset -= tupleSize;
    deleteTuple(blockIndex, offset);
}
*/

bool Record::match(Tuple &tuple, vector<QueryDetail> &queryList) {
    Value *vpx, *vpy;
    for (vector<QueryDetail>::iterator itr = queryList.begin(); itr != queryList.end(); itr++) {
        ValueDetail vDetail = tuple.at(itr->targetPosition);
        switch (vDetail.type) {
            case INTTYPE: {
                int x = stoi(vDetail.value);
                int y = stoi(itr->val.value);
                vpx = new Int(x);
                vpy = new Int(y);
                break;
            }
            case FLOATTYPE: {
                float x = stof(vDetail.value);
                float y = stof(itr->val.value);
                vpx = new Float(x);
                vpy = new Float(y);
                break;
            }
            case CHARTYPE: {
                vpx = new String(vDetail.value);
                vpy = new String(itr->val.value);
                break;
            }
            default:
                return false;
        }
        switch (itr->type) {
            case LESS: {
                if (*vpx == *vpy || *vpy < *vpx)
                    return false;
                break;
            }
            case LESSANDEQUAL: {
                if (*vpy < *vpx)
                    return false;
                break;
            }
            case EQUAL: {
                if (!(*vpx == *vpy))
                    return false;
                break;
            }
            case NOTEQUAL: {
                if (*vpx == *vpy)
                    return false;
                break;
            }
            case MOREANDEQUAL: {
                if (*vpx < *vpy)
                    return false;
                break;
            }
            case MORE: {
                if (*vpx < *vpy || *vpx == *vpy)
                    return false;
                break;
            }
            default:
                return false;
        }
    }
    return true;
}

bool Record::createTable(vector<attributeType> attrType, vector<int> attrSize) {
    if (attrType.size() != attrSize.size())
        return false;
    exist = true;
    blockIndex = 0;
    numAttr = attrType.size();
    offset = 10 + numAttr * 2;
    attrInfo = new AttrInfo[numAttr];
    tupleSize = 1;
    for (int i = 0; i < numAttr; i++) {
        attrInfo[i].type = attrType.at(i);
        attrInfo[i].size = attrSize.at(i);
        tupleSize += attrSize.at(i);
    }
    offset = (offset / tupleSize + 1) * tupleSize;
    storeStatusAndInfo();
    return true;
}

bool Record::dropTable() {
    if (!exist)
        return false;
    exist = false;
    storeStatusAndInfo();
//    Buffer::remove(tableName + ".record");
    return true;
}

bool Record::insert(Tuple &valueList, vector<int> primaryOrUniquePosition) {
    // check whether unique or primary attribute duplicated
    dataBegin = 10 + numAttr * 2;
    dataBegin = (dataBegin / tupleSize + 1) * tupleSize;
    int curOffset = dataBegin;
    int curBlockIndex = 0;
    while (true) {
        if (curOffset + tupleSize >= defaultBlockSize) {
            curBlockIndex++;
            curOffset = 0;
        }
        if (curBlockIndex > blockIndex)
            break;
        else if (curBlockIndex == blockIndex && curOffset >= offset)
            break;
        Tuple curTuple;
        if (!loadTuple(curTuple, curBlockIndex, curOffset))
            continue;
        for (vector<int>::iterator itr = primaryOrUniquePosition.begin(); itr != primaryOrUniquePosition.end(); itr++) {
            if (curTuple.at(*itr).value == valueList.at(*itr).value)
                return false;
        }
    }
    storeTuple(valueList, blockIndex, offset);
    return true;
}

void Record::deleteTuple(vector<QueryDetail> &queryList) {
    dataBegin = 10 + numAttr * 2;
    dataBegin = (dataBegin / tupleSize + 1) * tupleSize;
    int curOffset = dataBegin;
    int curBlockIndex = 0;
    while (true) {
        if (curOffset + tupleSize >= defaultBlockSize) {
            curBlockIndex++;
            curOffset = 0;
        }
        if (curBlockIndex > blockIndex)
            break;
        else if (curBlockIndex == blockIndex && curOffset >= offset)
            break;
        Tuple curTuple;
        int tmpBlockIndex = curBlockIndex;
        int tmpOffset = curOffset;
        if (!loadTuple(curTuple, curBlockIndex, curOffset))
            continue;
        if (!match(curTuple, queryList))
            continue;
        deleteTuple(tmpBlockIndex, tmpOffset);
    }
}

void Record::select(vector<QueryDetail> &queryList, Table &table) {
    table.clear();
    dataBegin = 10 + numAttr * 2;
    dataBegin = (dataBegin / tupleSize + 1) * tupleSize;
    int curOffset = dataBegin;
    int curBlockIndex = 0;
    while (true) {
        if (curOffset + tupleSize >= defaultBlockSize) {
            curBlockIndex++;
            curOffset = 0;
        }
        else curOffset += tupleSize;
        if (curBlockIndex > blockIndex)
            break;
        else if (curBlockIndex == blockIndex && curOffset >= offset)
            break;
        Tuple curTuple;
        if (!loadTuple(curTuple, curBlockIndex, curOffset))
            continue;
        if (match(curTuple, queryList))
            table.push_back(curTuple);
    }
}











