#include <iostream>
#include <vector>
#include <algorithm>
#include "board.h"

Board::Board(){
  for(int i=0;i<8;i++){
    if(grid[i])grid[56+i] = grid[i]+32; //copy from white initializer list
    grid[48+i] = 'p'; //black pawn setup
  }
  castling = 15; //castling bitmask all on
  enpassant = -1; //records enpassant for any pawn that moved 2 squares
  turn = 1; //1 white -1 black
  movecount = 1;
  winner = 0;
  refreshlegal(); //generate legal
}

void Board::show()const{
  if(turn) //show turn if not end
    std::cout << "move " << movecount << " - "
      << (turn+1?"white":"black") << " to move";
  else //show winner if end
    std::cout << (winner?winner+1?"white won":"black won":"drawn")
      << " in " << movecount << " moves";
  if(turn==1){ //print board unflipped
    for(int i=7;i>=0;i--){
      std::cout << '\n' << (1+i) << ' ';
      for(int j=0;j<8;j++){
        std::cout << (grid[i*8+j] ? grid[i*8+j] : i%2==j%2 ? '.' : '+') << ' ';
      }
    }
    std::cout << "\n  a b c d e f g h\n";
  }
  else{ //print flipped
    for(int i=0;i<8;i++){
      std::cout << '\n' << (1+i) << ' ';
      for(int j=7;j>=0;j--) {
        std::cout << (grid[i*8+j] ? grid[i*8+j] : i%2==j%2 ? '.' : '+') << ' ';
      }
    }
    std::cout << "\n  h g f e d c b a\n";
  }
}

//is my own piece not blocking this square (can move/capture to)
bool Board::k(int f)const{return !grid[f]||(turn==1)==(grid[f]>'a');}

void Board::diagonals(int i){ //generate legal diagonals from square index i
  for(int j=1;j<8;j++){
    if(((i&7)-j>=0) && ((i>>3)-j>=0) && k(i-j*9))
      legal.push_back((i<<6)|(i-j*9));
    else break;
  }
  for(int j=1;j<8;j++){
    if(((i&7)-j>=0) && ((i>>3)+j<8) && k(i+j*7))
      legal.push_back((i<<6)|(i+j*7));
    else break;
  }
  for(int j=1;j<8;j++){
    if(((i&7)+j<8) && ((i>>3)-j>=0) && k(i-j*7))
      legal.push_back((i<<6)|(i-j*7));
    else break;
  }
  for(int j=1;j<8;j++){
    if(((i&7)+j<8) && ((i>>3)+j<8) && k(i+j*9))
      legal.push_back((i<<6)|(i+j*9));
    else break;
  }
}

void Board::lines(int i){ //for lines
  for(int j=1;j<8;j++){
    if((i&7)-j>=0 && k(i-j))
      legal.push_back((i<<6)|(i-j));
    else break;
  }
  for(int j=1;j<8;j++){
    if((i&7)+j<8 && k(i+j))
      legal.push_back((i<<6)|(i+j));
    else break;
  }
  for(int j=1;j<8;j++){
    if((i>>3)-j>=0 && k(i-j*8))
      legal.push_back((i<<6)|(i-8*j));
    else break;
  }
  for(int j=1;j<8;j++){
    if((i>>3)+j<8 && k(i+j*8))
      legal.push_back((i<<6)|(i+j*8));
    else break;
  }
}

