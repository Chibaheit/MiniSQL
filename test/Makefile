COMMON_HEADERS := ../src/common.h
COMMON_OBJS := ../src/common.o
CC := g++
COP := -std=c++11 -g -Ofast

TARGET := testNode.run testInt.run testFloat.run testString.run testBuffer.run testIndex1.run testIndex2.run /
	testCatalog.run testRecord.run

.PHONY: all
.PHONY: %.run
all: $(TARGET)

BUFFER_PATH := ../src/buffer
BUFFER_OBJ := $(BUFFER_PATH)/buffer.o
BUFFER_HEADER := $(BUFFER_PATH)/buffer.h

testBuffer.o: testBuffer.cc $(COMMON_HEADERS) $(BUFFER_HEADER)
	$(CC) $(COP) -c $< -o $@

INDEX_PATH := ../src/index
INDEX_OBJ := $(INDEX_PATH)/index.o
INDEX_HEADER := $(INDEX_PATH)/index.h

testIndex%.o: testIndex%.cc $(COMMON_HEADERS) $(BUFFER_HEADER) $(INDEX_HEADER)
	$(CC) $(COP) -c $< -o $@

CATALOG_PATH := ../src/catalog
CATALOG_OBJ := $(CATALOG_PATH)/catalog.o
CATALOG_HEADER := $(CATALOG_PATH)/catalog.hpp
testCatalog.o: testCatalog.cpp $(COMMON_HEADERS) $(BUFFER_HEADER) $(CATALOG_HEADER)
	$(CC) $(COP) -c $< -o $@

RECORD_PATH := ../src/record
RECORD_OBJ := $(RECORD_PATH)/record.o
RECORD_HEADER := $(RECORD_PATH)/record.hpp
testRecord.o: testRecord.cpp $(COMMON_HEADERS) $(BUFFER_HEADER) $(RECORD_HEADER)
	$(CC) $(COP) -c $< -o $@	

%.o: %.cc $(COMMON_HEADERS)
	$(CC) $(COP) -c $< -o $@

testIndex%.exe: testIndex%.o $(INDEX_OBJ) $(BUFFER_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) $(INDEX_OBJ) -o $@

testBuffer.exe: testBuffer.o $(BUFFER_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) -o $@

testNode.exe: testNode.o $(INDEX_OBJ) $(BUFFER_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) $(INDEX_OBJ) -o $@

testCatalog.exe: testCatalog.o $(INDEX_OBJ) $(CATALOG_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) $(CATALOG_OBJ) -o $@

testRecord.exe: testRecord.o $(INDEX_OBJ) $(RECORD_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) $(RECORD_OBJ) -o $@

%.exe: %.o
	$(CC) $(COP) $< $(COMMON_OBJS) -o $@

testIndex%.run: testIndex%.exe
	./$<
	rm index.txt~

testBuffer.run: testBuffer.exe
	./testBuffer.exe
	rm test*.txt

testNode.run: testNode.exe
	./testNode.exe
	rm node.txt~

%.run: %.exe
	./$<

clean_o:
	rm -f *.o

clean_tmp:
	rm -f *~ *.swp *.output

clean_exe:
	rm -f *.out *.exe

clean: clean_tmp clean_o clean_exe
