#ifndef BOARD_H
#define BOARD_H
#include <vector>

class Board{
  char grid[64]{"RNBQKBNRPPPPPPPP"}; //64 char array of grid
  char turn; //turn, 0(game over), 1 (white) or -1(black)
  char winner; //winner, 0(draw), 1(white win) or -1(black win)
  char castling; //bitmask of castlable sides
  char enpassant; //store potential enpassant square
  int movecount;
  std::vector<unsigned short> legal; //form of (index_from<<6)|index_to
  void diagonals(int); //generate diagonal legal moves
  void lines(int); //generate horizontal/vertical legal moves
  void refreshlegal(); //refresh all legal moves
  bool k(int)const; //check if passed square index not blocked by own piece
  bool check(int kpos, int b)const; //check if kpos is attacked by side b(0 if white and 32 if black)
  public:
    Board(); //resets board
    const char*getgrid()const{return grid;}
    char getturn()const{return turn;}
    char getwinner()const{return winner;}
    char getmovecount()const{return movecount;}
    void show()const; //prints board to std::cout
    bool check()const; //check if current player in cehck
    bool move(char x1, char y1, char x2, char y2); //moves from x1,y1 to x2,y2, returns if move is legal
};

#endif
