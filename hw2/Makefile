# Main target
all: hw2

EXE=hw2
LIBOBJS=fatal.o print.o projection.o
LIB=CSCIx229.a
OBJS=hw2.o objects.o

ifeq "$(OS)" "Windows_NT"
CFLG=-O -Wall
LIBS=-lfreeglut -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o
else
ifeq "$(shell uname)" "Darwin"
CFLG=-O -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
else
CFLG=-O -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
CLEAN=rm -f $(EXE) *.o
endif

.c.o:
	gcc -c $(CFLG) $<

$(LIB):$(LIBOBJS)
	ar -rcs $@ $^

$(EXE):$(OBJS) $(LIB)
	gcc -o $@ $(OBJS) $(LIB) $(LIBS)

clean:
	$(CLEAN) $(LIB)

hw2.o: hw2.c CSCIx229.h
objects.o: objects.c CSCIx229.h
fatal.o: fatal.c CSCIx229.h
print.o: print.c CSCIx229.h
projection.o: projection.c CSCIx229.h
