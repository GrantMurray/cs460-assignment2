CC := gcc
CFLAGS := -Wall
EXEC := output
OBJS := main.o 

output: ${OBJS}
	${CC} ${CFLAGS} -o output ${OBJS}

main.o: main.c 
	${CC} ${CFLAGS} -c main.c 

clean: 
	rm ${EXEC} ${OBJS} 

