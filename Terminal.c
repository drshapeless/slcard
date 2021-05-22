#include "Config.h"
#include "Flashcard.h"
#include "Terminal.h"
#include "Utilities.h"
#include <stdio.h>
#include <stdlib.h>

void printQuestion(Flashcard *flashcard, int i);
void printAnswer(Flashcard *flashcard, int i);
void printEndGame(Deck *deck, int *cards, int *size, int *exit);
void addOneToAllCounts(int *counts, int size);

void termRun(Deck *deck, char *database) {
  printf("slcard: You are running in terminal mode.\n");
  int exit = 0;
  int pos = 0;
  int *cards = NULL;
  int size;
  cards = forgottenCards(deck->counts, deck->used, &size);
  if (!cards)
    printEndGame(deck, cards, &size, &exit);
  else
    shuffleCards(cards, size);

  int showAnswer = 0;
  char input;
  while ((scanf("%c", &input)) && !exit) {
    switch (input) {
    case ' ':
      if (showAnswer)
	printAnswer(deck->cards, cards[pos]);
      else {
	printf("counts: %d\t%d / %d\n", pos + 1, size, deck->counts[cards[pos]]);
	printQuestion(deck->cards, cards[pos]);
      }

      toggle(&showAnswer);
      break;
    case 'n':
      if (pos + 1 < size) {
	pos++;
      }
      else
	printf("You have reach the end.\n");
      break;
    case 'p':
      if (pos - 1 >= 0) {
	pos--;
      }
      else
	printf("You have reach the beginning.\n");
      break;
    case 't':
      addOneToAllCounts(deck->counts, deck->used);
      break;
    case 'k':
      deck->counts[cards[pos]]--;
      break;
    case 'j':
      deck->counts[cards[pos]]++;
      break;
    case 'q':
      exit = 1;
      break;
    case 'd':
      exportToDatabase(deck, database);
      break;
    case 'o':
      free(cards);
      cards = forgottenCards(deck->counts, deck->used, &size);
      pos = 0;
      showAnswer = 0;
      break;
    case 'r':
      pos = 0;
      showAnswer = 0;
      break;
    case 's':
      shuffleCards(cards, size);
      pos = 0;
      showAnswer = 0;
      break;
    default:
      break;
    }
  }
}

void printEndGame(Deck *deck, int *cards, int *size, int *exit) {
  printf("You have remembered all the cards.\n"
	 "Type t to start a new game.\n"
	 "Any other key to exit.\n");
  int c = getchar();
  if (c == 't') {
    addOneToAllCounts(deck->counts, deck->used);
    cards = forgottenCards(deck->counts, deck->used, size);
    if (cards)
      shuffleCards(cards, *size);
  } else {
    *exit = 1;
  }
}

void printQuestion(Flashcard *flashcard, int i) {
  printf("%s\n", flashcard[i].question);
}

void printAnswer(Flashcard *flashcard, int i) {
  printf("%s\n", flashcard[i].answer);
}

void addOneToAllCounts(int *counts, int size) {
  for (int i = 0; i < size; i++) {
    counts[i]++;
  }
}
