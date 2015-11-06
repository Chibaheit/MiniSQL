#include "../src/record/record.h"
#include "../src/buffer/buffer.h"

int defaultNumBlocks = 2, defaultBlockSize = 4096;

int main() {
    Record testRecord1("tableA");
    Record testRecord2("tableB");
    
    // create table
    vector<attributeType> avt;
    vector<int> ivt;
    avt.push_back(INTTYPE);
    avt.push_back(CHARTYPE);
    avt.push_back(FLOATTYPE);
    ivt.push_back(4);
    ivt.push_back(6);
    ivt.push_back(4);
    testRecord1.createTable(avt, ivt);
    
    // insert
    Tuple tup;
    tup.push_back(ValueDetail(INTTYPE, "123"));
    tup.push_back(ValueDetail(CHARTYPE, "string"));
    tup.push_back(ValueDetail(FLOATTYPE, "1.3"));
    vector<int> pos;
    if (testRecord1.insert(tup, pos))
        cout << "succeed to insert 1" << endl;
    else cout << "fail to insert 1" << endl;
    pos.push_back(2);
    tup.pop_back();
    tup.push_back(ValueDetail(FLOATTYPE, "1.9"));
    if (testRecord1.insert(tup, pos))
        cout << "succeed to insert 2" << endl;
    else cout << "fail to insert 2" << endl;
    if (testRecord1.insert(tup, pos))
        cout << "succeed to insert 3" << endl;
    else cout << "fail to insert 3" << endl;
    pos.push_back(1);
    Tuple tup2;
    tup2.push_back(ValueDetail(INTTYPE, "123"));
    tup2.push_back(ValueDetail(CHARTYPE, "string"));
    tup2.push_back(ValueDetail(FLOATTYPE, "1.9"));
    if (testRecord1.insert(tup2, pos))
        cout << "succeed to insert 4" << endl;
    else cout << "fail to insert 4" << endl;
    
    // select
    QueryDetail qDetail(MOREANDEQUAL, ValueDetail(INTTYPE, "123"), 0);
    vector<QueryDetail> qv;
    qv.push_back(qDetail);
    Table table;
    testRecord1.select(qv, table);
    for (Table::iterator tableItr = table.begin(); tableItr != table.end(); tableItr++) {
        for (Tuple::iterator tupleItr = tableItr->begin(); tupleItr != tableItr->end(); tupleItr++)
            cout << tupleItr->value << " ";
        cout << endl;
    }
    
    // deleteTuple
    QueryDetail qDetail2(LESS, ValueDetail(FLOATTYPE, "1.31"), 2);
    vector<QueryDetail> qv2;
    qv2.push_back(qDetail2);
    testRecord1.deleteTuple(qv2);
    
    // drop table
    testRecord1.dropTable();
    return 0;
}