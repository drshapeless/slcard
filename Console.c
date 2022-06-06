#include "Config.h"
#include "Flashcard.h"
#include "Console.h"
#include "Utilities.h"

#include <stdio.h>
#include <stdlib.h>

void printQuestion(struct Flashcard card);
void printAnswer(struct Flashcard card);
void printEndGameMessage(void);

void consoleRun(struct FlashcardGame *game)
{
	printf("slcard: You are running in console mode.\n");
	int exit = 0;
	int success = newGame(game);

	if (!success) {
		printEndGameMessage();
	}

	char input;
	while ((scanf("%c", &input)) && !exit) {
		switch (input) {
		case ' ':
			if (game->showAnswer) {
				/* printAnswer(game->cards[cardIndexes[pos]]); */
				puts(currentAnswer(game));
			} else {
				printf("counts: %d\tbad: %d\tgood: %d\t%d / %d\n",
				       game->memoryCounts
				       [game->cardIndexes[game->pos]],
				       game->badScore, game->goodScore,
				       game->pos + 1, game->gameSize);
				/* printQuestion(game->cards[cardIndexes[pos]]); */
				puts(currentQuestion(game));
			}
			toggleShowAnswer(game);
			break;
		case 'n':
			success = nextCard(game);
			if (!success) {
				puts("You have reach the end.");
			}
			break;
		case 'p':
			success = previousCard(game);
			if (!success) {
				puts("You have reach the beginning.");
			}
			break;
		case 'j':
			forget(game);
			break;
		case 'k':
			remember(game);
			break;
		case 'o':
			/* Open new game. */
			success = newGame(game);
			if (!success) {
				printEndGameMessage();
			}
			break;
		case 'r':
			/* Restart current game. */
			restart(game);
			break;
		case 's':
			/* Shuffle current game. */
			shuffle(game);
			break;
		case 'q':
			exit = 1;
			break;
		case 't':
			addOneMemoryCount(game);
			break;
		case 'd':
			exportToDatabase(game);
			break;
		default:
			break;
		}
	}
}

void printEndGameMessage(void)
{
	printf("You have remembered all the cards.\n"
	       "Type t to add one count to each card.\n"
	       "Type q to exit.\n");
}
