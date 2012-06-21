#ifndef SEARCH_H
#define SEARCH_H

#include <unordered_map> 
#include "board.h"

typedef std::pair<int,int> Action;


typedef struct{
	int value;
	int bestIndex;
	int depth;
}StateInfo;
typedef std::unordered_map<long long,StateInfo> StateMap;
typedef std::pair<long long,StateInfo> StateKeyValPair;


class AISeacher{
public:
	Action ABSearch(Color caller,Color player,bool isMax,const Board* state,int alpha,int beta,int depth);

public:
	static int maxDepth;
};
#endif
