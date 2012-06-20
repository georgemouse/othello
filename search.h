#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include <utility>
typedef std::pair<int,int> Action;

typedef struct ABPara{
	Color caller;
	Color player;
	bool isMax;
	const Board* state;
	int alpha;
	int beta;
	int depth;
}ABPara;

class AISeacher{
public:
	static int maxDepth;
	Action ABSearch(Color caller,Color player,bool isMax,const Board* state,int alpha,int beta,int depth);
};
#endif
