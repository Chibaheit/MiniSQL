# Buffer
- 结构: Buffer Manager管理打开文件的列表，并记录每个文件打开了哪些Block，这部分使用stl的unordered_map维护，也就是哈希。
- 对于每个缓冲的Block，Buffer Manager提供一块内存空间(char数组)给其他部分的程序访问和修改，并在释放时根据dirty bit写回。
- 替换策略选择了Clock，比较简单好写，在pin很少的情况下，均摊单次查找替换块是O(1)复杂度的，pin很少是符合实际应用环境需求的，所以使用Clock策略性能损失很小。
