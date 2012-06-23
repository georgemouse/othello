#include <iostream>
#include <iomanip>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "search.h"
#include "board.h"
using namespace std;
MoveList Board::corners=MoveList(4);
MoveList Board::XSquares=MoveList(12);
MoveList Board::Edges=MoveList(30);
Hashkey* Board::pieceHashKey=0;
int Board::endGamePieceCount=15;

HeuristicWeight Board::heuristicWeight[2]=
{
	// pieceW	cornerW	XSquareW	edgeW	useEndgemeSolver
		1,		3,		-2,			1,		false,	//black
		1,		3,		-2,			1,		false	//white
};

//generate a 64 bit key
long long ranGenkey(){
	long long key(rand());
	key<<=31;
	key+=rand();
	return key;
}

GameState Board::getState()const{
	if(pieceCount<10)
		return OPENGAME;
	else if(pieceCount>boardSize-endGamePieceCount)
		return ENDGAME;
	else 
		return MIDGAME;
}

Board::~Board(){
	delete board;
}
Board::Board(int sideLength){
	this->sideLength=sideLength;
	boardSize=sideLength*sideLength;
	board=new Color[boardSize];
	for(int i=0;i<boardSize;++i){
		board[i]=EMPTY;
	}

	initTranpositionTable();

	int centerx=sideLength/2;
	int centery=sideLength/2;
	setPiece(Position(centerx,centery),BLACK);
	setPiece(Position(centerx-1,centery-1),BLACK);
	setPiece(Position(centerx,centery-1),WHITE);
	setPiece(Position(centerx-1,centery),WHITE);
	pieceCount=4;

	initSpecialLocation();

}

void Board::initTranpositionTable(){
	//init tranposition table
	boardHashkey=0;
	int tableSize=2*boardSize;
	pieceHashKey=new Hashkey[tableSize];
	//ensure that we use a 64 bit hash key
	assert(sizeof(Hashkey)==8);
	for(int i=0;i<tableSize;++i){
		pieceHashKey[i]=ranGenkey();
	}
}
void Board::initSpecialLocation(){
	//corner
	int last=sideLength-1;
	corners[0]=Move(0,0);
	corners[1]=Move(0,last);
	corners[2]=Move(last,0);
	corners[3]=Move(last,last);

	//XSquares
	int Xlast=last-1;
	XSquares[0]=Move(1,1);
	XSquares[1]=Move(1,0);
	XSquares[2]=Move(0,1);
	XSquares[3]=Move(1,Xlast);
	XSquares[4]=Move(1,last);
	XSquares[5]=Move(0,Xlast);
	XSquares[6]=Move(Xlast,1);
	XSquares[7]=Move(Xlast,0);
	XSquares[8]=Move(last,1);
	XSquares[9]=Move(Xlast,Xlast);
	XSquares[10]=Move(Xlast,last);
	XSquares[11]=Move(last,Xlast);

	XSquares[0].addOffset(Position(0,0));
	XSquares[1].addOffset(Position(0,0));
	XSquares[2].addOffset(Position(0,0));
	XSquares[3].addOffset(Position(0,last));
	XSquares[4].addOffset(Position(0,last));
	XSquares[5].addOffset(Position(0,last));
	XSquares[6].addOffset(Position(last,0));
	XSquares[7].addOffset(Position(last,0));
	XSquares[8].addOffset(Position(last,0));
	XSquares[9].addOffset(Position(last,last));
	XSquares[10].addOffset(Position(last,last));
	XSquares[11].addOffset(Position(last,last));

	//Edges
	int col=0,row=0;
	for(int i=0; ;i++)
	{
		Edges[i]=Move(row,col);

		if(row==last && col==last) break;
		else if(col==last) { col=0;++row;}
		else if(row==0||row==last) ++col;
		else col=last;
	}
}

Board::Board(const Board& that){
	this->sideLength=that.sideLength;
	this->boardSize=that.boardSize;
	this->boardHashkey=that.boardHashkey;
	board=new Color[boardSize];
	memcpy(this->board,that.board,boardSize);
}

void Board::print() const{
	cout<<"  ";
	for(int col=0;col<sideLength;col++)
//        cout<<setw(2)<<col;
        cout<<setw(2)<<col+1;
	cout<<endl<<"--";
	for(int col=0;col<sideLength;col++)
		cout<<"--";
	cout<<endl;


	for(int row=0;row<sideLength;++row){
//        cout<<row<<"|";
		cout<<row+1<<"|";
		for(int col=0;col<sideLength;++col)
			cout<<left<<setw(2)<<board[rc(row,col)];
		cout<<endl;
	}

}

bool Board::isFull() const{
	return (pieceCount==boardSize);

	for(int row=0;row<sideLength;++row)
		for(int col=0;col<sideLength;++col)
			if(board[rc(row,col)]==EMPTY)
				return false;

	return true;
}

