#include <GL/glut.h>
#include <iostream>
#include <string.h>

#include "chess.h"
#include "upng/upng.h"

#include "modal.h"

// DEFAULT GAME SETTINGS
bool DEFAULT_WHITE_START = true;
float DEFAULT_ANIMATION_DURATION = 200;

// GAME VARIABLES
BoardSquare cels[64];

int selectedBoardPosition = -1;
int selectedCharPosition = -1;

bool isWhiteTurn = DEFAULT_WHITE_START;
bool isGameInteractive = true;

float originX = 0;
float originY = 0;

float scaleX = 1;
float scaleY = 1;

float boardSquareSize = 80;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 640;

// Animation
int aTargetPosition = -1;
float adX = 0;
float adY = 0;

// ============ GLUT FUNCTIONS ============ //
void render(void);
void keyboard(unsigned char c, int x, int y);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void timer(int value);

// ============ CHESS FUNCTIONS ============ //
BoardSquare *getBoardSquareByPosition(int x, int y) {
  return &cels[x + 8 * y];
}

bool areCoordsValid(int x, int y) {
  if (x < 0 || x >= 8 || y < 0 || y >= 8) {
    return false;
  }
  return true;
}

void updateStateOfSquaresInDirection(int selectIndex, int dX, int dY, int limit = 1) {
  BoardSquare *boardSquare = &cels[selectIndex];

  int x = selectIndex % 8;
  int y = selectIndex / 8;

  for (int i = 1; i <= limit; i++) {
    if (!areCoordsValid(x + i * dX, y + i * dY)) {
      break;
    }

    BoardSquare *iBoardSquare = getBoardSquareByPosition(x + i * dX, y + i * dY);

    if (!iBoardSquare->hasPiece) {
      setBoardSquareState(iBoardSquare, AVAILABLE);
    } else {
      if (iBoardSquare->piece.isWhite != boardSquare->piece.isWhite) {
        setBoardSquareState(iBoardSquare, ENEMY_PIECE);
      }
      break;
    }
  }
}

void resetBoardColors() {
  for (int i = 0; i < 64; i++) {
    cels[i].forceColor = false;
  }
}

void startMoveAnimation(BoardSquare *targetSquare) {
  resetBoardColors();

  int poX = selectedBoardPosition % 8;
  int poY = selectedBoardPosition / 8;

  int pX = aTargetPosition % 8;
  int pY = aTargetPosition / 8;

  adX = pX - poX;
  adY = pY - poY;

  isGameInteractive = false;
  glutTimerFunc(10, timer, 0);
}

void onChessPieceSelect(int pX, int pY) {
  BoardSquare *boardSquare = getBoardSquareByPosition(pX, pY);
  ChessPiece *piece = &boardSquare->piece;

  int currentSelectedBoardPosition = pX + 8 * pY;

  if (selectedBoardPosition > -1) {
    if (selectedBoardPosition == currentSelectedBoardPosition) {
      resetBoardColors();

      selectedBoardPosition = -1;
      glutPostRedisplay();
      return;
    }

    // If has clicked in a valid move square, move it.
    if (selectedBoardPosition != currentSelectedBoardPosition && boardSquare->forceColor) {
      aTargetPosition = currentSelectedBoardPosition;
      // On no animation version, here is where the move occours.
      startMoveAnimation(boardSquare);
      return;
    }
  }

  if (!boardSquare->hasPiece) {
    selectedBoardPosition = -1;
    resetBoardColors();

    glutPostRedisplay();
    return;
  }

  if (piece->isWhite ^ isWhiteTurn) {
    return;
  }

  resetBoardColors();

  std::cout << "Selected ";
  std::cout << (piece->isWhite ? "white " : "black ");
  std::cout << piece->type;
  std::cout << " at (";
  std::cout << pX;
  std::cout << ',';
  std::cout << pY;
  std::cout << ')' << std::endl;

  selectedBoardPosition = currentSelectedBoardPosition;

  setBoardSquareState(getBoardSquareByPosition(pX, pY), ACTIVE);

  switch (piece->type) {
  case Pawn:
    if (areCoordsValid(1 + pX, (piece->isWhite ? 1 : -1) + pY) && getBoardSquareByPosition(1 + pX, (piece->isWhite ? 1 : -1) + pY)->hasPiece && getBoardSquareByPosition(1 + pX, (piece->isWhite ? 1 : -1) + pY)->piece.isWhite != piece->isWhite) {
      setBoardSquareState(getBoardSquareByPosition(1 + pX, (piece->isWhite ? 1 : -1) + pY), ENEMY_PIECE);
    }
    if (areCoordsValid(pX - 1, (piece->isWhite ? 1 : -1) + pY) && getBoardSquareByPosition(pX - 1, (piece->isWhite ? 1 : -1) + pY)->hasPiece && getBoardSquareByPosition(pX - 1, (piece->isWhite ? 1 : -1) + pY)->piece.isWhite != piece->isWhite) {
      setBoardSquareState(getBoardSquareByPosition(pX - 1, (piece->isWhite ? 1 : -1) + pY), ENEMY_PIECE);
    }

    for (int i = 1; i <= (piece->hasMoved ? 1 : 2); i++) {
      if (!areCoordsValid(pX, pY + i * (piece->isWhite ? 1 : -1))) {
        break;
      }

      BoardSquare *iBoardSquare = getBoardSquareByPosition(pX, pY + i * (piece->isWhite ? 1 : -1));

      if (!iBoardSquare->hasPiece) {
        setBoardSquareState(iBoardSquare, AVAILABLE);
      } else {
        break;
      }
    }
    // updateStateOfSquaresInDirection(selectedBoardPosition, 0, piece->isWhite ? 1 : -1, piece->hasMoved ? 1 : 2);
    break;
  case Knight:
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, 2, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, -2, 1);

    updateStateOfSquaresInDirection(selectedBoardPosition, -1, 2, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, -2, 1);

    updateStateOfSquaresInDirection(selectedBoardPosition, 2, 1, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, 2, -1, 1);

    updateStateOfSquaresInDirection(selectedBoardPosition, -2, 1, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, -2, -1, 1);
    break;
  case Bishop:
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, 1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, -1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, 1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, -1, 8);
    break;
  case Rook:
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, 0, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, 0, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, 0, 1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, 0, -1, 8);
    break;
  case Queen:
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, 1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, -1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, 1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, -1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, 0, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, 0, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, 0, 1, 8);
    updateStateOfSquaresInDirection(selectedBoardPosition, 0, -1, 8);
    break;
  case King:
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, 1, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, -1, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, 1, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, -1, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, 1, 0, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, -1, 0, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, 0, 1, 1);
    updateStateOfSquaresInDirection(selectedBoardPosition, 0, -1, 1);
    break;
  }

  glutPostRedisplay();
}

