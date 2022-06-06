#include "Config.h"
#include "Flashcard.h"
#include "Utilities.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <jansson.h>

void getScore(struct FlashcardGame *game)
{
	FILE *file = NULL;
	/* if (game->scoreFilePath) { */

	/* } else { */
	/*   char *scorePath = malloc(STRING_SIZE); */
	/*   strcpy(scorePath, getenv("HOME")); */
	/*   strcat(scorePath, SCORE_PATH); */
	/*   file = fopen(scorePath, "r"); */
	/*   game->scoreFilePath = scorePath; */
	/* } */
	file = fopen(game->scoreFilePath, "r");

	if (!file) {
		printf("Default score file not found.\n");
		return;
	} else {
		char *line;
		line = fgetline(file);
		game->badScore = strtol(line, NULL, 10);
		line = fgetline(file);
		game->goodScore = strtol(line, NULL, 10);
	}
	fclose(file);
}

void importScoreFile(struct FlashcardGame *game, char *scoreFilePath)
{
	/* Don't bother to free the original one. */
	/* It doesn't matter. */
	game->scoreFilePath = scoreFilePath;
	getScore(game);
}

struct FlashcardGame *createGame(void)
{
	struct FlashcardGame *game = NULL;
	game = malloc(sizeof(struct FlashcardGame));
	if (!game) {
		printf("createGame: Cannot create game.\n");
		return NULL;
	}

	game->usedSize = 0;
	game->allocatedSize = INITIAL_GAME_SIZE;
	game->cards = malloc((sizeof(struct Flashcard) * INITIAL_GAME_SIZE));
	game->memoryCounts = malloc(sizeof(int) * INITIAL_GAME_SIZE);
	game->cardIndexes = NULL;

	game->scoreFilePath = malloc(STRING_SIZE);
	strcpy(game->scoreFilePath, getenv("HOME"));
	strcat(game->scoreFilePath, SCORE_PATH);
	game->goodScore = 0;
	game->badScore = 0;
	getScore(game);

	game->outputDatabasePath = NULL;
	game->openedFile = malloc(STRING_SIZE);
	game->openedFile[0] = '\0';
	return game;
}

void destroyCard(struct Flashcard *card)
{
	free(card->question);
	free(card->answer);
}

void destroyGame(struct FlashcardGame *game)
{
	for (int i = 0; i < game->usedSize; i++) {
		destroyCard(&game->cards[i]);
	}

	free(game->cards);
	free(game->memoryCounts);
	free(game->cardIndexes);
	free(game->openedFile);
	free(game);
}

int expandGameSize(struct FlashcardGame *game)
{
	game->allocatedSize += INITIAL_GAME_SIZE;
	struct Flashcard *cards = realloc(
		game->cards, sizeof(struct Flashcard) * game->allocatedSize);
	if (!cards) {
		printf("expandGameSize: Cannot realloc cards.\n");
		return 1;
	} else {
		game->cards = cards;
	}

	int *counts =
		realloc(game->memoryCounts, sizeof(int) * game->allocatedSize);
	if (!counts) {
		printf("expandGameSize: Cannot realloc memoryCounts.\n");
		return 1;
	} else {
		game->memoryCounts = counts;
	}

	return 0;
}

void addNewCard(struct FlashcardGame *game, char *q, char *a, int c)
{
	int failToExpand = 0;
	if (game->usedSize >= game->allocatedSize) {
		failToExpand = expandGameSize(game);
	}

	if (failToExpand) {
		return;
	}

	struct Flashcard card = { .question = strdup(q), .answer = strdup(a) };
	game->cards[game->usedSize] = card;
	game->memoryCounts[game->usedSize] = c;
	game->usedSize++;
}

void addDatabase(struct FlashcardGame *game, char *databaseName)
{
	FILE *file = NULL;
	file = fopen(databaseName, "r");
	if (!file) {
		printf("addDatabase: %s not found.\n", databaseName);
		return;
	}

	char q[STRING_SIZE], a[STRING_SIZE], c[COUNT_SIZE];
	int pos = 0;
	char currentChar;
	char *currentString;
	int exit = 0;

	/* Parse database. */
	while (((currentChar = fgetc(file)) != EOF) || !exit) {
		switch (currentChar) {
		case '{':
			currentString = q;
			pos = 0;
			break;
		case '\"':
			while ((currentChar = fgetc(file)) != '\"') {
				currentString[pos++] = currentChar;
			}
			currentString[pos] = '\0';
			break;
		case ',':
			if (currentString == q) {
				currentString = a;
			}
			pos = 0;
			break;
		default:
			c[pos++] = currentChar;
			while ((currentChar = fgetc(file)) != '}') {
				c[pos++] = currentChar;
			}
			c[pos] = '\0';
			addNewCard(game, q, a, strtol(c, NULL, 10));
			if ((currentChar = fgetc(file)) != ',') {
				exit = 1;
			}
			break;
		}
	}

	fclose(file);
	printf("Database imported: %s\n", databaseName);
	if (game->openedFile[0] != '\0') {
		strcat(game->openedFile, ", ");
		strcat(game->openedFile, databaseName);
	} else {
		strcpy(game->openedFile, databaseName);
	}
}

