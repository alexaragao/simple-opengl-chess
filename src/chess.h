#ifndef CHESS_H
#define CHESS_H

enum EType {
  Bishop,
  King,
  Knight,
  Pawn,
  Queen,
  Rook
};

struct ChessPiece {
  bool isWhite;
  EType type;
  bool hasMoved;
};

struct BoardSquare {
  bool hasPiece;
  ChessPiece piece;
  bool forceColor;
  int R;
  int G;
  int B;
  // Animation
  float dX;
  float dY;
};

enum EBoardSquareState {
  NONE,
  ACTIVE,
  AVAILABLE,
  ENEMY_PIECE
};

void setBoardSquareState(BoardSquare *boardSquare, EBoardSquareState state) {
  switch (state) {
  case ACTIVE:
    boardSquare->forceColor = true;
    boardSquare->R = 0x4A;
    boardSquare->G = 0x4C;
    boardSquare->B = 0xE8;
    break;
  case AVAILABLE:
    boardSquare->forceColor = true;
    boardSquare->R = 0x01;
    boardSquare->G = 0xBA;
    boardSquare->B = 0x01;
    break;
  case ENEMY_PIECE:
    boardSquare->forceColor = true;
    boardSquare->R = 0xFF;
    boardSquare->G = 0x00;
    boardSquare->B = 0x40;
    break;
  default:
    boardSquare->forceColor = false;
    break;
  }
}

EType getChessPieceTypeByIndex(int index) {
  switch (index) {
  case 0:
  case 7:
    return Rook;
  case 1:
  case 6:
    return Knight;
  case 2:
  case 5:
    return Bishop;
  case 3:
    return King;
  case 4:
    return Queen;
  default:
    return Pawn;
  }
}

const char *getPieceImagePathByType(EType type, bool isWhite) {
  switch (type) {
  case Rook:
    return isWhite ? "images/w_rook.png" : "images/b_rook.png";
  case Queen:
    return isWhite ? "images/w_queen.png" : "images/b_queen.png";
  case King:
    return isWhite ? "images/w_king.png" : "images/b_king.png";
  case Knight:
    return isWhite ? "images/w_knight.png" : "images/b_knight.png";
  case Bishop:
    return isWhite ? "images/w_bishop.png" : "images/b_bishop.png";
  default:
    return isWhite ? "images/w_pawn.png" : "images/b_pawn.png";
  }
}

#endif