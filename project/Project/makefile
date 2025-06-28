EXE=final

all: final

# Windows
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW         
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm  
CLEAN=rm -f *.exe *.o *.a        
else
# macOS
ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)  
CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES) -I/opt/homebrew/opt/sdl2/include/SDL2 -I/opt/homebrew/opt/sdl2_mixer/include/SDL2
LIBS=-framework GLUT -framework OpenGL -L/opt/homebrew/opt/sdl2/lib -L/opt/homebrew/opt/sdl2_mixer/lib -lSDL2 -lSDL2_mixer
# Linux/Unix
else
CFLG=-O3 -Wall                   
LIBS=-lGLEW -lglut -lGLU -lGL -lm -lSDL2 -lSDL2_mixer
endif
CLEAN=rm -f $(EXE) *.o *.a
endif

main.o: main.c CSCIx229.h landscape.h
landscape.o: landscape.c landscape.h CSCIx229.h

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

fractal_tree.o: fractal_tree.c fractal_tree.h
	gcc -c $(CFLG) fractal_tree.c

objects_render.o: objects_render.c objects_render.h landscape.h fractal_tree.h
	gcc -c $(CFLG) objects_render.c

particles.o: particles.c particles.h
	gcc -c $(CFLG) particles.c

sound.o: sound.c sound.h
	gcc -c $(CFLG) sound.c

CSCIx229.a: fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
	ar -rcs $@ $^

.c.o:
	gcc -c $(CFLG) $<
.cpp.o:
	g++ -c $(CFLG) $<

OBJS = main.o landscape.o shaders.o sky.o sky_clouds.o camera.o fractal_tree.o objects_render.o particles.o boulder.o grass.o sound.o

final: $(OBJS) CSCIx229.a
	$(CC) $(CFLAGS) -o final $(OBJS) CSCIx229.a $(LIBS)

grass.o: grass.c grass.h
	$(CC) $(CFLAGS) -c grass.c

boulder.o: boulder.c boulder.h
	$(CC) $(CFLG) -c boulder.c

clean:
	$(CLEAN)