CC = gcc
CXX = g++

#include assimp
COMPILE_FLAGS = -Wall -ggdb	
LINK_FLAGS = -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

cpp_files = test.cpp
objects = $(cpp_files:.cpp=.o)

.PHONY: all
.PHONY: clean

all: $(objects) glad.o
	$(CXX) $(COMPILE_FLAGS) $(objects) glad.o -o main $(LINK_FLAGS) 

$(objects): %.o: %.cpp 
	$(CXX) $(COMPILE_FLAGS) -c $< -o $@ 

glad.o: glad.c
	$(CC) $(COMPILE_FLAGS) -c $< -o $@ 

clean:
	rm -f *.o main glad.o

	