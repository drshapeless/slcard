#include "Config.h"
#include "Console.h"
#include "Flashcard.h"
#include "Gui.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void showUsage(void);
void showVersion(void);

int main(int argc, char *argv[]) {
  FlashcardGame *game = createGame();

  int noGame = 0;
  int consoleMode = 0;

  int c;
  while ((c = getopt(argc, argv, "hncvd:f:l:o:s:")) != -1) {
    switch (c) {
    case 'h':
      showUsage();
      return 0;
    case 'v':
      showVersion();
      return 0;
    case 'n':
      noGame = 1;
      break;
    case 'c':
      consoleMode = 1;
      break;
    case 'd':
      addDatabase(game, optarg);
      break;
    case 'f':
      addFile(game, optarg);
      break;
    case 'l':
      addFileList(game, optarg);
      break;
    case 's':
      importScoreFile(game, optarg);
      break;
    case 'o':
      game->outputDatabasePath = optarg;
      break;
    case '?':
      printf("Missing argument at %c\n", optopt);
      return -1;
    default:
      return 0;
    }
  }

  if (noGame) {
    exportToDatabase(game);
    return 0;
  }

  if (consoleMode) {
    consoleRun(game);
  } else {
    guiRun(game);
  }
  return 0;
}

void showUsage(void) {
  printf("slcard [options] [file]\n"
         "-h               Show this help and quit.\n"
         "-n               Export database and do not initialize a game.\n"
         "-c               Run in console mode.\n"
         "-f               Add a plain file.\n"
         "-d file          Add a database.\n"
         "-l file          Add a file list.\n"
         "-s file          Specify the score file.\n"
         "-o file          Export as file.\n");
}

void showVersion(void) {
  puts("slcard version 2.1.0");
}
