#ifndef GAME_H
#define GAME_H

#include <vector>
#include "board.h"
#include "search.h"

class Game{
	enum Mode{HVM=1,HVH,MVM};
private:
	int mode;
	Board board;
	std::vector<Move> validMove;
	AISeacher searcher;
public:
	void selectMode();
	void loop();
	void printPlayer(Color color);
	void showStatics();
	Move& humanSelect();
	Move& machineSelect(Color color);
};

#endif
