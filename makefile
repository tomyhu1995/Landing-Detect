CC = g++
CFLAGS += -Wall
CFLAGS += -g

EXE_NAME = Land_detect

LIB = 

INC = 
INC += -I.

SRC = 
SRC = ./Line_find

LDFLAG = 
LDFLAG += `pkg-config --cflags opencv` `pkg-config --libs opencv`

SRC_FILES =  $(addsuffix .cpp, $(SRC))
OBJ_FILES =  $(addsuffix .o, $(SRC))

all:${EXE_NAME}

${EXE_NAME}:${OBJ_FILES} ${OBJ_FILES_c}
	${CC} ${CFLAGS} ${INC} ${LIB} ${OBJ_FILES} -o $@ ${LDFLAG}
	${CC} ${CFLAGS} ${INC} ${LIB} Canny.cpp -o Canny ${LDFLAG}
${OBJ_FILES}:%.o: %.cpp
	${CC} ${CFLAGS} -c ${INC} ${STD11} $< -o $@
clean:
	rm ${EXE_NAME} ${OBJ_FILES} Canny