#ifndef FLASHCARD_H
#define FLASHCARD_H

typedef struct Flashcard {
  char *question;
  char *answer;
} Flashcard;

typedef struct FlashcardGame {
  /* Array of cards. */
  Flashcard *cards;
  /* Array of memory counts. */
  int *memoryCounts;

  int usedSize;
  int allocatedSize;

  int goodScore;
  int badScore;

  int pos;
  int showAnswer;
  int *cardIndexes;
  int gameSize;

  char *outputDatabasePath;
  char *scoreFilePath;
  char *openedFile;
} FlashcardGame;

FlashcardGame *createGame(void);
void destroyGame(FlashcardGame *game);
void addDatabase(FlashcardGame *game, char *databaseName);
void addFile(FlashcardGame *game, char *fileName);
void addFileList(FlashcardGame *game, char *fileListName);
void exportToDatabase(FlashcardGame *game);
int *forgottenCards(int *counts, int usedSize, int *newSize);
void writeLog(FlashcardGame *game);
void printScore(FlashcardGame *game);
void importScoreFile(FlashcardGame *game, char *scoreFilePath);
void writeScore(FlashcardGame *game);

void shuffleCards(int *cards, int size);
void addOneMemoryCount(FlashcardGame *game);
int newGame(FlashcardGame *game);
int nextCard(FlashcardGame *game);
int previousCard(FlashcardGame *game);
void remember(FlashcardGame *game);
void forget(FlashcardGame *game);
void restart(FlashcardGame *game);
void shuffle(FlashcardGame *game);
char *currentQuestion(FlashcardGame *game);
char *currentAnswer(FlashcardGame *game);
int currentMemoryCount(FlashcardGame *game);
void toggleShowAnswer(FlashcardGame *game);
#endif /* FLASHCARD_H */
