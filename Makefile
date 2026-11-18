expense_splitter: main.o list_manager.o file_io.o settlements.o ui.o analytics.o
	gcc -Wall -Wextra main.o list_manager.o file_io.o settlements.o ui.o analytics.o -lm -o expense_splitter

main.o: main.c
	gcc -Wall -Wextra -c main.c -o main.o

list_manager.o: list_manager.c
	gcc -Wall -Wextra -c list_manager.c -o list_manager.o

file_io.o: file_io.c
	gcc -Wall -Wextra -c file_io.c -o file_io.o

settlements.o: settlements.c
	gcc -Wall -Wextra -c settlements.c -o settlements.o

ui.o: ui.c
	gcc -Wall -Wextra -c ui.c -o ui.o

analytics.o: analytics.c
	gcc -Wall -Wextra -c analytics.c -o analytics.o