void init() {
  selectedCharPosition = -1;
  selectedBoardPosition = -1;
  isWhiteTurn = DEFAULT_WHITE_START;
  isGameInteractive = true;

  ChessPiece chessPiece;
  for (int i = 0; i < 64; i++) {
    if (i < 16) {
      // White pieces
      chessPiece = {true, getChessPieceTypeByIndex(i)};
      cels[i] = {true, chessPiece, false};
    } else if (i >= 48) {
      // Black pieces
      chessPiece = {false, getChessPieceTypeByIndex(i < 56 ? i - 40 : i - 56)};
      cels[i] = {true, chessPiece, false};
    } else {
      cels[i] = {false};
    }
  }
}

int main(int argc, char **argv) {
  init();

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(640, 640);
  glutCreateWindow("Simples OpenGL Chess");

  glClearColor(0.152, 0.164, 0.207, 1.0);

  glutDisplayFunc(render);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);

  glutMainLoop();
  return 0;
}

void drawBoard() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      int isWhite = (i + j) % 2 == 0;

      BoardSquare boardSquare = cels[j * 8 + i];
      if (boardSquare.forceColor) {
        glColor3ub(boardSquare.R, boardSquare.G, boardSquare.B);
      } else {
        glColor3ub(isWhite ? 0xFF : 0x00, isWhite ? 0xFF : 0x00, isWhite ? 0xFF : 0x00);
        // glColor3ub(isWhite ? 0xF5 : 0x66, isWhite ? 0xE6 : 0x44, isWhite ? 0xBF : 0x3A);
      }

      glBegin(GL_QUADS);
      glVertex2i(originX + i * boardSquareSize, originY + j * boardSquareSize);
      glVertex2i(originX + (i + 1) * boardSquareSize, originY + j * boardSquareSize);
      glVertex2i(originX + (i + 1) * boardSquareSize, originY + (j + 1) * boardSquareSize);
      glVertex2i(originX + i * boardSquareSize, originY + (j + 1) * boardSquareSize);
      glEnd();
    }
  }
}

void drawPieces() {
  glEnable(GL_TEXTURE_2D);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      BoardSquare boardSquare = cels[j * 8 + i];

      if (boardSquare.hasPiece) {
        upng_t *upng;
        GLuint texture;

        upng = upng_new_from_file(getPieceImagePathByType(boardSquare.piece.type, boardSquare.piece.isWhite));
        upng_decode(upng);

        glColor3ub(0xFF, 0xFF, 0xFF);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, upng_get_width(upng), upng_get_height(upng), 0, GL_RGBA, GL_UNSIGNED_BYTE, upng_get_buffer(upng));

        glBindTexture(GL_TEXTURE_2D, texture);

        glBegin(GL_QUADS);

        glTexCoord2f(0.0, 0.0);
        glVertex2i(originX + (i + boardSquare.dX) * boardSquareSize + 12, originY + (j + boardSquare.dY) * boardSquareSize + 12);

        glTexCoord2f(1.0, 0.0);
        glVertex2i(originX + (i + 1 + boardSquare.dX) * boardSquareSize - 12, originY + (j + boardSquare.dY) * boardSquareSize + 12);

        glTexCoord2f(1.0, 1.0);
        glVertex2i(originX + (i + 1 + boardSquare.dX) * boardSquareSize - 12, originY + (j + 1 + boardSquare.dY) * boardSquareSize - 12);

        glTexCoord2f(0.0, 1.0);
        glVertex2i(originX + (i + boardSquare.dX) * boardSquareSize + 12, originY + (j + 1 + boardSquare.dY) * boardSquareSize - 12);

        glEnd();
      }
    }
  }

  glDisable(GL_TEXTURE_2D);
}

