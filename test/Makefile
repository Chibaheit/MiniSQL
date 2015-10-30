COMMON_HEADERS := ../src/common.h
COMMON_OBJS := ../src/common.o
CC := g++
COP := -std=c++11 -Ofast

TARGET := testInt.run testFloat.run testString.run

all: $(TARGET)

%.o: %.cc $(COMMON_HEADERS)
	$(CC) $(COP) $< -c -o $@

%.exe: %.o
	$(CC) $(COP) $< $(COMMON_OBJS) -o $@

%.run: %.exe
	./$<

clean_o:
	rm -f *.o

clean_tmp:
	rm -f *~ *.swp *.output

clean_exe:
	rm -f *.out *.exe

clean: clean_tmp clean_o clean_exe

clean_not_src: clean_tmp clean_o clean_exe
