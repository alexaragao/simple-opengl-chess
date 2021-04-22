#ifndef MODAL_H
#define MODAL_H

#include <GL/glut.h>
#include <iostream>

#include "chess.h"
#include "upng/upng.h"

namespace ModalWindow {
int window;

void render(void);
void keyboard(unsigned char c, int x, int y);

bool _isWhiteWinner;

int show(bool isWhiteWinner) {
  _isWhiteWinner = isWhiteWinner;

  glutInitDisplayMode(GLUT_RGB);
  glutInitWindowPosition(200, 200);
  glutInitWindowSize(320, 200);
  window = glutCreateWindow("Fim de jogo!");

  glClearColor(0.156, 0.164, 0.207, 1.0);
  glOrtho(0, 320, 200, 0, -1, 1);

  glutDisplayFunc(render);
  glutKeyboardFunc(keyboard);

  glutMainLoop();

  return 0;
}

void drawMessage(std::string message, int x, int y) {
  glRasterPos2i(x, y);
  for (int c = 0; c < message.length(); c++) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, message.at(c));
  }
}

void render(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  // Message
  glColor3ub(0xFF, 0xFF, 0xFF);

  drawMessage(_isWhiteWinner ? "Brancos" : "Pretas", _isWhiteWinner ? 125 : 131, 138);
  drawMessage("Vencem!", 122, 168);

  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Image
  upng_t *upng;
  GLuint texture;

  upng = upng_new_from_file(getPieceImagePathByType(King, _isWhiteWinner));
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
  glVertex2i(160 - 40, 20);

  glTexCoord2f(1.0, 0.0);
  glVertex2i(160 + 40, 20);

  glTexCoord2f(1.0, 1.0);
  glVertex2i(160 + 40, 20 + 80);

  glTexCoord2f(0.0, 1.0);
  glVertex2i(160 - 40, 20 + 80);

  glEnd();

  glDisable(GL_TEXTURE_2D);

  glFlush();
}

void keyboard(unsigned char c, int x, int y) {
  switch (c) {
  case 0x1B: // ASCII of ESC
    glutDestroyWindow(window);
    break;
  default:
    break;
  }
}
} // namespace ModalWindow

#endif