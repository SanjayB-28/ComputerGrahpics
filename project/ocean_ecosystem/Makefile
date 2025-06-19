# Name of the final executable
EXE=final

# Compiler and flags
CC=gcc
# -Iinclude tells compiler to look in the 'include' folder.
# -Isrc tells compiler to look in the 'src' folder for engine headers.
CFLG=-O3 -Wall -Iinclude -Isrc

# List of all source files for the project.
# CORRECTED: Added the missing fatal.c utility file
SRCS = src/main.c \
       src/engine/renderer.c \
       src/engine/camera.c \
       src/engine/math_utils.c \
       src/utils/errcheck.c \
       src/utils/fatal.c

# Automatically generate object file names from source file names
OBJS = $(SRCS:.c=.o)


# --- Platform Specific Configuration ---

# Msys/MinGW on Windows
ifeq "$(OS)" "Windows_NT"
CFLG += -DUSEGLEW
LIBS = -lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN = rm -f *.exe $(OBJS)

# macOS
else ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'| awk '{if($$1 > 19) print 2; else print 1;}')
# Add flags for macOS, USEGLEW, and the Homebrew GLEW include path
CFLG += -DUSEGLEW -DAPPLE -DRES=$(RES) -Wno-deprecated-declarations -I$(shell brew --prefix glew)/include
# Add linker path for Homebrew GLEW and link all libraries
LIBS = -L$(shell brew --prefix glew)/lib -framework GLUT -framework OpenGL -lGLEW
CLEAN = rm -f $(EXE) $(OBJS)

# Linux/Unix/Solaris
else
CFLG += -DUSEGLEW
LIBS = -lglut -lGLU -lGL -lm -lGLEW
CLEAN = rm -f $(EXE) $(OBJS)
endif


# --- Build Rules ---

# The default target
all: $(EXE)

# Rule to link the final executable
$(EXE): $(OBJS)
	$(CC) $(CFLG) -o $@ $^ $(LIBS)

# Generic rule to compile a .c file into a .o file
%.o: %.c
	$(CC) $(CFLG) -c $< -o $@

# Rule to clean up the project directory
clean:
	$(CLEAN)