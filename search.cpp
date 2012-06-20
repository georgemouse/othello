#include <iostream>
#include <pthread.h>
#include "search.h"
#include "game.h"
#define max(a,b) (a>b)?a:b
using namespace std;

//Caller:refer to the root node
//player:current node color
//isMax:current node is max or min
Action AISeacher::ABSearch(Color caller,Color player,bool isMax,const Board* state,int alpha,int beta,int depth){

	//terminal test
	if(depth==maxDepth){
		return Action(state->eval(caller),-1);
	}
	else if(state->isFull()){
		return Action(state->utility(caller),-1);
	}


	MoveList validMove;
	state->getValidMove(player,validMove);
	MoveList::iterator it=validMove.begin();
	int bestMoveIndex=0;
	Color rival=Rival(player);
	int count=0;

	if(validMove.size()==0)
		return Action(state->utility(caller),-1);

	for(;it!=validMove.end();++it,++count){
		const Board* newState=state->result(player,*it);
		int nexVal=ABSearch(caller,rival,!isMax,newState,alpha,beta,depth+1).first;
		if(isMax){
			if(alpha<nexVal){
				alpha=nexVal;
				bestMoveIndex=count;
			}
		}
		else{
			if(beta>nexVal){
				beta=nexVal;
				bestMoveIndex=count;
			}
		}
		if(beta<=alpha)
			break;
	}
	if(isMax)
		return Action(alpha,bestMoveIndex);
	else
		return Action(beta,bestMoveIndex);
}

