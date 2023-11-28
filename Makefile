OPT = -Wall -Wextra -Werror -pthread -c #add -g for debug
OBJ = evidence.o ghost.o house.o hunter.o logger.o main.o room.o utils.o
LIBS = #-fsanitize=thread# uncomment if needed
EXEC = fp

all: $(OBJ)
	gcc $(OBJ) $(LIBS) -o $(EXEC)

evidence.o:
	gcc $(OPT) evidence.c
ghost.o:
	gcc $(OPT) ghost.c 
house.o:
	gcc $(OPT) house.c 
hunter.o:
	gcc $(OPT) hunter.c 
logger.o:
	gcc $(OPT) logger.c 
main.o:
	gcc $(OPT) main.c 
room.o:
	gcc $(OPT) room.c 
utils.o:
	gcc $(OPT) utils.c 

clean:
	rm -f $(OBJ) $(EXEC)
