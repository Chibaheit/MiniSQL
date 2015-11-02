COMMON_HEADERS := ../src/common.h
COMMON_OBJS := ../src/common.o
CC := g++
COP := -std=c++11 -g

TARGET := testNode.run testIndex.run testInt.run testFloat.run testString.run testBuffer.run

.PHONY: all
all: $(TARGET)

%.o: %.cc $(COMMON_HEADERS)
	$(CC) $(COP) $< -c -o $@

%.exe: %.o
	$(CC) $(COP) $< $(COMMON_OBJS) -o $@

BUFFER_PATH := ../src/buffer
BUFFER_OBJ := $(BUFFER_PATH)/buffer.o
testBuffer.exe: testBuffer.o $(BUFFER_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) -o $@

INDEX_PATH := ../src/index
INDEX_OBJ := $(INDEX_PATH)/index.o
testIndex.exe: testIndex.o $(INDEX_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) $(INDEX_OBJ) -o $@
testNode.exe: testNode.o $(INDEX_OBJ)
	$(CC) $(COP) $< $(COMMON_OBJS) $(BUFFER_OBJ) $(INDEX_OBJ) -o $@

%.run: %.exe
	./$<

testBuffer.run: testBuffer.exe
	./testBuffer.exe
	rm test*.txt

testIndex.run: testIndex.exe
	./testIndex.exe
	rm index.txt~

testNode.run: testNode.exe
	./testNode.exe
	rm node.txt~

clean_o:
	rm -f *.o

clean_tmp:
	rm -f *~ *.swp *.output

clean_exe:
	rm -f *.out *.exe

clean: clean_tmp clean_o clean_exe
