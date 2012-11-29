#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "game.h"
using namespace std;
int AISeacher::maxDepth=4;

int main(int argc, char* argv[]){
	int depth=4;

	if(argc>1){
		depth=atoi(argv[1]);
	}
	/*
	for(int i=1;i<argc;++i){
		if(strcmp(argv[i],"-d")==0){
			assert((i+1)<argc);
			depth=atoi(argv[++i]);
		}
	}
	*/
	AISeacher::maxDepth=depth;
	Game game;
	game.selectMode();
	game.loop();	
	return 0;
}