void addFile(struct FlashcardGame *game, char *fileName)
{
	FILE *file = NULL;
	file = fopen(fileName, "r");
	if (!file) {
		printf("File not found: %s\n", fileName);
		return;
	}

	char *q, *a;
	char *line;
	while ((line = fgetline(file))) {
		q = line;
		a = fgetline(file);
		addNewCard(game, q, a, 1);
	}

	fclose(file);
	printf("File imported: %s\n", fileName);
}

void addFileList(struct FlashcardGame *game, char *fileListName)
{
	FILE *list = NULL;
	list = fopen(fileListName, "r");
	if (!list) {
		printf("File list not found: %s\n", fileListName);
		return;
	}

	char *line = NULL;
	while ((line = fgetline(list))) {
		if (line[0] != '#') {
			char actualFilePath[STRING_SIZE];
			strcpy(actualFilePath, getenv("HOME"));
			strcat(actualFilePath, line);
			addFile(game, actualFilePath);
		}

		free(line);
	}

	fclose(list);
	if (game->openedFile[0] != '\0') {
		strcat(game->openedFile, ", ");
		strcat(game->openedFile, fileListName);
	} else {
		strcpy(game->openedFile, fileListName);
	}

	printf("File list imported: %s\n", fileListName);
}

void addJson(struct FlashcardGame *game, char *jsonFileName)
{
	FILE *f = NULL;
	f = fopen(jsonFileName, "r");
	if (!f) {
		printf("JSON not found: %s\n", jsonFileName);
		return;
	}

	/* Ignore the error. */
	json_t *js = json_loadf(f, 0, NULL);
	if (!js) {
		printf("JSON format error: %s\n", jsonFileName);
		return;
	}

	int size = (int)json_array_size(js);

	for (int i = 0; i < size; i++) {
		json_t *currentObj = json_array_get(js, i);
		json_t *questionObj = json_object_get(currentObj, "question");
		char *question = strdup(json_string_value(questionObj));

		json_t *answerObj = json_object_get(currentObj, "answer");
		char *answer = strdup(json_string_value(answerObj));

		addNewCard(game, question, answer, 1);
	}

	json_decref(js);
	fclose(f);
	printf("JSON imported: %s\n", jsonFileName);
}

char *databaseString(struct Flashcard card, int count)
{
	char *s = NULL;
	s = malloc(sizeof(char) * STRING_SIZE);
	int pos = 0;
	int i;
	s[pos++] = '{';
	s[pos++] = '\"';
	for (i = 0; card.question[i] != '\0'; i++) {
		s[pos++] = card.question[i];
	}
	s[pos++] = '\"';
	s[pos++] = ',';
	s[pos++] = '\"';
	for (i = 0; card.answer[i] != '\0'; i++) {
		s[pos++] = card.answer[i];
	}
	s[pos++] = '\"';
	s[pos++] = ',';
	int length = snprintf(NULL, 0, "%d", count);
	char cnt[length + 1];
	snprintf(cnt, length + 1, "%d", count);
	for (i = 0; cnt[i] != '\0'; i++) {
		s[pos++] = cnt[i];
	}
	s[pos++] = '}';
	s[pos] = '\0';
	return s;
}

void exportToDatabase(struct FlashcardGame *game)
{
	FILE *file = NULL;
	if (game->outputDatabasePath) {
		file = fopen(game->outputDatabasePath, "w");
		if (!file) {
			printf("Export failed. Cannot open %s.\n",
			       game->outputDatabasePath);
			return;
		}
	} else {
		char buf[64];
		time_t now = time(NULL);
		strftime(buf, 64, "%Y%m%d%H%M%S", localtime(&now));
		strcat(buf, "db.txt");
		char filename[100];
		strcpy(filename, getenv("HOME"));
		strcat(filename, DATABASE_DIRECTORY);
		strcat(filename, buf);
		file = fopen(filename, "w");
	}

	int i;
	for (i = 0; i < game->usedSize - 1; i++) {
		char *s = databaseString(game->cards[i], game->memoryCounts[i]);
		fprintf(file, "%s,", s);
		free(s);
	}

	char *s = databaseString(game->cards[i], game->memoryCounts[i]);
	fprintf(file, "%s", s);
	free(s);

	printf("Database exported: %s\n", game->outputDatabasePath);
	fclose(file);
}

