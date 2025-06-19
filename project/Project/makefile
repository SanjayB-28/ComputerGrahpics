# Terrain Generation Project
EXE=final

# Main target
all: $(EXE)

# Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
# OSX
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES)
LIBS=-framework GLUT -framework OpenGL
# Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
# OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
main.o: main.c CSCIx229.h terrain.h objects.h particles.h
terrain.o: terrain.c terrain.h CSCIx229.h
objects.o: objects.c objects.h terrain.h CSCIx229.h
particles.o: particles.c particles.h terrain.h CSCIx229.h
fatal.o: fatal.c CSCIx229.h
errcheck.o: errcheck.c CSCIx229.h
print.o: print.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
loadobj.o: loadobj.c CSCIx229.h
projection.o: projection.c CSCIx229.h
shaders.o: shaders.c CSCIx229.h
celestial.o: celestial.c celestial.h terrain.h
clouds.o: clouds.c clouds.h
camera.o: camera.c camera.h
# Create archive
CSCIx229.a: fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

# Link
final: main.o terrain.o objects.o particles.o shaders.o celestial.o clouds.o camera.o clouds.o CSCIx229.a
	gcc $(CFLG) -o $@ $^ $(LIBS)

# Clean
clean:
	$(CLEAN)