void render(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  drawBoard();
  drawPieces();

  for (int i = 0; i < 8; i++) {
    glColor3ub(i == selectedCharPosition ? 0xFF : 0xFF, i == selectedCharPosition ? 0x00 : 0xFF, i == selectedCharPosition ? 0x00 : 0xFF);
    glRasterPos2i(originX + boardSquareSize * (i + 0.5f) - 9, 27);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 0x41 + i);

    glColor3ub(i == selectedCharPosition ? 0xFF : 0xFF, i == selectedCharPosition ? 0x00 : 0xFF, i == selectedCharPosition ? 0x00 : 0xFF);
    glRasterPos2i(originX + boardSquareSize * (i + 0.5f) - 9, WINDOW_HEIGHT - 9);
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 0x41 + i);

    glColor3ub(0xFF, 0xFF, 0xFF);
    glRasterPos2i(18, originY + boardSquareSize * (i + 0.5f));
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 0x31 + i);

    glColor3ub(0xFF, 0xFF, 0xFF);
    glRasterPos2i(WINDOW_WIDTH - 18, originY + boardSquareSize * (i + 0.5f));
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, 0x31 + i);
  }

  glFlush();
}

void handleKeyPress(unsigned char c) {
  // if (selectedBoardPosition > -1) {
  //   selectedBoardPosition = -1;
  // }

  if (selectedCharPosition > -1) {
    if (c > 0x38) {
      return;
    }
    c -= 0x31;

    onChessPieceSelect(selectedCharPosition, c);

    selectedCharPosition = -1;
  } else {
    if (c <= 8) {
      return;
    }

    if (c > 0x60) {
      c -= 0x20;
    }

    selectedCharPosition = c - 0x41;
  }

  glutPostRedisplay();
}

void handleLeftButtonClick(int x, int y) {
  // Ignores if has clicked outside the boardtX
  if (y < originY || y > originY + 8 * boardSquareSize) {
    return;
  }

  int pX = (x - (int)originX) / boardSquareSize;
  int pY = (y - (int)originY) / boardSquareSize;

  selectedCharPosition = -1;
  onChessPieceSelect(pX, pY);
}

void keyboard(unsigned char c, int x, int y) {
  switch (c) {
  case 0x1B: // ASCII of ESC
    exit(0);
    break;
  case 0x52: // R
  case 0x72: // r
    // Restart
    if (aTargetPosition < 0) {
      init();
      glutPostRedisplay();
    }
    break;
  default:
    if (isGameInteractive) {
      if (0x40 < c && c <= 0x48 || 0x60 < c && c <= 0x68 || 0x30 < c && c <= 0x38) {
        handleKeyPress(c);
      }
    }
    break;
  }
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
  case GLUT_LEFT_BUTTON:
    if (state == GLUT_DOWN) {
      if (isGameInteractive) {
        handleLeftButtonClick(x, y);
      }
    }
    break;
  default:
    break;
  }
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glLoadIdentity();
  glOrtho(0, w, h, 0, -1, 1);

  WINDOW_WIDTH = w;
  WINDOW_HEIGHT = h;

  scaleX = w / 640.0f;
  scaleY = h / 640.0f;

  boardSquareSize = std::min(scaleX, scaleY) * 80 - 9;

  originX = (w - 8 * boardSquareSize) / 2.0f;
  originY = (h - 8 * boardSquareSize) / 2.0f;

  glutPostRedisplay();
}

void timer(int value) {
  BoardSquare *boardSquare = &cels[selectedBoardPosition];
  boardSquare->dX = (value / DEFAULT_ANIMATION_DURATION) * adX;
  boardSquare->dY = (value / DEFAULT_ANIMATION_DURATION) * adY;

  if (value >= DEFAULT_ANIMATION_DURATION) {
    isGameInteractive = true;

    BoardSquare *targetBoardSquare = &cels[aTargetPosition];

    boardSquare->hasPiece = false;

    bool isCheckmate = false;
    if (targetBoardSquare->hasPiece && targetBoardSquare->piece.type == King) {
      targetBoardSquare->piece = boardSquare->piece;

      isGameInteractive = false;
      isCheckmate = true;
    }

    targetBoardSquare->piece = boardSquare->piece;
    targetBoardSquare->piece.hasMoved = true;

    // Change player turn
    isWhiteTurn = !isWhiteTurn;

    targetBoardSquare->hasPiece = true;

    selectedBoardPosition = -1;

    // Reset anim variables
    aTargetPosition = -1;
    adX = 0;
    adY = 0;

    boardSquare->dX = 0;
    boardSquare->dY = 0;

    glutPostRedisplay();

    if (isCheckmate) {
      ModalWindow::show(targetBoardSquare->piece.isWhite);
    }
    return;
  }

  glutPostRedisplay();

  glutTimerFunc(10, timer, value + 10);
}