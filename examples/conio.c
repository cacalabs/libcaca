/*4il.c (4 in line) - compiled under Borland C++ 5.0.
 *Version : Beta
 *Author : Cheok Yan Cheng
 *Date started coding : 11/10/2000
 *Date finished coding : 13/10/2000
 */

/*If you had encounted any problem, bugs or comment on this source code
 *you are welcomed to contact me by my e-mail : yccheok@yahoo.com
 */

/*Some terms in this source code that you encounted may looked strange to you.
 *However, I have no time to include the explaination in detail.
 *If you really want to know, please contact me through e-mail.
 *Sorry for the inconvenience
 */

/*Ported to libcaca
 *Copyright (c) 2009-2014 Sam Hocevar <sam@hocevar.net>
*/

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "caca_conio.h"

#undef  max

#define X_BOARD 8
#define Y_BOARD 8
#define BOARD_SIZE  X_BOARD*Y_BOARD

#define BOOLEAN int
#define TRUE 1
#define FALSE 0

#define WIN 1
#define LOSE -1
#define DRAW 0
#define OK 2

#define COMPUTER 0
#define HUMAN 1
#define EMPTY -1
#define BASE -2

#define WIN_MESSAGE  "I Win! Press Any Key To Continue...\n"
#define LOSE_MESSAGE "You Win! Press Any Key To Continue...\n"
#define DRAW_MESSAGE "Draw! Press Any Key To Continue...\n"

BOOLEAN computerMove(void);
BOOLEAN doubleHead(int, int [Y_BOARD][X_BOARD]);
BOOLEAN humanMove(int);
BOOLEAN isFull(void);
BOOLEAN isWon(int, int [Y_BOARD][X_BOARD]);
BOOLEAN twoSnake(int, int, int [Y_BOARD][X_BOARD]);
int bestmove(void);
int max(int [X_BOARD]);
int status(void);
int xDoubleHead(int, int [Y_BOARD][X_BOARD]);
int xTwoSnake(int, int [Y_BOARD][X_BOARD]);
int xWon(int, int [Y_BOARD][X_BOARD]);
int y_Base(int, int [Y_BOARD][X_BOARD]);
void duplicate(int [Y_BOARD][X_BOARD], int [Y_BOARD][X_BOARD]);
void drawBoard(void);
void drawPiece(void);
void genNumWin(int [X_BOARD]);
void getHumanMove(void);
void init(void);
void makeMove(int, int, int [Y_BOARD][X_BOARD]);
void sorting(int n[X_BOARD]);

int move[X_BOARD] = {3, 4, 2, 5, 1, 6, 0, 7};
int col[X_BOARD]  = {-1,-1,-1,-1,-1,-1,-1,-1};
int square[Y_BOARD][X_BOARD] =
                        { {-1, -1, -1, -1, -1, -1, -1, -1},
                          {-1, -1, -1, -1, -1, -1, -1, -1},
                          {-1, -1, -1, -1, -1, -1, -1, -1},
                          {-1, -1, -1, -1, -1, -1, -1, -1},
                          {-1, -1, -1, -1, -1, -1, -1, -1},
                          {-1, -1, -1, -1, -1, -1, -1, -1},
                          {-1, -1, -1, -1, -1, -1, -1, -1},
                          {-2, -2, -2, -2, -2, -2, -2, -2},
                        };

BOOLEAN computerMove(void)
{
  int x_best;
  if (isFull())
    return FALSE;
  x_best = bestmove();
  gotoxy(52,2);
  printf("x:%d, y:%d\n" ,x_best+1 ,Y_BOARD-y_Base(x_best,square));
  makeMove(COMPUTER, x_best, square);
  return TRUE;
}

