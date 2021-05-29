#include "Config.h"
#include "Flashcard.h"
#include "Utilities.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

Deck *createDeck(void) {
  Deck *deck = NULL;
  deck = malloc(sizeof(Deck));
  if (!deck) {
    printf("Cannot create deck.\n");
    return NULL;
  }
  deck->used = 0;
  deck->size = INITIAL_DECK_SIZE;
  deck->cards = malloc(sizeof(Flashcard) * INITIAL_DECK_SIZE);
  deck->counts = malloc(sizeof(int) * INITIAL_DECK_SIZE);
  return deck;
}

void destroyCard(Flashcard *card) {
  free(card->question);
  free(card->answer);
}

void destroyDeck(Deck *deck) {
  for (int i = 0; i < deck->used; i++) {
    destroyCard(&deck->cards[i]);
  }
  free(deck->cards);
  free(deck->counts);
  free(deck);
}

void addNewCard(Deck *deck, char *que, char *ans, int cnt) {
  if (deck->used >= deck->size) {
    deck->size += 100;		/* If the deck is full, add 100 more space. */
    Flashcard *cards = realloc(deck->cards, sizeof(Flashcard) * deck->size);
    if (!cards) {
      printf("addNewCard: Fail to realloc flashcards.\n");
      return;
    } else {
      deck->cards = cards;
    }

    int *counts = realloc(deck->counts, sizeof(int) * deck->size);
    if (!counts) {
      printf("addNewCard: Fail to realloc counts.\n");
      return;
    } else {
      deck->counts = counts;
    }
  }

  Flashcard fc = {.question = strdup(que), .answer = strdup(ans)};
  deck->cards[deck->used] = fc;
  deck->counts[deck->used] = cnt;
  deck->used++;
}

void addDatabase(Deck *deck, char *database) {
  printf("Adding database %s.\n", database);
  FILE *file = NULL;
  file = fopen(database, "r");
  if (!file) {
    printf("Database: %s not found.\n", database);
    return;
  }

  char q[STRING_SIZE], a[STRING_SIZE], cnt[COUNT_SIZE];
  int pos = 0;
  char c;
  char *current;
  int exit = 0;

  while (((c = fgetc(file)) != EOF) || !exit) {
    switch (c) {
    case '{':
      current = q;
      pos = 0;
      break;
    case '\"':
      while ((c = fgetc(file)) != '\"') {
        current[pos++] = c;
      }
      current[pos] = '\0';
      break;
    case ',':
      if (current == q) {
        current = a;
      }
      pos = 0;
      break;
    default:
      cnt[pos++] = c;
      while ((c = fgetc(file)) != '}') {
        cnt[pos++] = c;
      }
      cnt[pos] = '\0';
      addNewCard(deck, q, a, strtol(cnt, NULL, 10));
      if ((c = fgetc(file)) != ',') {
        exit = 1;
      }
      break;
    }
  }

  fclose(file);
  printf("Database imported: %s\n", database);
}

void addFile(Deck *deck, char *file) {
  FILE *f = NULL;
  f = fopen(file, "r");
  if (!file) {
    printf("File not found: %s\n", file);
    return;
  }

  char *q, *a;
  char *line;
  while ((line = fgetline(f))) {
    q = line;
    a = fgetline(f);
    addNewCard(deck, q, a, 1);	/* The count is 1 for every new card. */
  }

  fclose(f);
  printf("File imported: %s\n", file);
}

void addFileList(Deck *deck, char *fileList) {
  FILE *list = NULL;
  list = fopen(fileList, "r");
  if (!list) {
    printf("File list not found: %s\n", fileList);
    return;
  }
  char *line;

  while ((line = fgetline(list))) {
    if (line[0] != '#')
      addFile(deck, line);
  }

  fclose(list);
  printf("File list imported: %s\n", fileList);
}

char *databaseString(Flashcard card, int count) {
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

void exportToDatabase(Deck *deck, char *database) {
  FILE *file = NULL;
  if (database)
    file = fopen(database, "w");
  else {
    char buf[64];
    time_t now = time(NULL);
    strftime(buf, 64, "%Y%m%d%H%M%S", localtime(&now));
    strcat(buf, "db.txt");
    char filename[100] = DATABASE_DIRECTORY;
    strcat(filename, buf);
    file = fopen(filename, "w");
    database = filename;
  }

  if (!file) {
    printf("Export failed. Cannot open %s.\n", database);
  }

  int i;
  for (i = 0; i < deck->used - 1; i++) {
    char *s = databaseString(deck->cards[i], deck->counts[i]);
    fprintf(file, "%s,", s);
    free(s);
  }
  char *s = databaseString(deck->cards[i], deck->counts[i]);
  fprintf(file, "%s", s);
  free(s);

  fclose(file);
  printf("Database exported: %s\n", database);
}

int *forgottenCards(int *counts, int size, int *length) {
  int *array = malloc(sizeof(int) * size);
  int pos = 0;
  int i;
  for (i = 0; i < size; i++) {
    if (counts[i] > 0) {
      array[pos++] = i;
    }
  }
  if (!pos)
    return NULL;

  if (length)
    *length = pos;

  return array;
}

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

void shuffleCards(int *cards, int size) {
  srand(time(NULL));
  int i;
  for (i = size - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    swap(&cards[i], &cards[j]);
  }
}
