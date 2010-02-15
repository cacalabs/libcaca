/*
 *  conio-snake   snake game using the <conio.h> API
 *  Copyright (c) 2003-2004 Simon Huggins <webmaster#simonhuggins.com>
 *                2009-2010 Sam Hocevar <sam@hocevar.net>
 *                All Rights Reserved
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <caca_conio.h>

/* prototypes */
void draw_line(int col, int row);
void show_score();
void add_segment();
void setup_level();

/* constants */
const int maxrow=15, maxcol=77;
const int snake_start_col=33,snake_start_row=7;
const char up_key='a', down_key='z', left_key='o', right_key='p';
const int pause_length=500000;

/* global variables */
int score, snake_length, speed, obstacles, level, firstpress, high_score=0;
char screen_grid[maxrow][maxcol];
char direction = right_key;
struct snake_segment {
  int row,col;
} snake[100];

int main()
{

  /* Variable declarations within main() only */
  char keypress=0;

  do /* restart game loop */
  {
    obstacles=4; level=1; score=0; speed=14;

    //randomize(); /* Ensure random seed initiated */

    setup_level();

    /* main loop */
    do
    {
      delay(speed * pause_length / 50000); /*pause*/

      /* If key has been hit, then check it is a direction key - if so,
         change direction */
      if (kbhit())
      {
        keypress=(char)getch();
        if((keypress==right_key)||(keypress==left_key)||
           (keypress==up_key)||(keypress==down_key))
          direction = keypress;
      }

      /* Add a segment to the end of the snake */
      add_segment();

      /* Blank last segment of snake */
      gotoxy(snake[0].col,snake[0].row);
      cprintf(" ");
      /* ... and remove it from the array */
        for (int i=1;i<=snake_length;i++)
          snake[i-1]=snake[i];
      /* Display snake in yellow */
      textcolor(YELLOW);
      for (int i=0;i<=snake_length;i++)
      {
        gotoxy(snake[i].col,snake[i].row);
        cprintf("O");
      }
      /* keeps cursor flashing in one place instead of following snake */
      gotoxy(1,1);

      /* If first press on each level, pause until a key is pressed */
      if (firstpress) { while(!kbhit()); firstpress = 0; }

      /* Collision detection - walls (bad!) */
      if ((snake[snake_length-1].row>maxrow+1)||(snake[snake_length-1].row<=1)||
          (snake[snake_length-1].col>maxcol+1)||(snake[snake_length-1].col<=1)||
      /* Collision detection - obstacles (bad!) */
          (screen_grid[snake[snake_length-1].row-2][snake[snake_length-1].col-2]=='x'))
        keypress='x'; /* i.e. exit loop - game over */
      /* Collision detection - snake (bad!) */
      for (int i=0;i<snake_length-1;i++)
        if ( (snake[snake_length-1].row)==(snake[i].row) &&
             (snake[snake_length-1].col)==(snake[i].col))
        {
          keypress='x'; /* i.e. exit loop - game over */
          break; /* no need to check any more segments */
        }
      /* Collision detection - food (good!) */
      if (screen_grid[snake[snake_length-1].row-2][snake[snake_length-1].col-2]=='.')
      {
        /* increase score and length of snake */
        score+=snake_length*obstacles; show_score(); snake_length++; add_segment();
        /* if length of snake reaches certain size, onto next level */
        if (snake_length==(level+3)*2)
        {
          score+=level*1000; obstacles+=2; level++;  /* add to obstacles */
          if ((level%5==0)&&(speed>1)) speed--; /* increase speed every 5 levels */
          setup_level(); /* display next level */
        }
      }
    } while (keypress!='x');

    /* game over message */
    if (score > high_score) high_score = score;
    show_score();
    gotoxy(30,6); textcolor(LIGHTRED); cprintf("G A M E   O V E R");
    gotoxy(30,9); textcolor(YELLOW); cprintf("Another Game (y/n)? ");
    do keypress=getch(); while((keypress!='y')&&(keypress!='n'));

  } while (keypress=='y');
}

void setup_level()
{

 /* variables local to setup_level() */
 int row,col;

  /* Set up global variables for new level */
  snake_length=level+4; direction = right_key;
  firstpress = 1;
 /* Fill grid with blanks */
  for(row=0;row<maxrow;row++)
    for(col=0;col<maxcol;col++)
     screen_grid[row][col]= ' ';

  /* Fill grid with Xs and food */
  for(int i=0;i<obstacles*2;i++)
  {
   row= rand()%maxrow;
   col= rand()%maxcol;
   if(i<obstacles)
     screen_grid[row][col]='x';
   else
     screen_grid[row][col]='.';
  }

  /* Create snake array of length snake_length */
  for(int i=0;i<snake_length;i++)
  {
    snake[i].row=snake_start_row;
    snake[i].col=snake_start_col+i;
  }

  /* Draw playing board */
  draw_line(1,1);
  for(row=0;row<maxrow;row++)
  {
    gotoxy(1,row+2);
    textcolor(LIGHTBLUE); cprintf("|");
    textcolor(WHITE);
    for(col=0;col<maxcol;col++)
      cprintf("%c",screen_grid[row][col]);
    textcolor(LIGHTBLUE);
    cprintf("|");
  }
  draw_line(1,maxrow+2);

  show_score();
  gotoxy(2,maxrow+5);
  textcolor(LIGHTRED);
  cprintf("~~ SNAKE GAME~~ Left: %c, Right: %c, Up: %c, Down: %c, Exit: x. Any key to start.",
          left_key,right_key,up_key,down_key);
}

void draw_line(int col, int row)
{
  gotoxy(col,row); textcolor(LIGHTBLUE);
  for (col=0;col<maxcol+2;col++) cprintf("=");
}

void show_score()
{
  textcolor(LIGHTCYAN);
  gotoxy(2,maxrow+3);
  cprintf("Level: %05d",level);
  gotoxy(40,maxrow+3);
  textcolor(LIGHTGREEN);
  cprintf("Score: %05d",score);
  gotoxy(60,maxrow+3);
  textcolor(LIGHTMAGENTA);
  cprintf("High Score: %05d",high_score);
}
void add_segment()
{
  switch(direction)
  {
    case(right_key): snake[snake_length].row=snake[snake_length-1].row;
                     snake[snake_length].col=snake[snake_length-1].col+1;
                     break;
    case(left_key) : snake[snake_length].row=snake[snake_length-1].row;
                     snake[snake_length].col=snake[snake_length-1].col-1;
                     break;
    case(up_key)   : snake[snake_length].row=snake[snake_length-1].row-1;
                     snake[snake_length].col=snake[snake_length-1].col;
                     break;
    case(down_key) : snake[snake_length].row=snake[snake_length-1].row+1;
                     snake[snake_length].col=snake[snake_length-1].col;
  }
}

