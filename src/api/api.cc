#include <iostream>
#include <unordered_map>
#include <string>
#include <iomanip>

#include "api.h"
#include "../interpreter/param.h"
#include "../common.h"
#include "../record/record.h"
#include "../catalog/catalog.h"

using namespace std;

namespace API {
    void createTable(string instruction) {
        // create table tableName
        auto args = PARAM::Split(instruction);
        if (args.size() < 5) {
            cout << "Error: Not enough argument" << endl;
            return;
        }
        string tableName;
        try {
          tableName = PARAM::Name(args[2]);
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
        Catalog *testCatalog = &Catalog::getInstance();
        // checkTableExist before table created
        if (testCatalog->checkTableExist(tableName)) {
          cout << ("Error: table " + tableName + " exists") << endl;
          return;
        }
        // creatTable
        vector<AttributeDetail> attributeList;
        for (size_t i = 3; i < args.size(); i += 2) {
            string name;
            try {
                name = PARAM::Name(args[i]);
            } catch (invalid_argument& e) {
                cout << "Syntax Error: " << e.what() << endl;
            }
            if (name == "primary") {
                if (i + 1 != args.size() && args[i + 1].find("key") == 0) {
                    string primaryKeyName = args[i + 1].substr(4, args[i + 1].length() - 1 - 4);
                    for (auto& u: attributeList) {
                        if (u.name == primaryKeyName) {
                            u.primary = true;
                        }
                    }
                    continue;
                }
            }
            if (args[i + 1].find("int") == 0) {
                attributeList.push_back(AttributeDetail(name, INTTYPE, 4));
            } else if (args[i + 1].find("float") == 0) {
                attributeList.push_back(AttributeDetail(name, FLOATTYPE, 4));
            } else if (args[i + 1].find("char") == 0 && args[i + 1][4] == '(' && (args[i + 1][args[i + 1].length() - 1] == ')' || args[i + 1][args[i + 1].length() - 2] == ')')) {
                int edPos = args[i + 1].length() - 1;
                if (args[i + 1][args[i + 1].length() - 1] != ')') {
                    --edPos;
                }
                int charSize;
                try {
                    charSize = PARAM::Int(args[i + 1].substr(5, edPos - 5));
                } catch (invalid_argument& e) {
                    cout << "Syntax Error: " << e.what() << endl;
                }
                AttributeDetail tmpAttribute(name, CHARTYPE, charSize);
                if (i + 2 != args.size() && args[i + 1][args[i + 1].length() - 1] != ',') {
                    if (args[i + 2].find("unique") == 0) {
                        tmpAttribute.unique = true;
                        i += 1;
                    } else {
                      cout << ("Unknown argument: " + args[i + 2]) << endl;
                      return;
                    }
                }
                attributeList.push_back(tmpAttribute);
            }
        }
        /*
        for (auto u: attributeList) {
          cout << u.name << " " << u.type << " " << u.length << " " << u.unique << " " << u.primary << endl;
        }
        */
        // call record -> createTable
        {
          Record record = Record(tableName);
          // create table
          vector<attributeType> avt;
          vector<int> ivt;
          for (auto u: attributeList) {
              avt.push_back(u.type);
              ivt.push_back(u.length);
          }
          record.createTable(avt, ivt);
        }
        // call catalog -> createTable
        {
            if (testCatalog->createTable(tableName, attributeList)) { // create table firstly
                cout << "create table(" << tableName << ") successfully" << endl;
            }
        }
    }

    void dropTable(string instruction) {
        // drop table tableName
        string tableName = instruction.substr(11);
        // cout << tableName << endl;
        try {
            tableName = PARAM::Name(tableName);
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
        Catalog *testCatalog = &Catalog::getInstance();
        // checkTableExist before table created
        if (!testCatalog->checkTableExist(tableName)) {
            cout << ("Error: table " + tableName + " dose not exist") << endl;
            return;
        }
        // call testCatalog -> dropTable
        {
            if (testCatalog->dropTable(tableName)) {
                cout << "drop table successfully" << endl;
            } else {
              cout << "fail to drop table" << endl;
            }
        }
        // call testRecord -> dropTable
        {
            Record record = Record(tableName);
            record.dropTable();
        }
    }

    void createIndex(string instruction) {
        // create index indexName on tableName (attributeName)
        auto args = PARAM::Split(instruction);
        if (args.size() != 6) {
            cout << "Error: Not enough argument" << endl;
            return;
        }
        if (args[3] != "on") {
            cout << "Syntax Error: Unknown SQL script" << endl;
        }
        string tableName, indexName, attributeName;
        try {
            indexName = PARAM::Name(args[2]);
            tableName = PARAM::Name(args[4]);
            attributeName = PARAM::Name(args[5]);
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
        Catalog *testCatalog = &Catalog::getInstance();
        // checkTableExist before table created
        if (!testCatalog->checkTableExist(tableName)) {
            cout << ("Error: table" + tableName + "does not exist") << endl;
            return;
        } else {
            cout << "table(" << tableName << ") exists" << endl;
        }
        // check a existent attribute
        if (!testCatalog->checkAttributeExist(tableName, attributeName)) {
            cout << "attribute(attrA) does not exist" << endl;
            return;
        }
        // checkIndexExist
        if (testCatalog->checkIndexExist(tableName, indexName)) {
            cout << "indexA exists" << endl;
            return;
        }
        if (testCatalog->createIndex(tableName, attributeName, indexName)) {
            cout << "create index(indexA) successfully" << endl;
        } else {
            cout << "fail to create index(indexA)" << endl;
        }
    }

    void dropIndex(string instruction) {
        // drop index indexName
        string indexName = instruction.substr(11);
        // cout << indexName << endl;
        try {
            indexName = PARAM::Name(indexName);
            //TODO: check index exist
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
    }

    void select(string instruction) {
        // select * from tableName || select * from tableName where attributeName relation Value
        string tableName, attributeName = "", val;
        auto args = PARAM::Split(instruction);
        bool filterFlag = false;
        if (args.size() == 8) {
            filterFlag = true;
        }
        if ((args.size() != 4 && args.size() != 8) || (args.size() == 8 && args[4] != "where")) {
            cout << "Error: unknown SQL script" << endl;
            return;
        }
        try {
            tableName = PARAM::Name(args[3]);
            if (filterFlag) {
                attributeName = PARAM::Name(args[5]);
                val = args[7];
                if (val[0] == '\'' || val[0] == '\"') {
                    val = val.substr(1, val.length() - 2);
                }
            }
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
        Catalog *testCatalog = &Catalog::getInstance();
        if (!testCatalog->checkTableExist(tableName)) {
            cout << ("Error: table " + tableName + " does not exist") << endl;
            return;
        }
        vector<QueryDetail> queryList;
        if (filterFlag) {
            int attrPos;
            if ((attrPos = testCatalog->checkAttributeExist(tableName, attributeName)) == -1) {
                cout << ("Error: the attribute " + attributeName + " does not exist") << endl;
                return;
            }
            vector<attributeType> attrType = testCatalog->getAttributeType(tableName, attributeName);
            ValueDetail value = ValueDetail(INTTYPE, val);
            try {
                switch (attrType[0]) {
                    case INTTYPE:
                        PARAM::Int(val);
                        value.type = INTTYPE;
                        break;
                    case CHARTYPE:
                        PARAM::Name(val);
                        value.type = CHARTYPE;
                        break;
                    case FLOATTYPE:
                        PARAM::Float(val);
                        value.type = FLOATTYPE;
                        break;
                    default:
                        break;
                }
            } catch (invalid_argument& e) {
                cout << "Syntax Error: " << e.what() << endl;
                return;
            }
            if (args[6] == "<") {
                queryList.push_back(QueryDetail(LESS, value, attrPos));
            } else if (args[6] == "<=") {
                queryList.push_back(QueryDetail(LESSANDEQUAL, value, attrPos));
            } else if (args[6] == "=") {
                queryList.push_back(QueryDetail(EQUAL, value, attrPos));
            } else if (args[6] == "!=") {
                queryList.push_back(QueryDetail(NOTEQUAL, value, attrPos));
            } else if (args[6] == ">") {
                queryList.push_back(QueryDetail(MOREANDEQUAL, value, attrPos));
            } else if (args[6] == ">=") {
                queryList.push_back(QueryDetail(MORE, value, attrPos));
            } else {
                cout << "Unmatched symbol: " << args[6] << endl;
                return;
            }
        }
        /*
        for (auto u: queryList) {
            cout << u.type << " " << u.val.type << " " << u.val.value << " " << u.targetPosition << endl;
        }
        */
        Table table;
        Record record(tableName);
        record.select(queryList, table);
        if (table.size() == 0) {
            cout << "No matching data" << endl;
        } else {
            cout << "Answer: " << endl;
            vector<string>attrName = testCatalog->getAttributeName(tableName);
            for (vector<string>::iterator u = attrName.begin(); u != attrName.end(); ++u) {
                cout << setw(19) << setiosflags(ios::left) << *u;
                if (u != attrName.end() - 1) {
                    cout << "|";
                } else {
                    cout << endl;
                }
            }
            for (int i = 0; i < attrName.size() * 20; ++i) {
                printf("%c", "-\n"[i == attrName.size() * 20 - 1]);
            }
            for (Table::iterator tableItr = table.begin(); tableItr != table.end(); tableItr++) {
                for (Tuple::iterator tupleItr = tableItr->begin(); tupleItr != tableItr->end(); tupleItr++) {
                    cout << setw(19) << setiosflags(ios::left) << tupleItr->value;
                    if (tupleItr != tableItr->end() - 1) {
                        cout << "|";
                    } else {
                        cout << endl;
                    }
                }
            }
        }
    }

    void insert(string instruction) {
        auto args = PARAM::Split(instruction);
        if (args.size() < 5 || args[3] != "values") {
            cout << "Syntax Error: Not enough argument" << endl;
            return;
        }
        string tableName;
        try {
            tableName = PARAM::Name(args[2]);
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
            return;
        }
        Catalog *testCatalog = &Catalog::getInstance();
        // checkTableExist before table created
        if (!testCatalog->checkTableExist(tableName)) {
            cout << ("Error: table " + tableName + " does not exist") << endl;
            return;
        }
        vector<attributeType> attrType = testCatalog->getAttributeType(tableName);
        if (args.size() != attrType.size() + 4) {
            cout << "Syntax Error: Not enough argument" << endl;
            return;
        }
        vector<int> pos = testCatalog->getPrimaryOrUniquePosition(tableName);
        Tuple tup;
        for (size_t i = 4; i < args.size(); ++i) {
            string val = args[i];
            if (val[val.length() - 1] == ',') {
                val = val.substr(0, val.length() - 1);
            }
            if (val[0] == '\'' || val[0] == '\"') {
                val = val.substr(1, val.length() - 2);
            }
            try {
                switch (attrType[i - 4]) {
                  case INTTYPE:
                      PARAM::Int(val);
                      tup.push_back(ValueDetail(INTTYPE, val));
                      break;
                  case CHARTYPE:
                      PARAM::Name(val);
                      tup.push_back(ValueDetail(CHARTYPE, val));
                      break;
                  case FLOATTYPE:
                      PARAM::Float(val);
                      tup.push_back(ValueDetail(FLOATTYPE, val));
                      break;
                  default:
                      break;
                }
            } catch (invalid_argument& e) {
                cout << "Syntax Error: " << e.what() << endl;
                return;
            }
        }
        /*
        for (auto u: tup) {
            cout << u.type << " " << u.value << endl;
        }
        */
        {
            Record record = Record(tableName);
            if (record.insert(tup, pos)) {
                cout << "succeed to insert" << endl;
            } else {
                cout << "fail to insert" << endl;
            }
        }
    }

    void deleteFrom(string instruction) {
        // delete from tableName || delete from tableName where
        string tableName, attributeName = "", val;
        auto args = PARAM::Split(instruction);
        bool filterFlag = false;
        if (args.size() == 7) {
            filterFlag = true;
        }
        if ((args.size() != 3 && args.size() != 7) || (args.size() == 7 && args[3] != "where")) {
            cout << "Error: unknown SQL script" << endl;
            return;
        }
        try {
            tableName = PARAM::Name(args[2]);
            if (filterFlag) {
                attributeName = PARAM::Name(args[4]);
                val = args[6];
                if (val[0] == '\'' || val[0] == '\"') {
                    val = val.substr(1, val.length() - 2);
                }
            }
        } catch (invalid_argument& e) {
            cout << "Syntax Error: " << e.what() << endl;
        }
        Catalog *testCatalog = &Catalog::getInstance();
        if (!testCatalog->checkTableExist(tableName)) {
            cout << ("Error: table " + tableName + " does not exist") << endl;
            return;
        }
        vector<QueryDetail> queryList;
        if (filterFlag) {
            int attrPos;
            if ((attrPos = testCatalog->checkAttributeExist(tableName, attributeName)) == -1) {
                cout << ("Error: the attribute " + attributeName + " does not exist") << endl;
                return;
            }
            vector<attributeType> attrType = testCatalog->getAttributeType(tableName, attributeName);
            ValueDetail value = ValueDetail(INTTYPE, val);
            try {
                switch (attrType[0]) {
                    case INTTYPE:
                        PARAM::Int(val);
                        value.type = INTTYPE;
                        break;
                    case CHARTYPE:
                        PARAM::Name(val);
                        value.type = CHARTYPE;
                        break;
                    case FLOATTYPE:
                        PARAM::Float(val);
                        value.type = FLOATTYPE;
                        break;
                    default:
                        break;
                }
            } catch (invalid_argument& e) {
                cout << "Syntax Error: " << e.what() << endl;
                return;
            }
            if (args[5] == "<") {
                queryList.push_back(QueryDetail(LESS, value, attrPos));
            } else if (args[5] == "<=") {
                queryList.push_back(QueryDetail(LESSANDEQUAL, value, attrPos));
            } else if (args[5] == "=") {
                queryList.push_back(QueryDetail(EQUAL, value, attrPos));
            } else if (args[5] == "!=") {
                queryList.push_back(QueryDetail(NOTEQUAL, value, attrPos));
            } else if (args[5] == ">") {
                queryList.push_back(QueryDetail(MOREANDEQUAL, value, attrPos));
            } else if (args[5] == ">=") {
                queryList.push_back(QueryDetail(MORE, value, attrPos));
            } else {
                cout << "Unmatched symbol: " << args[5] << endl;
                return;
            }
        }
        /*
        for (auto u: queryList) {
            cout << u.type << " " << u.val.type << " " << u.val.value << " " << u.targetPosition << endl;
        }
        */
        Record record(tableName);
        cout << record.deleteTuple(queryList) << " records deleted" << endl;
    }

    void quit(string instruction) {
        cout << "Bye~" << endl;
        exit(0);
    }

    void execfile(string instruction) {
        // execfile fileName
        string fileName = instruction.substr(9, instruction.length());
        // cout << fileName << endl;
        if (fileName.substr(fileName.length() - 4, fileName.length()) == ".sql") {
            // cout << fileName << endl;
        }
    }
}
