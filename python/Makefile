CC = gcc
RM = rm -f
CACAFLAGS = `caca-config --cflags`
CACALIBS  = `caca-config --libs`

PYTHONFLAGS = -I/usr/include/python2.4
PYTHONLIBS = -lpython2.4

NAME = caca.so

all:
	$(CC) pypycaca.c -c $(CACAFLAGS) $(PYTHONFLAGS) -Wall 
	$(LD) pypycaca.o -o $(NAME) $(CACALIBS) $(PYTHONLIBS) -shared


clean:
	$(RM) *.o $(NAME)