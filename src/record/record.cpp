//
//  record.cpp
//  MiniSQL
//
//  Created by scn3 on 15/10/30.
//  Copyright © 2015年 scn3. All rights reserved.
//

#include "record.hpp"
#include "../buffer/buffer.h"
#include <sstream>

Record::Record(string tableName) : tableName(tableName) {
    loadStatusAndInfo();
}

Record::~Record() {
    if (attrInfo != NULL)
        delete []attrInfo;
}

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
    if (attrInfo != NULL)
        delete []attrInfo;
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
        for (int i = 0; i < 7; i++)
            data[i] = 0;
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
        for (int i = 0; i < numAttr; i++) {
            if (attrInfo[i].type == INTTYPE)
                data[offset++] = 0;
            else if (attrInfo[i].type == FLOATTYPE)
                data[offset++] = 1;
            else data[offset++] = 2;
            data[offset++] = attrInfo[i].size;
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
    int tmp = static_cast<int>(floatData);
    data[curOffset++] = (tmp & 0xff000000) >> 24;
    data[curOffset++] = (tmp & 0x00ff0000) >> 16;
    data[curOffset++] = (tmp & 0x0000ff00) >> 8;
    data[curOffset++] = (tmp & 0x000000ff);
}

void Record::storeData(string stringData, char *data, int &curOffset) {
    for (int i = 0; i < stringData.size(); i++)
        data[curOffset++] = stringData.at(i);
}

Tuple Record::loadTuple(int fromBlockIndex, int fromOffset) {
    Block *block = Buffer::access(tableName + ".record", fromBlockIndex);
    const char *data = block->constData();
    Tuple tuple;
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
                unsigned int tmp = str.at(0);
                tmp = tmp * 256 + str.at(1);
                tmp = tmp * 256 + str.at(2);
                tmp = tmp * 256 + str.at(3);
                float floatData = static_cast<float>(tmp);
                stringstream ss;
                ss << floatData;
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
    return tuple;
}

void Record::deleteTuple(int fromBlockIndex, int fromOffset) {
    Block *block = Buffer::access(tableName + ".record", fromBlockIndex);
    char *data = block->data();
    for (int i = 0; i < tupleSize; i++)
        data[fromOffset + i] = 0;
}

void Record::storeTuple(Tuple &tuple, int &fromBlockIndex, int &fromOffset) {
    if (fromOffset + tupleSize >= defaultBlockSize) {
        fromBlockIndex++;
        fromOffset = 0;
    }
    string fileName = tableName + ".record";
    Block *block = Buffer::access(fileName, fromBlockIndex);
    char *data = block->data();
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

Tuple Record::loadLastTuple() {
    int curBlockIndex, curOffset;
    if (offset == 0) {
        curBlockIndex = blockIndex - 1;
        curOffset = (defaultBlockSize / tupleSize - 1) * tupleSize;
    }
    else {
        curBlockIndex = blockIndex;
        curOffset = offset - tupleSize;
    }
    Tuple tuple = loadTuple(curBlockIndex, curOffset);
    return tuple;
}

void Record::deleteLastTuple() {
    if (offset == 0) {
        blockIndex--;
        offset = (defaultBlockSize / tupleSize - 1) * tupleSize;
    }
    else offset -= tupleSize;
    deleteTuple(blockIndex, offset);
}

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

void Record::createTable(int numAttribute, attributeType *attrType, int *attrSize) {
    exist = true;
    blockIndex = 0;
    offset = 10 + numAttribute * 2;
    numAttr = numAttribute;
    attrInfo = new AttrInfo[numAttr];
    tupleSize = 0;
    for (int i = 0; i < numAttribute; i++) {
        attrInfo[i].type = attrType[i];
        attrInfo[i].size = attrSize[i];
        tupleSize += attrSize[i];
    }
    offset = (offset / tupleSize + 1) * tupleSize;
    storeStatusAndInfo();
}

bool Record::dropTable() {
    if (!exist)
        return false;
    exist = false;
    storeStatusAndInfo();
    return true;
}

void Record::insert(Tuple &valueList) {
    storeTuple(valueList, blockIndex, offset);
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
        
        Tuple curTuple = loadTuple(curBlockIndex, curOffset);
        if (!match(curTuple, queryList)) {
            curOffset += tupleSize;
            continue;
        }
        deleteTuple(curBlockIndex, curOffset);
        Tuple lastTuple = loadLastTuple();
        deleteLastTuple();
        if (lastTuple.size() != 0)
            while (match(lastTuple, queryList)) {
                lastTuple = loadLastTuple();
                deleteLastTuple();
                if (lastTuple.size() == 0)
                    break;
            }
        if (lastTuple.size() != 0)
            storeTuple(lastTuple, curBlockIndex, curOffset);
        else break;
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
        
        Tuple curTuple = loadTuple(curBlockIndex, curOffset);
        if (match(curTuple, queryList))
            table.push_back(curTuple);
    }
}











