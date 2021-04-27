#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

// fonction aide
void help(void) {
  printf("action: help\n");
  printf("press 't <i> <j>' to put a tent at square (i,j) \n");
  printf("press 'g <i> <j>' to put a grass at square (i,j) \n");
  printf("press 'e <i> <j>' to erase square (i,j) \n");
  printf("press 'z' to undo\n");
  printf("press 'y' to redo\n");
  printf("press 'r' to restart\n");
  printf("press 'q' to quit\n\n");
}

int main(int argc, char* argv[]) {
  game g;
  char c;
  uint i, j;
  int ret;

  g = game_load("test.txt");

  // if (argc == 2) {
  //   g = game_load(argv[1]);
  //   i = game_nb_rows(g);
  //   j = game_nb_cols(g);
  //   game_print(g);
  // } else {
  //   g = game_load("game_default.tnt");
  //   i = game_nb_rows(g);
  //   j = game_nb_cols(g);
  //   game_print(g);
  // }
  printf("? [h for help]\n");
  while (game_is_over(g) == false) {
    ret = scanf("%c", &c);
    if (ret == 1) {
      if (c == 'h') {
        printf("action: help\n");
        help();
        game_print(g);
        printf("? [h for help]\n");
      }
      if (c == 'r') {
        printf("action: restart\n");
        game_restart(g);
        game_print(g);
        printf("? [h for help]\n");
      }
      if (c == 'q') {
        printf("action: quit\n");
        printf("shame\n");
        game_delete(g);
        return EXIT_SUCCESS;
      }
      if (c == 'z') {
        printf("action: Undo\n");
        game_undo(g);
        game_print(g);
        printf("? [h for help]\n");
      }
      if (c == 'y') {
        printf("action: redo\n");
        game_redo(g);
        game_print(g);
        printf("? [h for help]\n");
      }
      if (c == 't' || c == 'e' || c == 'g') {
        ret = scanf(" %u %u", &i, &j);
        if (ret == 2) {
          if (c == 't' && i < DEFAULT_SIZE && j < DEFAULT_SIZE) {
            printf("action: play move 't' into square (%d,%d)\n", i, j);
            if (game_check_move(g, i, j, TENT) == LOSING)
              printf("Warning: losing move on square(%d,%d)!\n", i, j);
            if (game_check_move(g, i, j, TENT) == ILLEGAL) {
              printf("Warning: illegal move on square(%d,%d)!\n", i, j);
            } else {
              game_play_move(g, i, j, 2);
            }
          }
          if (c == 'e' && i < DEFAULT_SIZE && j < DEFAULT_SIZE) {
            printf("action: play move 'e' into square (%d,%d)\n", i, j);
            if (game_check_move(g, i, j, TENT) == ILLEGAL) {
              printf("Warning: illegal move on square(%d,%d)!\n", i, j);
            } else {
              game_play_move(g, i, j, 0);
            }
          }
          if (c == 'g' && i < DEFAULT_SIZE && j < DEFAULT_SIZE) {
            printf("action: play move 'g' into square (%d,%d)\n", i, j);
            if (game_check_move(g, i, j, GRASS) == LOSING)
              printf("Warning: losing move on square(%d,%d)!\n", i, j);
            if (game_check_move(g, i, j, TENT) == ILLEGAL) {
              printf("Warning: illegal move on square(%d,%d)!\n", i, j);
            } else {
              game_play_move(g, i, j, 3);
            }
          }
        }
        game_print(g);
        printf("? [h for help]\n");
      }
    }
  }
  game_print(g);
  printf("congratulation\n");
  game_delete(g);
  return EXIT_SUCCESS;
}