#include <iostream>
#include <pthread.h>
#include <map>
#include <algorithm>
#include "search.h"
#include "game.h"

using namespace std;

typedef pair<int,Board*> OrderState;

StateMap stateMap;


bool moveOredring(const OrderState& s1,const OrderState& s2){
	StateMap::iterator got1=stateMap.find(s1.second->boardHashkey);
	StateMap::iterator got2=stateMap.find(s2.second->boardHashkey);
	if(got1!=stateMap.end() && got2!=stateMap.end() ){
		return got1->second.value>got2->second.value;
	}
	return true;
	
}

inline StateKeyValPair makeStatePair(const Board* state,int stateVal,int bestIndex,int stateDepth){
	StateInfo i={stateVal,bestIndex,stateDepth};
	return StateKeyValPair(state->boardHashkey,i);
}

//Caller:refer to the root node
//player:current node color
//isMax:current node is max or min
Action AISeacher::ABSearch(Color caller,Color player,bool isMax,const Board* state,int alpha,int beta,int depth){

	//terminal test
	if(depth==0){
		int val=state->eval(caller);
		stateMap.insert(makeStatePair(state,val,-1,depth));
		return Action(val,-1);
	}
	else if(state->isFull()){
		return Action(state->utility(caller),-1);
	}

	StateMap::iterator got=stateMap.find(state->boardHashkey);
	if(got!=stateMap.end()){
		const StateInfo& info=got->second;
		if (info.depth>depth){
			return Action(info.value,info.bestIndex);
		}
	}

	MoveList validMove;
	state->getValidMove(player,validMove);
	int bestMoveIndex=0;
	Color rival=Rival(player);

	if(validMove.size()==0)
		return Action(state->utility(caller),-1);

	vector<OrderState>newStates(validMove.size());
	MoveList::iterator it=validMove.begin();
	for(int i=0;it!=validMove.end();++it,++i){
		newStates[i]=OrderState(i,state->result(player,*it));
	}
//	sort(newStates.begin(),newStates.end(),moveOredring);

	for(int i=0;i!=newStates.size();++i){
		Board* newState=newStates[i].second;
		int index=newStates[i].first;
		int nexVal=ABSearch(caller,rival,!isMax,newState,alpha,beta,depth-1).first;
		delete newState;
		if(isMax){
			if(alpha<nexVal){
				alpha=nexVal;
				bestMoveIndex=index;
			}
		}
		else{
			if(beta>nexVal){
				beta=nexVal;
				bestMoveIndex=index;
			}
		}
		if(beta<=alpha)
			break;
	}

	int val;
	if(isMax)
		val=alpha;
	else
		val=beta;

	stateMap[state->boardHashkey]=StateInfo{val,bestMoveIndex,depth};
	return Action(val,bestMoveIndex);
}

