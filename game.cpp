#include <iostream>
#include "game.h"
#include "search.h"
#include <limits.h>
using namespace std;

char cls[]={0x1b,'[','2','J'};


void Game::loop(){
	Color player=BLACK;
	//human always starts first
	bool isHuman=(mode!=MVM)?true:false;
	//to check double pass i.e. no move can be done by both players
	bool prevPass=false;
	while(true){
//		cout<<cls;
		cout<<endl;
		board.print();	
		if(board.isFull())
			break;
		printPlayer(player);
		validMove.clear();
		board.getValidMove(player,validMove);
		if(validMove.size()==0){
			//he pass you pass, end of game
			if(prevPass)
				break;
			prevPass=true;
			cout<<"no valid move,pass"<<endl;
		}
		else{
			prevPass=false;
			if(isHuman){
				board.doMove(player,humanSelect());
			}
			else{
				Move& bestMove=machineSelect(player);
				board.doMove(player,bestMove);
//                cout<<"choose ("<<bestMove.pos.x<<","<<bestMove.pos.y<<")"<<endl;
				cout<<"choose ("<<bestMove.pos.x+1<<","<<bestMove.pos.y+1<<")"<<endl;
			}
		}
		
		player=Rival(player);
		//only need to switch human/machine in HVM mode
		if(mode==HVM)
			isHuman=!isHuman;
	}
	showStatics();
}

Move& Game::humanSelect(){
	int row,col;
	while(true){
		cout<<"enter row & col:"<<endl;
		cin>>row>>col;
		/************/
		--row;
		--col;
		/***/
		MoveList::iterator it=validMove.begin();
		for(;it!=validMove.end();++it){
			if(row==(*it).pos.x && col==(*it).pos.y)
				return *it;
		}
		cout<<"invalid position!"<<endl;
	}
}

Move& Game::machineSelect(Color my){
	int alpha,beta;
	alpha=INT_MIN;
	beta=INT_MAX;
	//always start search as max node
	int searchDepth=AISeacher::maxDepth;
//    if(board.getState()==ENDGAME)
//        searchDepth=Board::endGamePieceCount;
	int bestIndex=searcher.ABSearch(my,my,true,&board,alpha,beta,searchDepth).second;

	return validMove[bestIndex];
}

void Game::selectMode(){
	cout<<"Select a game mode\n\
	  1. human vs machine\n\
	  2. human vs human\n\
	  3. machine va machine\n";
	cin>>mode;
	if(mode==HVM)
		cout<<"human vs machine mode selected\n";
	else if(mode==HVH)
		cout<<"human vs human mode selected\n";
	else if(mode==MVM)
		cout<<"machine vs machine mode selected\n";

}


void Game::printPlayer(Color color){
	if(color==BLACK)
		cout<<"Black player's turn(X)"<<endl;
	else
		cout<<"White player's turn(O)"<<endl;
}


void Game::showStatics(){
	int blackPiece;
	int whitePiece;
	board.countPiece(blackPiece,whitePiece);
	cout<<"black: "<<blackPiece<<endl;
	cout<<"white: "<<whitePiece<<endl;
	if(blackPiece>whitePiece)
		cout<<"black win "<<blackPiece-whitePiece<<" piece"<<endl;
	else if(blackPiece<whitePiece)
		cout<<"white win "<<whitePiece-blackPiece<<" piece"<<endl;
	else
		cout<<"Tie game"<<endl;
}