bool Board::check(int kpos, int b)const{ //is position kpos being attacked by side b(0 if white 32 if black)
  if (((kpos>>3)+turn==(kpos-turn*7)/8 && grid[kpos-turn*7]-b=='P')
      || ((kpos>>3)+turn==(kpos-turn*9)/8 && grid[kpos-turn*9]-b=='P')
      || ((kpos>>3)-1>=0 && (
          ((kpos&7)+2<8 && grid[kpos-6]-b=='N') 
          || ((kpos&7)-2>=0 && grid[kpos-10]-b=='N')
          || ((kpos>>3)-2 >= 0 && (
              ((kpos&7)+1<8 && grid[kpos-15]-b=='N')
              || ((kpos&7)-1>=0 && grid[kpos-17]-b=='N')
              ))))
      || ((kpos>>3)+1<8 && (
          ((kpos&7)+2<8 && grid[kpos+6]-b=='N') 
          || ((kpos&7)-2>=0 && grid[kpos+10]-b=='N')
          || ((kpos>>3)-2 >= 0 && (
              ((kpos&7)+1<8 && grid[kpos+15]-b=='N')
              || ((kpos&7)-1>=0 && grid[kpos+17]-b=='N')
              ))))
     ) return 1;
  for(int i=1;i<8;i++)
    if((kpos&7)+i<8)
      if((grid[kpos+i]-b=='R') || (grid[kpos+i]-b=='Q') || (i==1&&grid[kpos+1]-b=='K')) 
        return 1; else if(grid[kpos+i]) break;
  for(int i=1;i<8;i++)
    if((kpos&7)-i>=0)
      if((grid[kpos-i]-b=='R') || (grid[kpos-i]-b=='Q') || (i==1&&grid[kpos-1]-b=='K'))
        return 1; else if(grid[kpos-i]) break;
  for(int i=1;i<8;i++)
    if((kpos>>3)+i<8)
      if((grid[kpos+i*8]-b=='R') || (grid[kpos+i*8]-b=='Q') || (i==1&&(grid[kpos+8]-b=='K')))
        return 1; else if(grid[kpos+i*8]) break;
  for(int i=1;i<8;i++)
    if((kpos>>3)-i>=0)
      if((grid[kpos-i*8]-b=='R') || (grid[kpos-i*8]-b=='Q') || (i==1&&(grid[kpos-8]-b=='K')))
        return 1; else if(grid[kpos-i*8])break;
  
  for(int i=1;i<8;i++)
    if((kpos&7)+i<8 && (kpos>>3)-i>=0)
      if((grid[kpos-i*7]-b=='B') || (grid[kpos-i*7]-b=='Q') || (i==1&&(grid[kpos-7]-b=='K')))
        return 1; else if(grid[kpos-i*7])break;
  for(int i=1;i<8;i++)
    if((kpos&7)+i<8 && (kpos>>3)+i<8)
      if((grid[kpos+i*9]-b=='B') || (grid[kpos+i*9]-b=='Q') || (i==1&&(grid[kpos+9]-b=='K')))
        return 1; else if(grid[kpos+i*9])break;
  for(int i=1;i<8;i++)
    if((kpos&7)-i>=0 && (kpos>>3)-i>=0)
      if((grid[kpos-i*9]-b=='B') || (grid[kpos-i*9]-b=='Q') || (i==1&&(grid[kpos-9]-b=='K')))
        return 1; else if(grid[kpos-i*9])break;
  for(int i=1;i<8;i++)
    if((kpos&7)-i>=0 && (kpos>>3)+i<8)
      if((grid[kpos+i*7]-b=='B') || (grid[kpos+i*7]-b=='Q') || (i==1&&(grid[kpos+7]-b=='K')))
        return 1; else if(grid[kpos+i*7])break;
  return 0;
}

//is my king attacked by oppoment side
bool Board::check()const{return check(std::find(grid,grid+64,turn-1?'k':'K')-grid,turn-1?32:0);}

void Board::refreshlegal(){
  unsigned short a; //just stores iterator i shifted 6 bits for convenience
  char b = (turn+1)?0:32; //for char manipulation upper/lowercase
  legal.clear();
  
  for(int i=0;i<64;i++){
    a = i<<6;
    switch(grid[i]-b){ //-b so we can just use one case per piece for any turn
      case 'P': {
        if (!grid[i+8*turn]){ //one square
          legal.push_back(a|(i+8*turn));
          //two squares
          if((i>>3)==((turn+1)?1:6)&&!grid[i+16*turn]) legal.push_back(a|(i+16*turn));
        }
        //diagonal capture
        if (((i>>3)+turn==(i+9*turn)/8)&&((k(i+9*turn)&&grid[i+9*turn])||enpassant==(i+9*turn)))
          legal.push_back(a|(i+9*turn));
        if (((i>>3)+turn==(i+7*turn)/8)&&((k(i+7*turn)&&grid[i+7*turn])||enpassant==(i+7*turn)))
          legal.push_back(a|(i+7*turn));
        break;
      }
      case 'N': {
        //all L shapes
        if ((i>>3)-1 >= 0) {
          if ((i&7)+2<8 && k(i-6)) legal.push_back(a|(i-6));
          if ((i&7)-2>=0 && k(i-10)) legal.push_back(a|(i-10));
          if ((i>>3)-2 >= 0) {
            if ((i&7)+1<8 && k(i-15)) legal.push_back(a|(i-15));
            if ((i&7)-1>=0 && k(i-17)) legal.push_back(a|(i-17));
          }
        }
        if ((i>>3)+1 < 8) {
          if ((i&7)-2<8 && k(i+6)) legal.push_back(a|(i+6));
          if ((i&7)+2>=0 && k(i+10)) legal.push_back(a|(i+10));
          if ((i>>3)+2 < 8) {
            if ((i&7)+1<8 && k(i+17)) legal.push_back(a|(i+17));
            if ((i&7)-1>=0 && k(i+15)) legal.push_back(a|(i+15));
          }
        }
        break;
      }
      case 'B': {
        diagonals(i);
        break;
      }
      case 'R': {
        lines(i);
        break;
      }
      case 'Q': {
        //seperate functions instead just because queen exist
        diagonals(i);
        lines(i);
        break;
      }
      case 'K': {
        //any square
        if((i&7)-1>=0) {
          if (k(i-1)) legal.push_back(a|(i-1));
          if ((i>>3)-1>=0 && k(i-9)) legal.push_back(a|(i-9));
          if ((i>>3)+1<8 && k(i+7)) legal.push_back(a|(i+7));
        }
        if((i&7)+1<8) {
          if (k(i+1)) legal.push_back(a|(i+1));
          if ((i>>3)+1<8 && k(i+9)) legal.push_back(a|(i+9));
          if ((i>>3)-1>=0 && k(i-7)) legal.push_back(a|(i-7));
        }
        if((i>>3)-1>=0 && k(i-8)) legal.push_back(a|(i-8));
        if((i>>3)+1<8 && k(i+8)) legal.push_back(a|(i+8));
        break;
      }
      default: break;
    };
  }
  //swap turn temporarily to see if move results in capturable king
  b=32-b;
  char tmp, f, t;
  int kpos= std::find(grid,grid+64,'k'-b)-grid; //find the king
  turn = -turn;
  for(int e=0;e<legal.size();e++){ //loop all legal
    t = legal[e]&63; f = (legal[e]>>6); //from and to squares
    tmp = grid[t]; //temp store to square in case capture
    if(f==kpos)kpos=t; //update kingpos if moving king
    if(t==enpassant && grid[f]-b=='p')grid[t+8*turn]=0; //enpassant special case
    grid[t] = grid[f]; //try the move
    grid[f] = 0; //erase if illegal
    if (check(kpos,b)) legal.erase(legal.begin()+e--);
    grid[f] = grid[t]; //undo the move
    grid[t] = tmp;
    if(t==kpos)kpos=f; //put the kingpos back
    if(t==enpassant && grid[f]-b=='p')grid[t+8*turn]='p'-b; //put the pawn back if enpassant
  }
  if(b){ //check if can castle, castling square empty and not attacked
    if((castling&1)&&(!(grid[1]|grid[2]|grid[3]|check(1,b)|check(2,b)|check(3,b)|check(4,b))))
      legal.push_back(256); //white queenside
    if((castling&2)&&(!(grid[5]|grid[6]|check(4,b)|check(5,b)|check(6,b))))
      legal.push_back(263); //white kingside
  }
  else{
    if((castling&4)&&(!(grid[61]|grid[62]|check(60,b)|check(61,b)|check(62,b))))
      legal.push_back(3903); //black kingside
    if((castling&8)&&(!(grid[57]|grid[58]|grid[59]|check(57,b)|check(58,b)|check(59,b)|check(60,b))))
      legal.push_back(3896); //black queenside
  }
  turn=-turn; //flip the turn back
}

