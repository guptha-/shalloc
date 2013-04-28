DFLAG = -g
WFLAG = -Wall
C11FLAG = -std=c++0x
THREADFLAG = -pthread

INCL = lib/shalloclib.hpp

CREATEDIR = mkdir -p obj bin

shalloclib: obj/shalloclib.o
	$(CREATEDIR)
	g++ -o bin/shalloclib $(WFLAG) obj/shalloclib.o -lm $(THREADFLAG)

obj/shalloclib.o: lib/shalloclib.cpp $(INCL)
	$(CREATEDIR)
	g++ -c lib/shalloclib.cpp -I inc $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/shalloclib.o
