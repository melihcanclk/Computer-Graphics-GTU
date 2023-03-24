#create makefile for main.cc

#compiler
CC = g++
#compiler flags
CFLAGS = -Wall -g
#linker flags
LFLAGS = -Wall -g
#source files
SRCS = main.cc
#object files
OBJS = $(SRCS:.cc=.o)
#executable file
MAIN = main
#makefile rules

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(MAIN)

.cc.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(MAIN)

#end of makefile