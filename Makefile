CC := gcc
CFLAGS := -Wall
LDLIBS := -lpthreads
EXEC := micro-schedule
OBJS := main.o 

${EXEC}: ${OBJS}
	${CC} ${CFLAGS} -o ${EXEC} ${OBJS}

main.o: main.c 
	${CC} ${CFLAGS} -c main.c ${LDLIBS} 

clean: 
	rm ${EXEC} ${OBJS} 
