#include "game_aux.h"
#include <stdio.h>
#include <stdlib.h>
#include "game_ext.h"

uint t_row[DEFAULT_SIZE] = {3, 0, 4, 0,
                            4, 0, 1, 0};  // initialisation de la ligne
uint t_col[DEFAULT_SIZE] = {4, 0, 1, 2,
                            1, 1, 2, 1};  // initialisation de la colonne

game game_default(void) {
  square squares[] = {// Initialisation d'un tableau de jeu
                      EMPTY, EMPTY, EMPTY, EMPTY, TREE,  TREE,  EMPTY, EMPTY,
                      TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, TREE,
                      EMPTY, EMPTY, EMPTY, EMPTY, TREE,  EMPTY, EMPTY, EMPTY,
                      TREE,  EMPTY, EMPTY, EMPTY, EMPTY, TREE,  EMPTY, EMPTY,
                      EMPTY, TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                      TREE,  EMPTY, EMPTY, EMPTY, TREE,  EMPTY, TREE,  EMPTY,
                      EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
                      TREE,  EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY};
  return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, squares, t_row, t_col, false,
                      false);
}

game game_default_solution(void) {
  // initialisation de la grille
  square t_square[64] = {TENT, GRASS, GRASS, TENT,  TREE,  TREE,  TENT,  GRASS,
                         TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, TREE,
                         TENT, GRASS, GRASS, TENT,  TREE,  TENT,  GRASS, TENT,
                         TREE, GRASS, GRASS, GRASS, GRASS, TREE,  GRASS, GRASS,
                         TENT, TREE,  TENT,  GRASS, TENT,  GRASS, TENT,  GRASS,
                         TREE, GRASS, GRASS, GRASS, TREE,  GRASS, TREE,  GRASS,
                         TENT, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS,
                         TREE, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS, GRASS};
  return game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, t_square, t_row, t_col, false,
                      false);
}

void game_print(cgame g) {
  printf("   ");
  for (uint i = 0; i < game_nb_cols(g); i++) {
    printf("%u", i);
  }
  printf("\n");
  printf("   ");
  for (uint i = 0; i < game_nb_cols(g); i++) {
    printf("-");
  }

  for (unsigned int i = 0; i < game_nb_rows(g); i++) {
    printf("\n%u |", i);
    for (unsigned int j = 0; j < game_nb_cols(g); j++) {
      square v = game_get_square(g, i, j);

      if (v == TREE) {
        printf("x");
      }
      if (v == EMPTY) {
        printf(" ");
      }
      if (v == GRASS) {
        printf("-");
      }
      if (v == TENT) {
        printf("*");
      }
    }
    printf("| %u", game_get_expected_nb_tents_row(g, i));
  }
  printf("\n");
  printf("   ");
  for (uint i = 0; i < game_nb_cols(g); i++) {
    printf("-");
  }
  printf("\n");

  printf("   ");
  for (uint i = 0; i < game_nb_cols(g); i++) {
    printf("%u", game_get_expected_nb_tents_col(g, i));
  }
  printf("\n");

  return;
}
