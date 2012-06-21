#ifndef BOARD_H
#define BOARD_H

#define rc(row,col) (row)*sideLength+col
#include <vector>
#include <list>
class Move;
class Position;
typedef char Color;
typedef std::vector<Move> MoveList;
typedef std::list<Position> PositionList;
typedef long long int Hashkey;
enum Piece{EMPTY='-',BLACK='X',WHITE='O'};

inline Color Rival(Color myColor){
	return (myColor==BLACK)?WHITE:BLACK;
}

class Position{
public:
	short x;
	short y;
	Position(int x,int y):x(x),y(y){}
	Position operator +(const Position& that){
		return Position(x+that.x,y+that.y);
	}
	Position& operator +=(const Position& that){
		x+=that.x;
		y+=that.y;
		return (*this);
	}
};

class Move{
public:
	Position pos;
	PositionList validOffsets;
	Move():pos(0,0){}
	Move(int x,int y):pos(x,y){}
	void addOffset(Position offset){validOffsets.push_back(offset);}
	PositionList& getOffsets(){return validOffsets;}
};

class Board{
	static MoveList corners;
	static MoveList XSquares;
	static Hashkey* pieceHashKey;
public:
	Board(int sideLength=8);
	Board(const Board& that);
	~Board();
	void initTranpositionTable();
	void initSpecialLocation();
	void print() const;
	void countPiece(int& blackPiece,int& whitePiece) const;
	bool isFull() const;
	int utility(Color caller) const;
	int eval(Color caller) const;
	int positionCount(Color my,MoveList& positions) const;
	void getValidMove(Color my,MoveList& validMove) const;
	Color piece(Position p) const {return board[rc(p.x,p.y)];}
	Color piece(int row,int col) const {return board[row*sideLength+col];}
	bool hasClosure(Color my,Position pos,const Position& offset)const;
	bool isValidPosition(const Position& p)const;
	void doMove(Color my,Move& move);
	Board* result(Color my,Move& move) const;
	void setPiece(const Position& pos,Color c);
	inline Color Rival(Color myColor)const{return (myColor==BLACK)?WHITE:BLACK;}


	//note: adding any data member should check if copy constructor has copy that member
public:
	Hashkey boardHashkey;
private:
	int sideLength;
	int boardSize;
	Color* board;

};
#endif
