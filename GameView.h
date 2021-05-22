#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include "Flashcard.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct Text {
  char *string;
  TTF_Font *font;
  SDL_Color color;
  SDL_Rect rect;
} Text;

void run(Deck *deck, char *outputDatabase);

#endif /* GAMEVIEW_H */
