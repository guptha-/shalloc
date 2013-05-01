DFLAG = -g
WFLAG = -Wall
C11FLAG = -std=c++0x
THREADFLAG = -pthread

INCL = lib/shalloclib.hpp\
       lib/PracticalSocket.h

OBJL = obj/test.o\
       obj/shalloclib.o\
       obj/PracticalSocket.o

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

obj/PracticalSocket.o: lib/PracticalSocket.cpp $(INCL)
	$(CREATEDIR)
	g++ -c lib/PracticalSocket.cpp -I inc $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/PracticalSocket.o

clean:
	rm -rf bin obj