#include "Config.h"
#include "Gui.h"
#include "Flashcard.h"
#include "Utilities.h"

void updatePosition(slPosition *positions, slWindow windowPara);
void drawFlashcard(FlashcardGame *game, slWindow windowPara, slPosition positions, TTF_Font *font, SDL_Renderer *renderer);
SDL_Texture *createUtf8StringTexture(char *string, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer);
void windowResizeCallback(SDL_Window *window, slWindow *windowPara, slPosition *positions);

void guiRun(FlashcardGame *game) {
  /* Initializing SDL */
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Unable to initialize SDL: %s\n", SDL_GetError());
    return;
  }
  /* Initializing SDL_ttf */
  if (TTF_Init() == -1) {
    puts("Unable to initialize SDL_ttf.");
    SDL_Quit();
    return;
  }

  SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    puts("guiRun: Cannot create window.");
    return;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    puts("guiRun: Cannot create renderer.");
    return;
  }

  TTF_Font *font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
  if (!font) {
    puts("guiRun: Cannot open font.");
    return;
  }

  slWindow windowPara = {.width = WINDOW_WIDTH,
                         .height = WINDOW_HEIGHT,
                         .padding = WINDOW_PADDING,
                         .fontsize = FONT_SIZE};

  slPosition *positions = malloc(sizeof (slPosition));
  updatePosition(positions, windowPara);
  newGame(game);

  SDL_Event e;
  int exit = 0;
  while (!exit) {
    if (SDL_WaitEvent(&e)) {
      switch (e.type) {
      case SDL_QUIT:
        exit = 1;
        break;
      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
          windowResizeCallback(window, &windowPara, positions);
          drawFlashcard(game, windowPara, *positions, font, renderer);
        }
        break;
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          writeScore(game);
          writeLog(game);
          exit = 1;
          break;
        case SDLK_SPACE:
          toggleShowAnswer(game);
          break;
        case SDLK_d:
          exportToDatabase(game);
          writeScore(game);
          break;
        case SDLK_j:
          forget(game);
          nextCard(game);
          break;
        case SDLK_k:
          remember(game);
          nextCard(game);
          break;
        case SDLK_n:
          nextCard(game);
          break;
        case SDLK_p:
          previousCard(game);
          break;
        case SDLK_o:
          newGame(game);
          break;
        case SDLK_s:
          shuffle(game);
          break;
        case SDLK_t:
          addOneMemoryCount(game);
          break;
        }
        drawFlashcard(game, windowPara, *positions, font, renderer);
        break;
      }
    }
  }

  destroyGame(game);
  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

SDL_Rect createRect(int x, int y, int w, int h) {
  SDL_Rect result = {x, y, w, h};
  return result;
}

void updatePosition(slPosition *positions, slWindow windowPara) {
  int padding = windowPara.padding;
  int fontsize = windowPara.fontsize;
  int height = windowPara.height;
  int width = windowPara.width;

  positions->questionRect = createRect(padding, padding + fontsize, 0, 0);
  positions->answerRect = createRect(padding, height / 2, 0, 0);
  positions->memoryRect = createRect(padding, padding, 0, 0);
  positions->counterRect = createRect(width - padding - fontsize * 4, padding, 0, 0);
  positions->goodScoreRect = createRect(width / 3 * 2, padding, 0, 0);
  positions->badScoreRect = createRect(width / 3, padding, 0, 0);
}

SDL_Texture *createUtf8StringTexture(char *string, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer) {
  SDL_Surface *surface = TTF_RenderUTF8_Blended(font, string, color);
  if (!surface) {
    printf("createUtf8StringTexture: Cannot create surface.\n%s\n", string);
    return NULL;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    printf("createUtf8StringTexture: Cannot create texture.\n");
    return NULL;
  }
  SDL_FreeSurface(surface);

  return texture;
}

void drawSmallText(SDL_Renderer *renderer, char *string, TTF_Font *font, SDL_Color color, SDL_Rect rect) {
  int w, h;
  SDL_Texture *texture = createUtf8StringTexture(string, font, color, renderer);
  SDL_QueryTexture(texture, NULL, NULL, &w, &h);
  rect.w = w / 2;
  rect.h = h / 2;
  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_DestroyTexture(texture);
}

SDL_Texture *createUtf8StringTextureWrapped(char *string, TTF_Font *font, SDL_Color color, SDL_Renderer *renderer, int renderWidth) {
  SDL_Surface *surface = TTF_RenderUTF8_Blended_Wrapped(font, string, color, renderWidth);
  if (!surface) {
    printf("createUtf8StringTextureWrapped: Cannot create surface.\n%s\n", string);
    return NULL;
  }

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!texture) {
    printf("createUtf8StringTextureWrapped: Cannot create texture.\n");
    return NULL;
  }
  SDL_FreeSurface(surface);

  return texture;
}

