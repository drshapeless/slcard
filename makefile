CC = clang
LIBS = -lSDL2 -lSDL2_ttf
OBJS = main.o Console.o Flashcard.o Utilities.o Gui.o

slcard: $(OBJS)
	$(CC) $(LIBS) $(OBJS) -o slcard

main.o : Config.h Flashcard.h Console.h Gui.h main.c
	$(CC) $(LIBS) -c main.c

Flashcard.o : Config.h Flashcard.h Flashcard.c
	$(CC) -c Flashcard.c

Console.o : Config.h Flashcard.h Console.h Console.c
	$(CC) -c Console.c

Gui.o : Config.h Flashcard.h Gui.h Gui.c
	$(CC) -c Gui.c

Utilities.o : Utilities.h Utilities.c
	$(CC) -c Utilities.c

.PHONY : clean install uninstall

clean :
	rm $(OBJS) slcard

install :
	cp slcard /usr/local/bin/

uninstall :
	rm /usr/local/bin/slcard
