EXE=hw1

# Main target
all: $(EXE)

#  Compiler flags and libraries
#  Msys/MinGW (Windows)
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall
LIBS=-lglut -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX (macOS)
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Compile rule for .c files
.c.o:
	gcc -c $(CFLG) $<

#  Link the executable
$(EXE):lorenz.o
	gcc $(CFLG) -o $@ $^ $(LIBS)

#  Clean up build files
clean:
	$(CLEAN)