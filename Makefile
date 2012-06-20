TAR=othello
SRC=main.cpp game.cpp board.cpp search.cpp
OBJS=main.o game.o board.o search.o

all:$(TAR)

othello:$(OBJS)
	g++ -g -o $@ $(OBJS) -lpthread
%.o: %.cpp
	g++ -g -c -o $@ $<

clean:
	rm $(TAR) $(OBJS)
