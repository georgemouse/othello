#include <iostream>
#include <pthread.h>
#include <map>
#include <algorithm>
#include <assert.h>
#include "search.h"
#include "game.h"

#define max(a,b) (a>b)?a:b
#define min(a,b) (a<b)?a:b
using namespace std;

typedef pair<int,Board*> OrderState;

//StateMap minStateMap;
//StateMap maxStateMap;
StateMap stateMap;

bool moveOredring(const OrderState& s1,const OrderState& s2){
	int score1,score2;
	/*
	StateMap::iterator got1=stateMap.find(s1.second->boardHashkey);
	StateMap::iterator got2=stateMap.find(s2.second->boardHashkey);
	if(got1!=stateMap.end())
		score1=got1->second.value;
	else
		score1=s1.second->eval(WHITE);

	if(got2!=stateMap.end())
		score2=got2->second.value;
	else
		score2=s2.second->eval(WHITE);
		*/
	score1=s1.second->eval(WHITE);
	score2=s2.second->eval(WHITE);

	return (score1>score2);
}


int cutcount=0;

inline StateKeyValPair makeStatePair(const Board* state,int stateVal,int bestIndex,int stateDepth){
	StateInfo i={stateVal,bestIndex,stateDepth};
	return StateKeyValPair(state->boardHashkey,i);
}

//Caller:refer to the root node
//player:current node color
//isMax:current node is max or min
Action AISeacher::ABSearch(Color caller,Color player,bool isMax,const Board* state,int alpha,int beta,int depth){

//    StateMap& stateMap=isMax?maxStateMap:minStateMap;
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
			++cutcount;
			cout<<depth<<endl;
			return Action(info.value,info.bestIndex);
		}
	}


	MoveList validMove;
	state->getValidMove(player,validMove);
	int bestMoveIndex=0;
	Color rival=Rival(player);

	if(validMove.size()==0)
		return Action(state->eval(caller),-1);

	vector<OrderState>newStates;
	MoveList::iterator it=validMove.begin();
	for(int i=0;it!=validMove.end();++it,++i){
		newStates.push_back(OrderState(i,state->result(player,*it)));
	}
    //sort(newStates.begin(),newStates.end(),moveOredring);

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


