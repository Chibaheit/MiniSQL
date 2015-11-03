COMMON_HEADERS := common.h buffer/buffer.h
COMMON_OBJS :=
CC := g++
COP := -std=c++11 -g -Ofast

TARGET := common.o buffer/buffer.o index/index.o

all: $(TARGET)

%.o: %.cc %.h $(COMMON_HEADERS)
	$(CC) $(COP) -c $< -o $@

%.exe: %.o
	$(CC) $(COP) $< $(COMMON_OBJS) -o $@

clean_o:
	rm -f *.o */*.o

clean_tmp:
	rm -f *~ *.swp *.output

clean_exe:
	rm -f *.out *.exe

clean: clean_tmp clean_o clean_exe
