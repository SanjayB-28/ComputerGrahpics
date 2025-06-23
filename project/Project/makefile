# Terrain Generation Project
EXE=final

# Main target
all: final

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
main.o: main.c CSCIx229.h landscape.h objects.h weather_particles.h
landscape.o: landscape.c landscape.h CSCIx229.h
objects.o: objects.c objects.h landscape.h CSCIx229.h
weather_particles.o: weather_particles.c weather_particles.h landscape.h CSCIx229.h
fatal.o: fatal.c CSCIx229.h
errcheck.o: errcheck.c CSCIx229.h
print.o: print.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
loadobj.o: loadobj.c CSCIx229.h
projection.o: projection.c CSCIx229.h
shaders.o: shaders.c CSCIx229.h
sky.o: sky.c sky.h landscape.h
sky_clouds.o: sky_clouds.c sky_clouds.h
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
final: main.o landscape.o objects.o weather_particles.o shaders.o sky.o sky_clouds.o camera.o sky_clouds.o CSCIx229.a
	gcc $(CFLG) -o $@ $^ $(LIBS)

# Clean
clean:
	$(CLEAN)