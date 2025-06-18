EXE=hw6

all: $(EXE)

ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=-lglut32cu -lglu32 -lopengl32
CLEAN=del *.exe *.o *.a

else ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations -arch arm64
LIBS=-framework GLUT -framework OpenGL
CLEAN=rm -f $(EXE) *.o *.a

else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
CLEAN=rm -f $(EXE) *.o *.a
endif

hw6.o: hw6.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
print.o: print.c CSCIx229.h
object.o: object.c CSCIx229.h

CSCIx229.a:loadtexbmp.o print.o object.o 
	ar -rcs $@ $^

.c.o:
	gcc -v -c $(CFLG) $<
.cpp.o:
	g++ -v -c $(CFLG) $<

hw6: hw6.o CSCIx229.a
	gcc -O3 -o $@ $^ $(LIBS)

clean:
	$(CLEAN)
