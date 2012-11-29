TAR=othello
SRC=main.cpp game.cpp board.cpp search.cpp
OBJS=main.o game.o board.o search.o
CFLAGS=-O3 -std=c++0x

all:$(TAR)

othello:$(OBJS)
	g++ $(CFLAGS) -o $@ $(OBJS) -lpthread

board.o: board.cpp weight.txt
	g++  $(CFLAGS) -c -o $@ $<

%.o: %.cpp
	g++  $(CFLAGS) -c -o $@ $<

clean:
	rm $(TAR) $(OBJS)
