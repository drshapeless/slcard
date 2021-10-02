#ifndef GUI_H
#define GUI_H

#include "Flashcard.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
  int width;
  int height;
  int padding;
  int fontsize;
} slWindow;

typedef struct {
  SDL_Rect questionRect;
  SDL_Rect answerRect;
  SDL_Rect memoryRect;
  SDL_Rect counterRect;
  SDL_Rect goodScoreRect;
  SDL_Rect badScoreRect;
} slPosition;

void guiRun(FlashcardGame *game);

#endif /* GUI_H */
