COMMON_HEADERS := ./src/common.h
COMMON_OBJS := ./src/common.o
CC := clang++
COP := -std=c++11

TARGET := minisql
.PHONY: all
all: $(TARGET)

# buffer
BUFFER_PATH := ./src/buffer
BUFFER_OBJ := $(BUFFER_PATH)/buffer.o
BUFFER_HEADER := $(BUFFER_PATH)/buffer.h

buffer.o: $(BUFFER_PATH)/buffer.cc $(COMMON_HEADERS) $(BUFFER_HEADER)
	$(CC) $(COP) -c $< -o $(BUFFER_PATH)/$@

# index
INDEX_PATH := ./src/index
INDEX_OBJ := $(INDEX_PATH)/index.o
INDEX_HEADER := $(INDEX_PATH)/index.h

index.o: $(INDEX_PATH)/index.cc $(COMMON_HEADERS) $(BUFFER_HEADER) $(INDEX_HEADER)
	$(CC) $(COP) -c $< -o $(INDEX_PATH)/$@

# catalog

CATALOG_PATH := ./src/catalog
CATALOG_OBJ := $(CATALOG_PATH)/catalog.o
CATALOG_HEADER := $(CATALOG_PATH)/catalog.h

catalog.o: $(CATALOG_PATH)/catalog.cc $(COMMON_HEADERS) $(BUFFER_HEADER) $(CATALOG_HEADER)
	$(CC) $(COP) -c $< -o $(CATALOG_PATH)/$@

# record
RECORD_PATH := ./src/record
RECORD_OBJ := $(RECORD_PATH)/record.o
RECORD_HEADER := $(RECORD_PATH)/record.h

record.o: $(RECORD_PATH)/record.cc $(COMMON_HEADERS) $(BUFFER_HEADER) $(RECORD_HEADER)
	$(CC) $(COP) -c $< -o $(RECORD_PATH)/$@

# interpreter
INTERPRETER_PATH := ./src/interpreter
INTERPRETER_OBJ := $(INTERPRETER_PATH)/command.o $(INTERPRETER_PATH)/input.o $(INTERPRETER_PATH)/param.o $(INTERPRETER_PATH)/help.o
INTERPRETER_HEADERS := $(INTERPRETER_PATH)/command.h $(INTERPRETER_PATH)/input.h $(INTERPRETER_PATH)/param.h $(INTERPRETER_PATH)/help.h
# command
command.o: $(INTERPRETER_PATH)/command.cc $(INTERPRETER_PATH)/command.h
	$(CC) $(COP) -c $< -o $(INTERPRETER_PATH)/$@
# input
input.o: $(INTERPRETER_PATH)/input.cc $(INTERPRETER_PATH)/input.h
	$(CC) $(COP) -c $< -o $(INTERPRETER_PATH)/$@
# param
param.o: $(INTERPRETER_PATH)/param.cc $(INTERPRETER_PATH)/param.h
	$(CC) $(COP) -c $< -o $(INTERPRETER_PATH)/$@
# help
help.o: $(INTERPRETER_PATH)/help.cc $(INTERPRETER_PATH)/help.h
	$(CC) $(COP) -c $< -o $(INTERPRETER_PATH)/$@

# api
API_PATH := ./src/api
API_OBJ := $(API_PATH)/api.o $(API_PATH)/repl.o
API_HEADER := $(API_PATH)/api.h $(API_PATH)/repl.h
# api
api.o: $(API_PATH)/api.cc $(COMMON_HEADERS) $(RECORD_HEADER) $(CATALOG_HEADER) $(INTERPRETER_PATH)/param.h $(API_PATH)/api.h
	$(CC) $(COP) -c $< -o $(API_PATH)/$@
# repl
repl.o: $(API_PATH)/repl.cc $(INTERPRETER_PATH)/input.h $(INTERPRETER_PATH)/param.h $(API_PATH)/api.h $(BUFFER_PATH)
	$(CC) $(COP) -c $< -o $(API_PATH)/$@

main.o: main.cc $(INTERPRETER_HEADERS) $(API_HEADER) $(RECORD_HEADER) $(CATALOG_HEADER) $(COMMON_HEADERS) $(BUFFER_HEADER) $(INDEX_HEADER)
	$(CC) $(COP) -c $< -o $@

minisql: main.o buffer.o index.o catalog.o record.o command.o input.o param.o help.o api.o repl.o
	$(CC) $(COP) $< $(INTERPRETER_OBJ) $(API_OBJ) $(RECORD_OBJ) $(CATALOG_OBJ) $(COMMON_OBJS) $(BUFFER_OBJ) $(INDEX_OBJ) -o $@  -lreadline

clean_o:
	rm -f *.o

clean_tmp:
	rm -f *~ *.swp *.output

clean_exe:
	rm -f *.out *.exe

clean: clean_tmp clean_o clean_exe
