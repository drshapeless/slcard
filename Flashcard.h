#ifndef FLASHCARD_H
#define FLASHCARD_H

typedef struct Flashcard {
  char *question;
  char *answer;
} Flashcard;

typedef struct Deck {
  Flashcard *cards;
  int *counts;
  int used;
  int size;
} Deck;

Deck *createDeck(void);
void destroyDeck(Deck *deck);
void addDatabase(Deck *deck, char *database);
void addFile(Deck *deck, char *file);
void addFileList(Deck *deck, char *fileList);
void exportToDatabase(Deck *deck, char *database);
int *forgottenCards(int *counts, int size, int *length);
void shuffleCards(int *cards, int size);

#endif /* FLASHCARD_H */