BOOLEAN doubleHead(int who, int xsquare[Y_BOARD][X_BOARD])
{
  int x,y;
  for(y=0; y<Y_BOARD; y++)
  {
    for(x=0; x<(X_BOARD-4); x++)
    {
      if( xsquare[y][x] == BASE &&
          xsquare[y][x+1] == who &&
          xsquare[y][x+2] == who &&
          xsquare[y][x+3] == who &&
          xsquare[y][x+4] == BASE )
      return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN humanMove(int x)
{
  gotoxy(1,20);
  if ((x < 0) || x >= X_BOARD)
    return FALSE;
  if (y_Base(x, square) == -1) /*indicate no base at that x-coulomb*/
    return FALSE;
  if (isFull()) /*the board is full*/
    return FALSE;
  gotoxy(52,5);
  printf("x:%d, y:%d\n" ,x+1 ,Y_BOARD-y_Base(x,square));
  makeMove(HUMAN,x,square);
  return TRUE;
}

BOOLEAN isFull()
{
  int x;
  for(x=0; x<X_BOARD; x++)
  {
    if(square[0][x] == EMPTY || square[0][x] == BASE)
      return FALSE;
  }
  return TRUE;
}

BOOLEAN isWon(int who, int xsquare[Y_BOARD][X_BOARD])
{
  int x,y;
  for(x=0; x<X_BOARD; x++)
  {
    for(y=0; y<Y_BOARD; y++)
    {
      /*horizontal position*/
      if( (x+3)<X_BOARD &&
          xsquare[y][x]   == who &&
          xsquare[y][x+1] == who &&
          xsquare[y][x+2] == who &&
          xsquare[y][x+3] == who
        )
        return TRUE;

      /*vertical position*/
      if( (y+3)<Y_BOARD &&
          xsquare[y][x]   == who &&
          xsquare[y+1][x] == who &&
          xsquare[y+2][x] == who &&
          xsquare[y+3][x] == who
        )
        return TRUE;

      /*downstair diagonal position*/
      if( (x+3)<X_BOARD             &&
          (y+3)<Y_BOARD             &&
          xsquare[y][x]      == who &&
          xsquare[y+1][x+1]  == who &&
          xsquare[y+2][x+2]  == who &&
          xsquare[y+3][x+3]  == who
        )
        return TRUE;

      /*upstair diagonal position*/
      if( (x+3)<X_BOARD             &&
          (y-3)>=0                  &&
          xsquare[y][x]      == who &&
          xsquare[y-1][x+1]  == who &&
          xsquare[y-2][x+2]  == who &&
          xsquare[y-3][x+3]  == who
        )
        return TRUE;
    }
  }
  return FALSE;
}

BOOLEAN twoSnake(int who, int x, int xsquare[Y_BOARD][X_BOARD])
{
  int xxsquare[Y_BOARD][X_BOARD];
  int n[Y_BOARD] = {0,0,0,0,0,0,0,0};
  int i;

  for(i=0; i<Y_BOARD; i++)
  {
    if(xsquare[i][x] == BASE || xsquare[i][x] == EMPTY)
    {
      duplicate(xxsquare, xsquare);
      xxsquare[i][x] = who;

      if(isWon(who, xxsquare))
        n[i] = TRUE;
    }
  }

  for(i=0; i<(Y_BOARD-1); i++)
  {
    if( n[i] == TRUE && n[i+1] == TRUE )
      return TRUE;
  }
  return FALSE;
}

int bestmove()
{
  int xsquare[Y_BOARD][X_BOARD], n[X_BOARD], i, snake;

  gotoxy(1,19);
  textcolor(4);
  if(xWon(COMPUTER, square) != -1)
  {
    cprintf("Computer Previous Depth : +1\n");
    return xWon(COMPUTER, square);
  }
  if(xWon(HUMAN, square) != -1)
  {
    cprintf("Computer Previous Depth : -1\n");
    return xWon(HUMAN, square);
  }

  for(i=0; i<X_BOARD; i++)
  {
    if(y_Base(move[i], square) != -1 && col[move[i]] == COMPUTER)
    {
      duplicate(xsquare, square);
      makeMove(COMPUTER, move[i], xsquare);
      if(xWon(HUMAN, xsquare) == -1)
      {
        cprintf("Computer Previous Depth : +2\n");
        return move[i];
      }
    }
  }

  if(xDoubleHead(COMPUTER, square) != -1)
  {
    duplicate(xsquare, square);
    makeMove(COMPUTER, xDoubleHead(COMPUTER, xsquare), xsquare);
    if(xWon(HUMAN, xsquare) == -1)
    {
      cprintf("Computer Previous Depth : +3\n");
      return xDoubleHead(COMPUTER, square);
    }
  }

  if(xDoubleHead(HUMAN, square) != -1)
  {
    duplicate(xsquare, square);
    makeMove(COMPUTER, xDoubleHead(HUMAN, xsquare), xsquare);
    if(xWon(HUMAN, xsquare) == -1)
    {
      cprintf("Computer Previous Depth : -3\n");
      return xDoubleHead(HUMAN, square);
    }
  }

  snake =xTwoSnake(COMPUTER, square);
  if( snake != -1)
  {
    duplicate(xsquare, square);
    makeMove(COMPUTER, snake, xsquare);
    if(xWon(HUMAN, xsquare) == -1)
    {
      cprintf("Computer Previous Depth : +4\n");
      return snake;
    }
  }
  if(xTwoSnake(HUMAN, square) != -1)
  {
    duplicate(xsquare, square);
    makeMove(COMPUTER, xTwoSnake(HUMAN, xsquare), xsquare);
    if(xWon(HUMAN, xsquare) == -1)
    {
      cprintf("Computer Previous Depth : -4\n");
      return xTwoSnake(HUMAN, square);
    }
  }

  genNumWin(n);
  sorting(n);

  for(i=0; i<X_BOARD; i++)
  {
    if( y_Base (n[i], square) != -1)
    {
      duplicate(xsquare, square);
      makeMove(COMPUTER, n[i], xsquare);
      if(xWon(HUMAN, xsquare) == -1)
      {
        cprintf("Computer Previous Depth : +5\n");
        return n[i];
      }
    }
  }

  for(i=0; i<X_BOARD; i++)
  {
    if( y_Base (move[i], square) != -1)
    {
      cprintf("Computer Previous Depth : +0\n");
      return move[i];
    }
  }
  return -1;
}

int max(int n[X_BOARD])
{
  int i, big;
  big = 0;
  for(i=0; i<X_BOARD; i++)
  {
    if(n[i]>big)
      big = n[i];
  }
  return big;
}

int status()
{
  if (isWon(COMPUTER, square))
    return WIN;
  else
    if (isWon(HUMAN, square))
      return LOSE;
    else
      if (isFull())
        return DRAW;
      else
        return OK;
}

int xDoubleHead(int who, int xsquare[Y_BOARD][X_BOARD])
{
  int x;
  int xxsquare[Y_BOARD][X_BOARD];
  for(x=0; x<X_BOARD; x++)
  {
    if(y_Base(x,xsquare) != -1)
    {
      duplicate(xxsquare, xsquare);
      makeMove(who, x, xxsquare);
      if(doubleHead(who, xxsquare))
        return x;
    }
  }
  return -1;
}

int xTwoSnake(int who, int xsquare[Y_BOARD][X_BOARD])
{
  int x, dx;
  int xxsquare[Y_BOARD][X_BOARD];
  for(x=0; x<X_BOARD; x++)
  {
    if(y_Base(move[x],xsquare) != -1)
    {
      duplicate(xxsquare, xsquare);
      makeMove(who, move[x], xxsquare);
      for(dx=0; dx<X_BOARD; dx++)
      {
        if( twoSnake(who, move[dx], xxsquare) && col[move[dx]] != who)
        {
          if(who == COMPUTER)
            col[move[dx]] = who;
          return move[x];
        }
      }
    }
  }
  return -1;
}

int xWon(int who, int xsquare[Y_BOARD][X_BOARD])
{
  int x;
  int xxsquare[Y_BOARD][X_BOARD];
  for(x=0; x<X_BOARD; x++)
  {
    if(y_Base(x,xsquare) != -1)
    {
      duplicate(xxsquare, xsquare);
      makeMove(who, x, xxsquare);
      if(isWon(who, xxsquare))
        return x;
    }
  }
  return -1;
}

int y_Base(int x, int xsquare[Y_BOARD][X_BOARD])
{
  int y;
  for(y=0; y<Y_BOARD; y++)
  {
    if(xsquare[y][x] == BASE)
      return y;
  }
  return -1; /*indicate no base at that x*/
}

void duplicate(int xSquare[Y_BOARD][X_BOARD], int oSquare[Y_BOARD][X_BOARD])
{
  int x,y;
  for(x=0; x<X_BOARD; x++)
  {
    for(y=0; y<Y_BOARD; y++)
    {
      xSquare[y][x] = oSquare[y][x];
    }
  }
}

void drawBoard()
{
  textcolor(0);
  textbackground(7);
  clrscr();
  gotoxy(1,1);
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("|     |     |     |     |     |     |     |     |\n");
  printf("+-----+-----+-----+-----+-----+-----+-----+-----+\n");
  printf("ARROW:move\tSPACE:select\tESC:exit\n");
  textcolor(1);
  gotoxy(44,20);
  cprintf("%-1s","4 In Line\n");
  gotoxy(44,21);
  cprintf("%-1s","ver Beta by Cheok Yan Cheng\n");
  gotoxy(44,22);
  cprintf("E-mail : yccheok@yahoo.com\n");
  gotoxy(44,23);
  cprintf("Web Site: www.geocities.com/yccheok\n");
  gotoxy(44,24);
  cprintf("Source code included!\n");
  gotoxy(1,22);
  printf("Human's Piece is O\n");
  gotoxy(1,23);
  printf("Computer's Piece is X\n");
  gotoxy(52,1);
  printf("Computer Move :\n");
  gotoxy(52,4);
  printf("Human Move :\n");
}

void drawPiece()
{
  int x,y;
  for(x=0; x<X_BOARD; x++)
  {
    for(y=0; y<Y_BOARD; y++)
    {
      if(square[y][x] == HUMAN)
      {
        gotoxy(x*6+4, y*2+2);
        textcolor(1);
        cprintf("O\n");
      }
      else
      if(square[y][x] == COMPUTER)
      {
        gotoxy(x*6+4, y*2+2);
        textcolor(4);
        cprintf("X\n");
      }
    }
  }
}

void genNumWin(int n[X_BOARD])
{
  int i, j, k;
  int xsquare[Y_BOARD][X_BOARD];
  int xxsquare[Y_BOARD][X_BOARD];

  for(i=0; i<X_BOARD; i++)
  {
    n[i]=0;
    if(y_Base(i, square) != -1)               /*has base exsit?*/
    {
      duplicate(xsquare, square);
      makeMove(COMPUTER, i, xsquare);

      for(j=0; j<X_BOARD; j++)
      {
        for(k=0; k<Y_BOARD; k++)
        {
          if(xsquare[k][j] == EMPTY || xsquare[k][j] == BASE)
          {
            duplicate(xxsquare, xsquare);
            xxsquare[k][j] = COMPUTER;
            if(isWon(COMPUTER, xxsquare))
              n[i]++;
          }
        }
      }
    }
  }
}

void getHumanMove()
{
  int x=3 ;
  int ch;
        while(TRUE)
  {
    gotoxy(x*6 +4, 2);
    ch = (int)getch();
    switch(ch)
    {
      case 75:/*LEFT*/
        if(x>0)
          x--;
        break;
      case 77:/*RIGHT*/
        if(x<(X_BOARD-1))
          x++;
        break;
      case 27:/*ESC*/
        textcolor(7);
        textbackground(0);
        clrscr();
        printf("Thank You For Playing 4 in line by Cheok Yan Cheng!\n");
        exit(0);
        break;
      case 32:/*SPACE*/
        if(humanMove(x))
        {
          drawPiece();
          return;
        }
        else
        {
          gotoxy(1,20);
          textcolor(4);
          cprintf("OOPs! Wrong Move! \n");
        }
    }
        }
}

void init()
{
  int x,y;
  for(x=0; x<X_BOARD; x++)
  {
    for(y=0; y<(Y_BOARD-1); y++)
    {
      square[y][x] = EMPTY;
    }
    square[7][x] = BASE;
    col[x] = -1;
  }
}

void makeMove(int who, int x, int xsquare[Y_BOARD][X_BOARD])
{
  int y;
  y = y_Base(x, xsquare);
  xsquare[y][x] = who;
  if(y>0)
    xsquare[y-1][x] = BASE;
}

void sorting(int n[])
{
  int i, j, alpha;
  int store[X_BOARD];

  for(j=0; j<X_BOARD; j++)
  {
    alpha = max(n);
    for(i=0; i<X_BOARD; i++)
    {
      if(n[move[i]] == alpha)
      {
        store[j] = move[i];
        n[move[i]] = -1;
        break;
      }
    }
  }
  for(i=0; i<X_BOARD; i++)
    n[i] = store[i];
}

int main(void)
{
  BOOLEAN myturn;
  myturn = TRUE;
  drawBoard();
  srand(time(NULL));
  rand();
  do
  {
    switch (status())
    {
      case WIN:
      case LOSE:
      case DRAW:
      init();
      drawBoard();
      if (myturn)
      {
        makeMove(COMPUTER,(2+rand()%4),square);
      }
      myturn = !myturn;
      drawPiece();
    }
    textcolor(4);
    gotoxy(1,20);
    cprintf("Your Turn, Please.\n");
    getHumanMove();
    gotoxy(1,20);
    textcolor(4);
    switch (status())
    {
      case WIN:
        cprintf(WIN_MESSAGE);
        getch();
      break;
      case LOSE:
        cprintf(LOSE_MESSAGE);
        getch();
      break;
      case DRAW:
        cprintf(DRAW_MESSAGE);
        getch();
        break;
      default:/*OK*/
        if(computerMove())
        {
          gotoxy(1,20);
          drawPiece();
          gotoxy(1,20);
          switch (status())
          {
            case WIN:
              cprintf(WIN_MESSAGE);
              getch();
            break;
            case LOSE:
              cprintf(LOSE_MESSAGE);
              getch();
            break;
            case DRAW:
              cprintf(DRAW_MESSAGE);
              getch();
            break;
          }
        }
    }
  }
  while(TRUE);
}

