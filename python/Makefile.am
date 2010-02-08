
EXTRA_DIST = caca.txt pypycaca.c pypycaca.h snake.py test1.py test2.py

CC = gcc
RM = rm -f
CACAFLAGS = `caca-config --cflags`
CACALIBS  = `caca-config --libs`

PYTHONFLAGS = -I/usr/include/python2.4
PYTHONLIBS = -lpython2.4

NAME = caca.so

all:

python:
	$(CC) pypycaca.c -c $(CACAFLAGS) $(PYTHONFLAGS) -Wall
	$(LD) pypycaca.o -o $(NAME) $(CACALIBS) $(PYTHONLIBS) -shared


clean:
	$(RM) *.o $(NAME)
