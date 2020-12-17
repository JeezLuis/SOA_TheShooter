all: Shooter

main.o: main.c commands.h
	gcc main.c -c -Wall -Wextra

utilities.o: utilities.c utilities.h
	gcc utilities.c -c -Wall -Wextra

commands.o: commands.c commands.h info.h find.h delete.h
	gcc commands.c -c -Wall -Wextra

info.o: info.c info.h utilities.h
		gcc info.c -c -Wall -Wextra

find.o: find.c find.h utilities.h
		gcc find.c -c -Wall -Wextra

delete.o: delete.c delete.h utilities.h
		gcc delete.c -c -Wall -Wextra

Shooter: main.o commands.o info.o find.o delete.o utilities.o
	gcc main.o commands.o info.o find.o delete.o utilities.o -o Shooter -Wall -Wextra

clean:
	rm *.o Shooter
