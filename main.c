#include "Config.h"
#include "Flashcard.h"
#include "GameView.h"
#include "Terminal.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void showUsage(void);

int main(int argc, char *argv[]) {
  Deck *deck = createDeck();
  char *databaseName = NULL;
  int noGame = 0;
  int runInTerminal = 0;
  int noScore = 0;

  char *scoreFileName = NULL;

  if (argc == 1) {
    showUsage();
    return 0;
  } else {
    int pos;
    for (pos = 1; pos < argc; ++pos) {
      if (argv[pos][0] == '-') {
        switch (argv[pos][1]) {
        case 'h':
          showUsage();
          return 0;
          break;
        case 'd':
          addDatabase(deck, argv[++pos]);
          break;
        case 'l':
          addFileList(deck, argv[++pos]);
          break;
        case 'o':
          databaseName = strdup(argv[++pos]);
          break;
        case 'n':
          noGame = 1;
          break;
        case 's':
          scoreFileName = strdup(argv[++pos]);
          break;
        case 't':
          runInTerminal = 1;
          break;
        default:
          showUsage();
          return -1;
          break;
        }
      } else {
        addFile(deck, argv[pos]);
      }
    }
  }

  if (noGame) {
    exportToDatabase(deck, databaseName);
    return 0;
  }

  if (scoreFileName == NULL && !noScore) {
    scoreFileName = malloc(STRING_SIZE);
    strcpy(scoreFileName, SCORE_PATH);
  }

  if (runInTerminal) {
    termRun(deck, databaseName);
  } else {
    /* Initializing SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      printf("Unable to initialize SDL: %s\n", SDL_GetError());
      return 1;
    }
    /* Initializing SDL_ttf */
    if (TTF_Init() == -1) {
      printf("Unable to initialize SDL_ttf.\n");
      SDL_Quit();
      return 1;
    }
    run(deck, databaseName, scoreFileName);
  }

  return 0;
}

void showUsage(void) {
  printf(
         "slcard [options] [file]\n"
         "-h               Show this help and quit.\n"
         "-n               Export database and do not initialize a game.\n"
         "-t               Run in terminal mode.\n"
         "-d file          Treat the file as database.\n"
         "-l file          Treat the file as file list.\n"
         "-s file          Treat the file as score file.\n"
         "-o file          Export as file.\n"
         );
}
