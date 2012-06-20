#include <iostream>
#include <iomanip>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "board.h"
using namespace std;
MoveList Board::corners=MoveList(4);
MoveList Board::XSquares=MoveList(4);
Hashkey* Board::pieceHashKey=0;

//generate a 64 bit key
long long ranGenkey(){
	long long key(rand());
	key<<=31;
	key+=rand();
	return key;
}

Board::~Board(){
	delete board;
}
Board::Board(int boardSize){
	this->boardSize=boardSize;
	int size=boardSize*boardSize;
	board=new Color[size];
	for(int i=0;i<size;++i){
		board[i]=EMPTY;
	}

	initTranpositionTable();
	
	int centerx=boardSize/2;
	int centery=boardSize/2;
	setPiece(Position(centerx,centery),BLACK);
	setPiece(Position(centerx-1,centery-1),BLACK);
	setPiece(Position(centerx,centery-1),WHITE);
	setPiece(Position(centerx-1,centery),WHITE);

	initSpecialLocation();

}

void Board::initTranpositionTable(){
	//init tranposition table
	boardHashkey=0;
	int tableSize=2*boardSize*boardSize;
	pieceHashKey=new Hashkey[tableSize];
	//ensure that we use a 64 bit hash key
	assert(sizeof(Hashkey)==8);
	for(int i=0;i<tableSize;++i){
		pieceHashKey[i]=ranGenkey();
	}
}
void Board::initSpecialLocation(){
	int last=boardSize-1;
	corners[0]=Move(0,0);
	corners[1]=Move(0,last);
	corners[2]=Move(last,0);
	corners[3]=Move(last,last);

	int Xlast=last-1;
	XSquares[0]=Move(1,1);
	XSquares[1]=Move(1,Xlast);
	XSquares[2]=Move(Xlast,1);
	XSquares[3]=Move(Xlast,Xlast);
	XSquares[0].addOffset(Position(0,0));
	XSquares[1].addOffset(Position(0,last));
	XSquares[2].addOffset(Position(last,0));
	XSquares[3].addOffset(Position(last,last));
}

Board::Board(const Board& that){
	this->boardSize=that.boardSize;
	int size=boardSize*boardSize;
	board=new Color[size];
	memcpy(this->board,that.board,size);
}

void Board::print() const{
	cout<<" ";
	for(int col=0;col<boardSize;col++)
		cout<<setw(2)<<col;
	cout<<endl<<"--";
	for(int col=0;col<boardSize;col++)
		cout<<"--";
	cout<<endl;


	for(int row=0;row<boardSize;++row){
		cout<<row<<"|";
		for(int col=0;col<boardSize;++col)
			cout<<left<<setw(2)<<board[rc(row,col)];
		cout<<endl;
	}

}

bool Board::isFull() const{
	for(int row=0;row<boardSize;++row)
		for(int col=0;col<boardSize;++col)
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
	int score;
	int blackPiece,whitePiece;
	int pieceScore;
	countPiece(blackPiece,whitePiece);
	if(caller==BLACK)
		pieceScore= blackPiece-whitePiece;
	else
		pieceScore= whitePiece-blackPiece;

	if(isFull())
		return pieceScore;

	Color rival=Rival(caller);

	int cornersCount=positionCount(caller,corners)-positionCount(rival,corners);
	int xcount=positionCount(caller,XSquares)-positionCount(rival,XSquares);
	int bonus=10*cornersCount+(-10)*xcount;

	if(caller==WHITE){
		score=pieceScore+bonus;
		//score=pieceScore;
	}
	else{
		score=pieceScore;
	}
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
	for(int row=0;row<boardSize;++row){
		for(int col=0;col<boardSize;++col){
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
	for(int row=0;row<boardSize;++row){
		for(int col=0;col<boardSize;++col){
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
}

Board* Board::result(Color my,Move& move) const{
	Board* copy=new Board(*this);
	copy->doMove(my,move);
	return copy;
}

void Board::setPiece(const Position& pos,Color c){
	static int size=boardSize*boardSize;
	assert(c!=EMPTY);

	//handle hash table
	int piece=(c==WHITE)?0:1;
	int posind=rc(pos.x,pos.y);
	//if empty, just put c on(XOR one time)
	//if not, take rival of c off and put c on
	if(board[posind]!=EMPTY)
		boardHashkey^=pieceHashKey[(!piece)*size+posind];
	boardHashkey^=pieceHashKey[piece*size+posind];

	board[posind]=c;
}

bool Board::hasClosure(Color my,Position pos,const Position& offset)const{
	assert(piece(pos)==EMPTY);
	Color rival=Rival(my);
	pos+=offset;
	if(!isValidPosition(pos) || piece(pos)!=rival)
		return false;

	while(isValidPosition(pos)){
		if(piece(pos)==my)
			return true;
		pos+=offset;
	}
	return false;
}

bool Board::isValidPosition(const Position& p) const{
	if(p.x<0 || p.y<0 || p.x>=boardSize || p.y>=boardSize)
		return false;
	return true;
}

