#ifndef GUI_H
#define GUI_H

#include "Flashcard.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

struct slWindow {
	int width;
	int height;
	int padding;
	int fontsize;
};

struct slPosition {
	SDL_Rect questionRect;
	SDL_Rect answerRect;
	SDL_Rect memoryRect;
	SDL_Rect counterRect;
	SDL_Rect goodScoreRect;
	SDL_Rect badScoreRect;
};

void guiRun(struct FlashcardGame *game);

#endif /* GUI_H */
