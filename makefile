DFLAG = -g
WFLAG = -Wall
C11FLAG = -std=c++0x
THREADFLAG = -pthread -lrt

INCL = lib/shalloclib.hpp

OBJL = obj/test.o\
       obj/shalloclib.o

CREATEDIR = mkdir -p obj bin

all: test

obj/shalloclib.o: lib/shalloclib.cpp $(INCL)
	$(CREATEDIR)
	g++ -c lib/shalloclib.cpp $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/shalloclib.o

obj/test.o: src/test.cpp $(INCL)
	$(CREATEDIR)
	g++ -c src/test.cpp $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/test.o

test: $(OBJL) $(INCL)
	$(CREATEDIR)
	g++ -o bin/test $(OBJL) $(C11FLAG) $(WFLAG) $(DFLAG) -lm $(THREADFLAG)

clean:
	rm -rf bin obj
