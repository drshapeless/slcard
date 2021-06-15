#include "Config.h"
#include "Flashcard.h"
#include "GameView.h"
#include "Utilities.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void updateString(Deck *deck, int *game, int pos,int itemCount, char **question, char **answer, char *memory, char *counter);
void windowResizeCallback(SDL_Window *window, int *width, int *height, SDL_Rect *answerRect, SDL_Rect *counterRect, SDL_Rect *goodScoreRect, SDL_Rect *badScoreRect);
void drawText(SDL_Renderer *renderer, int width, char *string, TTF_Font *font, SDL_Color color, SDL_Rect rect);
void drawSmallText(SDL_Renderer *renderer, char *string, TTF_Font *font, SDL_Color color, SDL_Rect rect);
void exportToScoreFile(char *scoreFileName, int badScore, int goodScore);
void writeToLogFile(char *outputDatabase, int badScore, int goodScore);

void run(Deck *deck, char *outputDatabase, char *scoreFileName) {
  SDL_Window *window = SDL_CreateWindow(WINDOW_TITLE, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    printf("run: Cannot create window.\n");
    return;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("run: Cannot create renderer.\n");
    return;
  }

  int width = WINDOW_WIDTH;
  int height = WINDOW_HEIGHT;
  int padding = WINDOW_PADDING;
  int fontsize = FONT_SIZE;

  TTF_Font *font = TTF_OpenFont(FONT_PATH, fontsize);
  if (!font) {
    printf("run: Cannot open font.\n");
    return;
  }
  SDL_Color red = {255, 0, 0, 255};
  SDL_Color white = {255, 255, 255, 255};
  SDL_Color green = {0, 255, 0, 255};
  SDL_Color yellow = {255, 255, 0, 255};

  int showAnswer = 0;
  int pos = 0;			/* Game position. */
  int itemCount = 0;
  int *gameItem = forgottenCards(deck->counts, deck->used, &itemCount);
  shuffleCards(gameItem, itemCount);
  int scoreMode = 0;
  int goodScore, badScore;

  SDL_Rect questionRect = {padding, padding + fontsize, 0, 0};
  SDL_Rect answerRect = {padding, height / 2, 0, 0};
  SDL_Rect memoryRect = {padding, padding, 0, 0};
  SDL_Rect counterRect = {width - padding - fontsize * 4, padding, 0, 0};
  SDL_Rect goodScoreRect = {width / 3 * 2, padding, 0, 0};
  SDL_Rect badScoreRect = {width / 3, padding, 0, 0};

  char *question = "test question.";
  char *answer = "test answer.";
  char *memory = malloc(COUNT_SIZE);
  char *counter = malloc(COUNT_SIZE * 3);
  char *bad = malloc(COUNT_SIZE);
  char *good = malloc(COUNT_SIZE);

  if (scoreFileName != NULL) {
    scoreMode = 1;
    FILE *scoreFile;
    scoreFile = fopen(scoreFileName, "r");
    char *temp = fgetline(scoreFile);
    strcpy(bad, temp);
    free(temp);
    temp = fgetline(scoreFile);
    strcpy(good, temp);
    free(temp);
    badScore = strtol(bad, NULL, 10);
    goodScore = strtol(good, NULL, 10);
    fclose(scoreFile);
  }

  updateString(deck, gameItem, pos, itemCount, &question, &answer, memory, counter);
  /* Draw the first question without answer when initialized */
  SDL_RenderClear(renderer);
  drawSmallText(renderer, memory, font, red, memoryRect);
  drawSmallText(renderer, counter, font, white, counterRect);
  if (scoreMode) {
    drawSmallText(renderer, bad, font, yellow, badScoreRect);
    drawSmallText(renderer, good, font, green, goodScoreRect);
  }
  drawText(renderer, width, question, font, white, questionRect);
  SDL_RenderPresent(renderer);

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
          windowResizeCallback(window, &width, &height, &answerRect, &counterRect, &goodScoreRect, &badScoreRect);
          /* Redraw. */
          SDL_RenderClear(renderer);
          drawSmallText(renderer, memory, font, red, memoryRect);
          drawSmallText(renderer, counter, font, white, counterRect);
          if (scoreMode) {
            drawSmallText(renderer, bad, font, yellow, badScoreRect);
            drawSmallText(renderer, good, font, green, goodScoreRect);
          }
          drawText(renderer, width, question, font, white, questionRect);
          if (showAnswer)
            drawText(renderer, width, answer, font, white, answerRect);
          SDL_RenderPresent(renderer);
        }
        break;
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          writeToLogFile(outputDatabase, badScore, goodScore);
          exit = 1;
          break;
        case SDLK_SPACE:
          toggle(&showAnswer);
          break;
        case SDLK_d:
          exportToDatabase(deck, outputDatabase);
          if (scoreMode)
            exportToScoreFile(scoreFileName, badScore, goodScore);
          break;
        case SDLK_j:
          deck->counts[gameItem[pos]]++;
          /* Increase and update bad score. */
          badScore++;
          snprintf(bad, COUNT_SIZE, "%d", badScore);

          if (pos + 1 < itemCount) {
            pos++;
            showAnswer = 0;
          }
          break;
        case SDLK_k:
          if (deck->counts[gameItem[pos]] > 0) {
            deck->counts[gameItem[pos]]--;
            /* Decrease and update good score. */
            goodScore++;
            snprintf(good, COUNT_SIZE, "%d", goodScore);
          }

          if (pos + 1 < itemCount) {
            pos++;
            showAnswer = 0;
          }
          break;
        case SDLK_n:
          if (pos + 1 < itemCount) {
            pos++;
            showAnswer = 0;
          }
          break;
        case SDLK_o:
          free(gameItem);
          gameItem = forgottenCards(deck->counts, deck->used, &itemCount);
          if (gameItem)
            shuffleCards(gameItem, itemCount);
          pos = 0;
          showAnswer = 0;
          break;
        case SDLK_p:
          if (pos > 0) {
            pos--;
            showAnswer = 0;
          }
          break;
        case SDLK_s:
          shuffleCards(gameItem, itemCount);
          pos = 0;
          showAnswer = 0;
        case SDLK_t:
          for (int i = 0; i < deck->used; i++)
            deck->counts[i]++;
          free(gameItem);
          gameItem = forgottenCards(deck->counts, deck->used, &itemCount);
          shuffleCards(gameItem, itemCount);
          pos = 0;
          showAnswer = 0;
          break;
        }
        updateString(deck, gameItem, pos, itemCount, &question, &answer, memory, counter);
        /* Redraw. */
        SDL_RenderClear(renderer);
        drawSmallText(renderer, memory, font, red, memoryRect);
        drawSmallText(renderer, counter, font, white, counterRect);
        if (scoreMode) {
          drawSmallText(renderer, bad, font, yellow, badScoreRect);
          drawSmallText(renderer, good, font, green, goodScoreRect);
        }
        drawText(renderer, width, question, font, white, questionRect);
        if (showAnswer)
          drawText(renderer, width, answer, font, white, answerRect);
        SDL_RenderPresent(renderer);
        break;
      }
    }
  }
  destroyDeck(deck);
  free(gameItem);
  free(outputDatabase);
  free(memory);
  free(counter);
  /* We don't need to free question and answer. */
  /* They are free with deck. */

  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
}

