#include <iostream>
#include <string>
#include "board.h"

int main(){
  Board board;
  std::string move;
  while(board.getturn()){ //0 if game over
    board.show();
    std::cout << "move (eg e2e4): ";
    std::cin >> move;
    if(move.size()>3)
      board.move(move[0]-'a', move[1]-'1', move[2]-'a', move[3]-'1');
  }
  board.show();
  return 0;
}