//caller is the root node of the search tree
int Board::utility(Color caller) const{
	int blackPiece,whitePiece;
	countPiece(blackPiece,whitePiece);
	if(caller==BLACK)
		return blackPiece-whitePiece;
	else
		return whitePiece-whitePiece;
}

int Board::eval(Color caller) const{
	HeuristicWeight& weight=(caller==BLACK)?heuristicWeight[0]:heuristicWeight[1];

	int score;
	int blackPiece,whitePiece;
	int pieceScore;
	countPiece(blackPiece,whitePiece);
	if(caller==BLACK)
		pieceScore= blackPiece-whitePiece;
	else
		pieceScore= whitePiece-blackPiece;

	if(weight.useEndgemeSolver && getState()==ENDGAME)
		return pieceScore;


	if(isFull())
		return pieceScore;

	Color rival=Rival(caller);

	int cornersCount=positionCount(caller,corners)-positionCount(rival,corners);
	int edgeCount=positionCount(caller,Edges)-positionCount(rival,Edges);
	int xcount=positionCount(caller,XSquares)-positionCount(rival,XSquares);
	int bonus=0;

	score=	weight.pieceW*pieceScore+
	  		weight.edgeW*edgeCount+
	  		weight.cornerW*cornersCount+
	  		weight.XSquareW*xcount;
	return score;

}

int Board::positionCount(Color my,MoveList& positions) const{
	int occupy=0;
	MoveList::iterator mit=positions.begin();
	for(;mit!=positions.end();++mit){
		if(piece((*mit).pos)!=my)
			continue;
		PositionList checkList=(*mit).getOffsets();
		PositionList::iterator pit=checkList.begin();
		if(checkList.size()==0){
			++occupy;
			continue;
		}
		bool support=false;
		for(;pit!=checkList.end();++pit){
			if(piece(*pit)==my){
				support=true;
				break;
			}
		}
		if(!support)
			++occupy;
	}
	return occupy;
}

void Board::countPiece(int& blackPiece,int& whitePiece) const{
	blackPiece=whitePiece=0;
	for(int row=0;row<sideLength;++row){
		for(int col=0;col<sideLength;++col){
			if(board[rc(row,col)]==BLACK)
				++blackPiece;
			else if(board[rc(row,col)]==WHITE)
				++whitePiece;
		}
	}
}

void Board::getValidMove(Color my,MoveList& validMove) const{
	static Position offsets[8]={
		Position(-1,-1),Position(-1,0),Position(-1,1),
		Position(0,-1),Position(0,1),
		Position(1,-1),Position(1,0),Position(1,1)
	};
	for(int row=0;row<sideLength;++row){
		for(int col=0;col<sideLength;++col){
			Move m(row,col);
			Position current(row,col);
			if(piece(current)!=EMPTY)
				continue;
			for(int i=0;i<8;++i){
				if(hasClosure(my,current,offsets[i]))
					m.addOffset(offsets[i]);
			}
			if(m.getOffsets().size()!=0)
				validMove.push_back(m);
		}
	}
}

void Board::doMove(Color my,Move& move){
	Color rival=Rival(my);
	list<Position>& offsets=move.getOffsets();
	list<Position>::iterator it=offsets.begin();
	Position p=move.pos;

	Position pos=move.pos;
	assert(piece(pos)==EMPTY);
	setPiece(pos,my);
	it=offsets.begin();
	for(;it!=offsets.end();++it){
		pos=move.pos+(*it);
		while(piece(pos)!=my){
			setPiece(pos,my);
			pos+=*it;
		}
	}
	++pieceCount;
}

Board* Board::result(Color my,Move& move) const{
	Board* copy=new Board(*this);
	copy->doMove(my,move);
	return copy;
}

void Board::setPiece(const Position& pos,Color c){
	assert(c!=EMPTY);

	//handle hash table
	int piece=(c==WHITE)?0:1;
	int posind=rc(pos.x,pos.y);
	//if empty, just put c on(XOR one time)
	//if not, take rival of c off and put c on
	if(board[posind]!=EMPTY)
		boardHashkey^=pieceHashKey[(!piece)*boardSize+posind];
	boardHashkey^=pieceHashKey[piece*boardSize+posind];

	board[posind]=c;
}

bool Board::hasClosure(Color my,Position pos,const Position& offset)const{
	assert(piece(pos)==EMPTY);
	Color rival=Rival(my);
	pos+=offset;
	if(!isValidPosition(pos) || piece(pos)!=rival)
		return false;

	while(isValidPosition(pos)){
		if(piece(pos)==EMPTY)
			return false;
		if(piece(pos)==my)
			return true;
		pos+=offset;
	}
	return false;
}

inline bool Board::isValidPosition(const Position& p) const{
	if(p.x<0 || p.y<0 || p.x>=sideLength || p.y>=sideLength)
		return false;
	return true;
}
