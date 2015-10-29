COMMON_HEADERS := common.h
COMMON_OBJS :=
CC := g++
COP := -std=c++11 -Ofast

TARGET := buffer/buffer.o

all: $(TARGET)

%.o: %.cpp $(COMMON_HEADERS)
	$(CC) $(COP) $< -c -o $@

%.exe: %.o
	$(CC) $(COP) $< $(COMMON_OBJS) -o $@

clean_o:
	rm -f *.o

clean_tmp:
	rm -f *~ *.swp *.output

clean_exe:
	rm -f *.out *.exe

clean: clean_tmp clean_o clean_exe

clean_not_src: clean_tmp clean_o clean_exe
