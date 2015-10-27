## Interpreter 
-> Catalog
```cc

  /*
  * Interpreter will check the SQL instruction is vaild or not
  * Ensure calling the function from Catalog is always lawful.
  */
  bool checkTableExist(string tableName);
    
  bool checkAttributeExist(string tableName, string attributeName);

  vector<string> getIndexList(string tableName, string attributeName);

  vector<string> getAttributeType(string tableName); 
```

## API
```cc

  vector<int> getAttributeSize(string tableName, vector<string> attributeName);
  
  bool create (string tableName, vector<attributeDetail> attributeList, int primaryKeyPosition = -1);
  
  bool drop (string tableName);
  
  bool createIndex (string tableName, string attributeName, vector<string> indexList);
  
  bool dropIndex (string indexName);
  
  bool select (string tableName, vector<queryDetail> queryList);
  
  bool insert (string tableName, vector<valueDetail> valueList);
  
  bool delete (string tableName, vector<queryDetail> queryList);
  
```
  
