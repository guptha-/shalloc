DFLAG = -g
WFLAG = -Wall
C11FLAG = -std=c++0x
THREADFLAG = -pthread

INCL = lib/shalloclib.hpp

CREATEDIR = mkdir -p obj bin

all: obj/shalloclib.o test

obj/shalloclib.o: lib/shalloclib.cpp $(INCL)
	$(CREATEDIR)
	g++ -c lib/shalloclib.cpp -I inc $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/shalloclib.o

test: src/test.cpp $(INCL)
	$(CREATEDIR)
	g++ -o bin/test src/test.cpp obj/shalloclib.o -I inc $(C11FLAG) $(WFLAG) $(DFLAG) -lm $(THREADFLAG)