void updateString(Deck *deck, int *game, int pos, int itemCount, char **question, char **answer, char *memory, char *counter) {

  *question = deck->cards[game[pos]].question;
  *answer = deck->cards[game[pos]].answer;

  sprintf(memory, "%d", deck->counts[game[pos]]);
  sprintf(counter, "%d / %d", pos + 1, itemCount);
}

void windowResizeCallback(SDL_Window *window, int *width, int *height, SDL_Rect *answerRect, SDL_Rect *counterRect, SDL_Rect *goodScoreRect, SDL_Rect *badScoreRect) {
  counterRect->x -= *width;
  SDL_GetWindowSize(window, width, height);
  answerRect->y = *height / 2;
  counterRect->x += *width;
  goodScoreRect->x = *width / 3 * 2;
  badScoreRect->x = *width / 3;
}

int utf8StringWidth(char *s) {
  int width = 0;
  while (*s) {
    if ((*s & 0x80) == 0x00)
      width += 1;
    else if ((*s & 0xc0) == 0xc0)
      width += 2;

    s++;
  }
  return width;
}

size_t utf8StringLength(char *s) {
  size_t length = 0;
  while (*s)
    length += (*s++ & 0xc0) != 0x80;
  return length;
}

char *utf8strndup(char *s, int width, char **nextPtr) {
  char *ptr = s;
  int len = 0;
  while (width >= 0) {
    if ((*s & 0x80) == 0x00) {
      width -= 1;
      ptr++;
      len++;
    }
    else if ((*s & 0xc0) == 0xc0) {
      width -= 2;
      ptr += 3;
      len += 3;
    }
  }
  if (nextPtr)
    *nextPtr = ptr;

  return strndup(s, len);
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

void drawText(SDL_Renderer *renderer, int width, char *string, TTF_Font *font, SDL_Color color, SDL_Rect rect) {
  int stringWidth = utf8StringWidth(string);
  int lineCount = 0;
  int w, h;
  int y = rect.y;
  SDL_Texture *texture = NULL;
  char *stringBuffer;
  char *pCurrent = string;
  char *pNext;
  /* This is full char width. */
  int maxWidthPerLine = (width - WINDOW_PADDING * 4) / FONT_SIZE * 2;
  while (stringWidth > maxWidthPerLine) {
    stringBuffer = utf8strndup(pCurrent, maxWidthPerLine, &pNext);
    texture = createUtf8StringTexture(stringBuffer, font, color, renderer);
    if (!texture)
      return;
    SDL_QueryTexture(texture, NULL, NULL, &w, &h);
    rect.w = w;
    rect.h = h;
    rect.y = y + lineCount * (h + h / 8);
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    free(stringBuffer);
    SDL_DestroyTexture(texture);
    texture = NULL;

    pCurrent = pNext;
    stringWidth -= maxWidthPerLine;
    lineCount++;
  }

  /* In some rare case, the string is just bigger than
     maxWidthPerLine, pCurrent would be a NULL pointer because the
     property of c string. Therefore we need to check if it is
     NULL. Otherwise, an error will rise. */
  if (strcmp(pCurrent, ""))
    texture = createUtf8StringTexture(pCurrent, font, color, renderer);
  if (!texture)
    return;
  SDL_QueryTexture(texture, NULL, NULL, &w, &h);
  rect.w = w;
  rect.h = h;
  rect.y = y + lineCount * (h + h / 8);
  SDL_RenderCopy(renderer, texture, NULL, &rect);
  SDL_DestroyTexture(texture);
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

void exportToScoreFile(char *scoreFileName, int badScore, int goodScore) {
  FILE *scoreFile;
  scoreFile = fopen(scoreFileName, "w");
  fprintf(scoreFile, "%d\n%d\n", badScore, goodScore);
  char buf[64];
  time_t now = time(NULL);
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&now));
  fprintf(scoreFile, "%s\n", buf);
  fclose(scoreFile);
  printf("Score file exported: %s\n", scoreFileName);
}

void writeToLogFile(char *outputDatabase, int badScore, int goodScore) {
  FILE *log;
  log = fopen(LOG_PATH, "a");
  char buf[64];
  time_t now = time(NULL);
  strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&now));

  if (outputDatabase == NULL)
    fprintf(log, "%s, bad: %d, good: %d\n", buf, badScore, goodScore);
  else
    fprintf(log, "%s, bad: %d, good: %d, \"%s\"\n", buf, badScore, goodScore, outputDatabase);

  printf("Written to log file.\n");
}