char *breakString(char *string, TTF_Font *font, SDL_Renderer *renderer, int width) {
  /* Remember to free it after using. */
  int spaceWidth, fullWidth;
  SDL_Color white = {255, 255, 255, 255};
  SDL_Texture *texture;
  /* texture= createUtf8StringTexture(" ", font, white, renderer); */
  /* SDL_QueryTexture(texture, NULL, NULL, &spaceWidth, NULL); */
  /* SDL_DestroyTexture(texture); */
  texture = createUtf8StringTexture("一", font, white, renderer);
  SDL_QueryTexture(texture, NULL, NULL, &fullWidth, NULL);
  SDL_DestroyTexture(texture);
  spaceWidth = fullWidth / 2;

  /* Have to minus one char. */
  width -= fullWidth;
  int stringWidth = 0;
  char *returnString = malloc(STRING_SIZE);
  int returnStringPos = 0;
  while (*string) {
    if ((*string & 0x80) == 0x00) {
      /* Check if the first bit is 1. */
      /* Which means that it is a single unicode char. */
      stringWidth += spaceWidth;
      returnString[returnStringPos++] = *string;
      string++;
    } else if ((*string & 0xc0) == 0xc0) {
      /* Check if the first two bit is 11. */
      /* Which means it is the beginning of multibyte unicode. */
      stringWidth += fullWidth;
      returnString[returnStringPos++] = *string;
      string++;

      /* Copy until a new start of unicode character. */
      /* The first bit is 1, second bit is 0. */
      while (*string && (*string & 0xc0 | 0x80) == 0x80) {
        returnString[returnStringPos++] = *string;
        string++;
      }
    }
    if (stringWidth > width) {
      returnString[returnStringPos++] = '\n';
      stringWidth = 0;
    }
  }

  returnString[returnStringPos] = '\0'; /* Append a null char. */

  return returnString;
}

void drawText(SDL_Renderer *renderer, char *string, TTF_Font *font, SDL_Color color, SDL_Rect rect, int width) {
  int w, h;
  char *brokenString = breakString(string, font, renderer, width);

  /* 100 should be very enough. */
  char **stringArr = malloc(sizeof(char*) * 100);
  int stringArrPos = 0;
  char tempString[STRING_SIZE];
  int tempStringPos = 0;
  char *strPtr = brokenString;
  while (*strPtr) {
    if (*strPtr != '\n') {
      tempString[tempStringPos] = *strPtr;
      strPtr++;
      tempStringPos++;
    } else {
      tempString[tempStringPos] = '\0';
      char *s = malloc(STRING_SIZE);
      strcpy(s, tempString);
      stringArr[stringArrPos++] = s;
      tempString[0] = 0;
      tempStringPos = 0;
      strPtr++;
    }
  }

  if (tempString[0] != '\0') {
    tempString[tempStringPos] = '\0';
    char *s = malloc(STRING_SIZE);
    strcpy(s, tempString);
    stringArr[stringArrPos++] = s;
  }

  SDL_Texture *texture;
  for (int i = 0; i < stringArrPos; i++) {
    texture = createUtf8StringTexture(stringArr[i], font, color, renderer);
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    rect.w = w;
    rect.h = h;
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    rect.y += h + 2;
    SDL_DestroyTexture(texture);
    free(stringArr[i]);
  }

  free(stringArr);
  free(brokenString);
}

void drawFlashcard(FlashcardGame *game, slWindow windowPara, slPosition positions, TTF_Font *font, SDL_Renderer *renderer) {
  SDL_RenderClear(renderer);

  /* Draw memory count. */
  char *memoryString = malloc(COUNT_SIZE);
  sprintf(memoryString, "%d", currentMemoryCount(game));
  SDL_Color yellow = {255, 255, 0, 255};
  drawSmallText(renderer, memoryString, font, yellow, positions.memoryRect);


  /* Draw scores. */
  char *badScoreString = malloc(COUNT_SIZE);
  sprintf(badScoreString, "%i", game->badScore);
  SDL_Color red = {255, 0, 0, 255};
  drawSmallText(renderer, badScoreString, font, red, positions.badScoreRect);

  char *goodScoreString = malloc(COUNT_SIZE);
  sprintf(goodScoreString, "%i", game->goodScore);
  SDL_Color green = {0, 255, 0, 255};
  drawSmallText(renderer, goodScoreString, font, green, positions.goodScoreRect);


  /* Draw counter. */
  char *counterString = malloc(COUNT_SIZE * 3);
  sprintf(counterString, "%d / %d", game->pos + 1, game->gameSize);
  SDL_Color white = {255, 255, 255, 255};
  drawSmallText(renderer, counterString, font, white, positions.counterRect);


  /* Draw question. */
  drawText(renderer, currentQuestion(game), font, white, positions.questionRect, windowPara.width - windowPara.padding * 2);

  /* Draw answer. */
  if (game->showAnswer) {
    drawText(renderer, currentAnswer(game), font, white, positions.answerRect, windowPara.width - windowPara.padding * 2);
  }

  SDL_RenderPresent(renderer);
}

void windowResizeCallback(SDL_Window *window, slWindow *windowPara, slPosition *positions) {
  positions->counterRect.x -= windowPara->width;
  SDL_GetWindowSize(window, &windowPara->width, &windowPara->height);
  positions->answerRect.y = windowPara->height / 2;
  positions->counterRect.x += windowPara->width;
  positions->goodScoreRect.x = windowPara->width / 3 * 2;
  positions->badScoreRect.x = windowPara->width / 3;
}
