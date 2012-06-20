#include <iostream>
#include <stdlib.h>
#include "game.h"
using namespace std;
int AISeacher::maxDepth=4;

int main(int argc, char* argv[]){
	int depth=4;
	bool seri=false;
	if(argc>1){
		depth=atoi(argv[1]);
	}
	AISeacher::maxDepth=depth;
	Game game;
	game.selectMode();
	game.loop();	
	return 0;
}

