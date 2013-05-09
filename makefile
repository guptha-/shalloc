DFLAG = -g
WFLAG = -Wall
C11FLAG = -std=c++0x
THREADFLAG = -pthread -lrt

INCL = lib/shalloclib.hpp

OBJL = obj/shalloclib.o

CREATEDIR = mkdir -p obj bin test/recent

all: testmain test1 test2 test3 test4

testmain: test/test.cpp
	$(CREATEDIR)
	g++ -o bin/test test/test.cpp

obj/shalloclib.o: lib/shalloclib.cpp $(INCL)
	$(CREATEDIR)
	g++ -c lib/shalloclib.cpp $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/shalloclib.o

obj/test1.o: src/test1.cpp $(INCL)
	$(CREATEDIR)
	g++ -c src/test1.cpp $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/test1.o
test1: obj/test1.o $(OBJL) $(INCL)
	$(CREATEDIR)
	g++ -o bin/test1 obj/test1.o $(OBJL) $(C11FLAG) $(WFLAG) $(DFLAG) -lm $(THREADFLAG)

obj/test2.o: src/test2.cpp $(INCL)
	$(CREATEDIR)
	g++ -c src/test2.cpp $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/test2.o
test2: obj/test2.o $(OBJL) $(INCL)
	$(CREATEDIR)
	g++ -o bin/test2 obj/test2.o $(OBJL) $(C11FLAG) $(WFLAG) $(DFLAG) -lm $(THREADFLAG)

obj/test3.o: src/test3.cpp $(INCL)
	$(CREATEDIR)
	g++ -c src/test3.cpp $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/test3.o
test3: obj/test3.o $(OBJL) $(INCL)
	$(CREATEDIR)
	g++ -o bin/test3 obj/test3.o $(OBJL) $(C11FLAG) $(WFLAG) $(DFLAG) -lm $(THREADFLAG)

obj/test4.o: src/test4.cpp $(INCL)
	$(CREATEDIR)
	g++ -c src/test4.cpp $(C11FLAG) $(WFLAG) $(DFLAG) -o obj/test4.o
test4: obj/test4.o $(OBJL) $(INCL)
	$(CREATEDIR)
	g++ -o bin/test4 obj/test4.o $(OBJL) $(C11FLAG) $(WFLAG) $(DFLAG) -lm $(THREADFLAG)

clean:
	rm -rf bin obj
