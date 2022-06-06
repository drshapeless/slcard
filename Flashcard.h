#ifndef FLASHCARD_H
#define FLASHCARD_H

struct Flashcard {
	char *question;
	char *answer;
};

struct FlashcardGame {
	/* Array of cards. */
	struct Flashcard *cards;
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
};

struct FlashcardGame *createGame(void);
void destroyGame(struct FlashcardGame *game);
void addDatabase(struct FlashcardGame *game, char *databaseName);
void addFile(struct FlashcardGame *game, char *fileName);
void addFileList(struct FlashcardGame *game, char *fileListName);
void addJson(struct FlashcardGame *game, char *jsonFileName);
void exportToDatabase(struct FlashcardGame *game);
int *forgottenCards(int *counts, int usedSize, int *newSize);
void writeLog(struct FlashcardGame *game);
void printScore(struct FlashcardGame *game);
void importScoreFile(struct FlashcardGame *game, char *scoreFilePath);
void writeScore(struct FlashcardGame *game);

void shuffleCards(int *cards, int size);
void addOneMemoryCount(struct FlashcardGame *game);
int newGame(struct FlashcardGame *game);
int nextCard(struct FlashcardGame *game);
int previousCard(struct FlashcardGame *game);
void remember(struct FlashcardGame *game);
void forget(struct FlashcardGame *game);
void restart(struct FlashcardGame *game);
void shuffle(struct FlashcardGame *game);
char *currentQuestion(struct FlashcardGame *game);
char *currentAnswer(struct FlashcardGame *game);
int currentMemoryCount(struct FlashcardGame *game);
void toggleShowAnswer(struct FlashcardGame *game);
#endif /* FLASHCARD_H */