bool Board::move(char x1, char y1, char x2, char y2){
  if(x1<0||y1<0||x1<0||y2<0||x1>7||y1>7||x2>7||y2>7) return 0; //validate input
  char f=y1*8+x1, t=y2*8+x2;
  if(std::find(legal.begin(),legal.end(),f<<6|t)==legal.end()) return 0; //illegal move
  grid[t] = grid[f]; //move piece
  if(t==enpassant&&(grid[t]=='p'||grid[t]=='P')) grid[t-8*turn] = 0; //kill pawn if enpassant
  enpassant = ((grid[t]=='P'||grid[t]=='p')&&(turn*(y2-y1))==2)? //set enpassant if pawn move 2 squares
                t-8*turn:-1;
  grid[f] = 0; //finish move piece
  if(turn==1){ //finish castling if move is to castle
    if((castling&1)&&(turn==1)&&(f==4)&&(t==0)){
      grid[0]=0;
      grid[2]='K';
      grid[3]='R';
      castling &= 0b1100;
    }
    else if((castling&2)&&(turn==1)&&(f==4)&&(t==7)){
      grid[7]=0;
      grid[5]='R';
      grid[6]='K';
      castling &= 0b1100;
    }
  }
  else{
    if((castling&4)&&(turn!=1)&&(f==60)&&(t==63)){
      grid[63]=0;
      grid[62]='k';
      grid[61]='r';
      castling &= 0b0011;
    }
    else if((castling&8)&&(turn!=1)&&(f==60)&&(t==56)){
      grid[56]=0;
      grid[58]='k';
      grid[59]='r';
      castling &= 0b0011;
    }
  }
  //update castling bitmask
  if(castling&4){
    if(grid[t]=='k') castling &= 0b0011;
    else if(grid[63]!='r') castling &= 0b1011;
  }
  else if(castling&8){
    if(grid[56]!='r') castling &= 0b0111;
  }
  if(castling&1) {
    if(grid[4]!='K') castling &= 0b1100;
    else if(grid[0]!='R') castling &= 0b1110;
  }
  else if(castling&2){
    if(grid[7]!='R') castling &= 0b1101;
  }
  //flip turn, add movecount, set legal moves
  turn = -turn;
  if(turn==1) movecount++;
  refreshlegal();
  //no more legal moves, win by checkmate/stalemate
  if(!legal.size()){winner=check()*-turn; turn=0;}
/*   for(auto i:legal){
    std::cout << char('a'+(i>>6)%8) << 1+((i>>6)/8)<< char('a'+(i&63)%8) << 1+((i&63)/8) << ' ';
  } */
  return 1;
}
