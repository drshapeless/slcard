CC = clang
LIBS = -lSDL2 -lSDL2_ttf
OBJS = main.o Flashcard.o Terminal.o GameView.o Utilities.o

slcard : $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o slcard

main.o : Config.h Flashcard.h main.c
	$(CC) $(LIBS) -c main.c

Flashcard.o : Config.h Flashcard.h Flashcard.c
	$(CC) -c Flashcard.c

Terminal.o : Config.h Flashcard.h Terminal.h Terminal.c
	$(CC) -c Terminal.c

GameView.o : Config.h Flashcard.h GameView.h GameView.c
	$(CC) $(LIBS) -c GameView.c

Utilities.o : Utilities.h Utilities.c
	$(CC) -c Utilities.c

.PHONY : clean install uninstall
clean :
	rm $(OBJS) slcard
install :
	cp slcard /usr/local/bin/
uninstall :
	rm /usr/local/bin/slcard