int *forgottenCards(int *counts, int usedSize, int *newSize)
{
	int *arr = malloc(sizeof(int) * usedSize);
	int pos = 0;
	int i;
	for (i = 0; i < usedSize; i++) {
		if (counts[i] > 0) {
			arr[pos++] = i;
		}
	}
	if (!pos)
		return NULL;
	if (newSize)
		*newSize = pos;

	/* Reallocating the array may slightly slow down the game. */
	/* Not recommend to realloc this array, an array of integer is affordable. */
	/* arr = (int*)realloc(arr, pos); */

	return arr;
}

void writeLog(struct FlashcardGame *game)
{
	FILE *log;
	char actualLogPath[100];
	strcpy(actualLogPath, getenv("HOME"));
	strcat(actualLogPath, LOG_PATH);
	log = fopen(actualLogPath, "a");
	char buf[64];
	time_t now = time(NULL);
	strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&now));

	fprintf(log, "%s, bad: %d, good: %d, \"%s\"\n", buf, game->badScore,
		game->goodScore, game->openedFile);
}

void printScore(struct FlashcardGame *game)
{
	char buf[64];
	time_t now = time(NULL);
	strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&now));

	printf("%s, bad: %d, good: %d\n", buf, game->badScore, game->goodScore);
}

void writeScore(struct FlashcardGame *game)
{
	FILE *file = NULL;
	file = fopen(game->scoreFilePath, "w");
	if (file) {
		char buf[64];
		time_t now = time(NULL);
		strftime(buf, 64, "%Y-%m-%d %H:%M:%S", localtime(&now));
		fprintf(file, "%i\n%i\n%s\n", game->badScore, game->goodScore,
			buf);
	} else {
		printf("writeScore: Cannot open score file.\n");
	}
}

void shuffleCards(int *cards, int size)
{
	srand(time(NULL));
	int i;
	for (i = size - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		swap(&cards[i], &cards[j]);
	}
}

void addOneMemoryCount(struct FlashcardGame *game)
{
	for (int i = 0; i < game->usedSize; i++) {
		game->memoryCounts[i]++;
	}
}

int newGame(struct FlashcardGame *game)
{
	/* Return 0 if failed. */
	if (game->cardIndexes) {
		free(game->cardIndexes);
	}
	game->cardIndexes = forgottenCards(game->memoryCounts, game->usedSize,
					   &game->gameSize);
	if (!game->cardIndexes) {
		return 0;
	} else {
		shuffleCards(game->cardIndexes, game->gameSize);
		game->pos = 0;
		game->showAnswer = 0;
		return 1;
	}
}

int nextCard(struct FlashcardGame *game)
{
	/* Return 1 if success. */
	if (game->pos + 1 < game->gameSize) {
		game->pos++;
		game->showAnswer = 0;
		return 1;
	} else {
		return 0;
	}
}

int previousCard(struct FlashcardGame *game)
{
	/* Return 1 if success. */
	if (game->pos - 1 >= 0) {
		game->pos--;
		game->showAnswer = 0;
		return 1;
	} else {
		return 0;
	}
}

void remember(struct FlashcardGame *game)
{
	if (game->memoryCounts[game->cardIndexes[game->pos]]) {
		game->memoryCounts[game->cardIndexes[game->pos]]--;
		game->goodScore++;
	}
}

void forget(struct FlashcardGame *game)
{
	game->memoryCounts[game->cardIndexes[game->pos]]++;
	game->badScore++;
}

void restart(struct FlashcardGame *game)
{
	game->pos = 0;
	game->showAnswer = 0;
}

void shuffle(struct FlashcardGame *game)
{
	shuffleCards(game->cardIndexes, game->gameSize);
	game->pos = 0;
	game->showAnswer = 0;
}

struct Flashcard currentCard(struct FlashcardGame *game)
{
	return game->cards[game->cardIndexes[game->pos]];
}

char *currentQuestion(struct FlashcardGame *game)
{
	return currentCard(game).question;
}

char *currentAnswer(struct FlashcardGame *game)
{
	return currentCard(game).answer;
}

int currentMemoryCount(struct FlashcardGame *game)
{
	return game->memoryCounts[game->cardIndexes[game->pos]];
}

void toggleShowAnswer(struct FlashcardGame *game)
{
	toggle(&game->showAnswer);
}
