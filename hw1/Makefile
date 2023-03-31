# makefile for raytracer

# Compiler

CC = g++

# Compiler flags

CFLAGS = -Wall -O3 -fopenmp

# Linker flags

LDFLAGS = -fopenmp

# Libraries

LIBS = -lm

# Source files

SRCS = raytracer/main.cpp raytracer/pugixml/src/pugixml.cpp

# Object files

OBJS = $(SRCS:.cpp=.o)

# Executable

EXEC = rtracer

# Makefile rules

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC) *.ppm

# End of Makefile

