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

## API
  sqlError getAttributeSize(string tableName, vector<string> attributeName, vector<int>& attributeSize);
  bool create (string tableName, vector<rowDetail> row, int primaryKeyPosition = -1);
  bool drop (string tableName);
  bool createIndex (string tableName, string rowName, vector<string> indexName);
  bool dropIndex (string indexName);
  bool select (string tableName, vector<queryDetail> query);
  bool insert (string tableName, vector<valueDetail> values);
  bool delete (string tableName, vector<queryDetail> query);

  
