#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"

#include "game_private.h"

/* ********************* FONCTION GAME LOAD **************************** */
game game_load(char* filename) {
  if (filename == NULL) {
    exit(EXIT_FAILURE);
  }
  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    exit(EXIT_FAILURE);
  }

  uint nb_row, nb_col;
  int w, d;
  fscanf(f, "%u %u %d %d", &nb_row, &nb_col, &w, &d);

  fseek(f, +1, SEEK_CUR);

  square squares[nb_row * nb_col];
  uint row_exp[nb_row];
  uint col_exp[nb_col];
  for (uint k = 0; k < nb_row; k++) {
    fscanf(f, "%u", &row_exp[k]);
  }

  for (uint k = 0; k < nb_col; k++) {
    fscanf(f, "%u", &col_exp[k]);
  }

  fseek(f, +2, SEEK_CUR);
  char v;

  for (uint k = 0; k < nb_row; k++) {
    for (uint j = 0; j < nb_col; j++) {
      fscanf(f, "%c", &v);
      // printf("%c",v);
      squares[k * nb_col + j] = _str2square(v);
    }
    fseek(f, +1, SEEK_CUR);
  }

  return game_new_ext(nb_row, nb_col, squares, row_exp, col_exp, w, d);
}

/* ********************* FONCTION GAME SAVE **************************** */

/*
<nb_rows> <nb_cols> <is_swap> <is_diagadj>\n
<nb_tents_row[0]> <nb_tents_row[1]> ... <nb_tents_row[nb_rows-1]> \n
<nb_tents_col[0]> <nb_tents_col[1]> ... <nb_tents_col[nb_cols-1]> \n
<squares[0][0]><squares[0][1]>...<squares[0][nb_cols-1]>\n
<squares[1][0]><squares[1][1]>...<squares[1][nb_cols-1]>\n
...
<squares[nb_rows-1][0]><squares[nb_rows-1][1]>...<squares[nb_rows-1][nb_cols-1]>\n
*/

void game_save(cgame g, char* filename) {
  if (filename == NULL || g == NULL) {
    exit(EXIT_FAILURE);
  }
  FILE* f = fopen(filename, "w");
  if (f == NULL) {
    exit(EXIT_FAILURE);
  }

  fprintf(f, "%u %u ", game_nb_rows(g), game_nb_cols(g));
  if (game_is_wrapping(g)) {
    fprintf(f, "1 ");
  } else {
    fprintf(f, "0 ");
  }
  if (game_is_diagadj(g)) {
    fprintf(f, "1\n");
  } else {
    fprintf(f, "0\n");
  }

  for (uint r = 0; r < game_nb_rows(g); r++) {
    fprintf(f, "%u ", g->nb_tents_row[r]);
  }
  fprintf(f, "\n");

  for (uint c = 0; c < game_nb_cols(g); c++) {
    fprintf(f, "%u ", g->nb_tents_col[c]);
  }
  fprintf(f, "\n");

  for (uint i = 0; i < game_nb_rows(g); i++) {
    for (uint j = 0; j < game_nb_cols(g); j++) {
      square s = game_get_square(g, i, j);
      char c = _square2str(s);
      fprintf(f, "%c", c);
    }
    fprintf(f, "\n");
  }
  fclose(f);
}

/************ Arranger l executable ./game_solve <option> <input> [<output>]
**********

<option> :  -s (game_solve retourne premiere solution trouvée OU EXIT_FAILURE)
            -c (game_nb_solutions retourne le nb de solutions possibles (avec un
retour a la ligne apres)) <input> = game.tnt <output> (pas obligatoire) =
fichier de réponse
*/

// renvoie la pos du nth square dans le tableau pos[2]
void game_get_pos_nth_with(game g, uint n, square s, uint* pos) {
  for (uint k = 0; k < (g->nb_rows * g->nb_cols); k++) {
    if (g->squares[k] == s) {
      if (n == 1) {
        pos[1] = k % g->nb_cols;
        pos[0] = k / g->nb_cols;
        return;
      }
      n = n - 1;
    }
  }
}

// rempli le game g de grass là ou aucune tent ne peut etre plantée (en fonction
// de wrapping) et place celles dont la position est sure
void game_fill_min(game g) {
  for (uint i = 0; i < (g->nb_rows); i++) {
    for (uint j = 0; j < (g->nb_cols); j++) {
      if ((g->squares[i * (g->nb_cols) + j] != TREE) &&
          !((g->squares[i * (g->nb_cols) + j] == EMPTY) &&
            _neigh(g, i, j, TREE, g->diagadj))) {
        g->squares[i * (g->nb_cols) + j] = GRASS;
      }
    }
  }
  for (uint i = 0; i < g->nb_rows; i++) {
    if (g->nb_tents_row[i] == 0) {
      game_fill_grass_row(g, i);
    }
  }
  for (uint i = 0; i < g->nb_cols; i++) {
    if (g->nb_tents_col[i] == 0) {
      game_fill_grass_col(g, i);
    }
  }
  for (uint i = 0; i < g->nb_rows; i++) {
    if (game_get_expected_nb_tents_row(g, i) -
            (game_get_current_nb_tents_row(g, i) +
             _nb_squares_row(g, i, EMPTY)) ==
        0) {
      for (uint j = 0; j < g->nb_cols; j++) {
        if (SQUARE(g, i, j) == EMPTY) game_play_move(g, i, j, TENT);
      }
    }
  }
  for (uint i = 0; i < g->nb_cols; i++) {
    if (game_get_expected_nb_tents_col(g, i) -
            (game_get_current_nb_tents_col(g, i) +
             _nb_squares_col(g, i, EMPTY)) ==
        0) {
      for (uint j = 0; j < g->nb_rows; j++) {
        if (SQUARE(g, j, i) == EMPTY) game_play_move(g, j, i, TENT);
      }
    }
  }
}

// retourne le nb de cases du game g contenant la valeur s
uint game_nb_all(cgame g, square s) {
  assert(g);
  int cpt_nb_all = 0;
  for (uint i = 0; i < g->nb_rows; i++)
    for (uint j = 0; j < g->nb_cols; j++)
      if (SQUARE(g, i, j) == s) cpt_nb_all++;
  return cpt_nb_all;
}
// rempli la nieme case libre (Empty) par l'element s
void fill_nth_with(game g, uint n, square s) {
  for (uint i = 0; i < (g->nb_rows * g->nb_cols); i++) {
    if (g->squares[i] == EMPTY) {
      if (n == 1) {
        g->squares[i] = s;
        return;
      }
      n = n - 1;
    }
  }
  exit(EXIT_FAILURE);
}

uint n;
// undo jusqu'a undo la derniere square s et y joue une tente si possible sinon
// recommence
void undo_to_last(game g, square s) {
  if (game_get_current_nb_tents_all(g) < 1) {
    n = 0;
    return;
  }
  move* m = queue_peek_head(g->undo_stack);
  while (m->new != TENT && queue_length(g->undo_stack) > 1) {
    game_undo(g);
    m = queue_peek_head(g->undo_stack);
  }
  game_undo(g);

  if (queue_length(g->undo_stack) == 0) {
    n = n - 1;
  }
  uint pos[] = {0, 0};
  game_get_pos_nth_with(g, 1, EMPTY, pos);

  if (game_check_move(g, pos[0], pos[1], GRASS) == REGULAR) {
    game_play_move(g, pos[0], pos[1], GRASS);
  } else {
    undo_to_last(g, TENT);
  }
  return;
}

// fonction auxiliaire pour solve et nb_solutions
uint aux_solve_solutions(game g, char o) {
  game_fill_min(g);
  uint ec = game_nb_all(g, EMPTY);
  uint pos[] = {0, 0};
  uint cpt_sol = 0;
  n = 1 + ec -
      (game_get_expected_nb_tents_all(g) - game_get_current_nb_tents_all(g));

  while (n > 0) {
    if (game_is_over(g)) {
      if (o == 's') {
        return 1;
      }
      cpt_sol = cpt_sol + 1;
      undo_to_last(g, TENT);

      while (game_get_current_nb_tents_all(g) + game_nb_all(g, EMPTY) <
             game_get_expected_nb_tents_all(g)) {
        undo_to_last(g, TENT);
      }
    }

    game_get_pos_nth_with(g, 1, EMPTY, pos);

    if (game_check_move(g, pos[0], pos[1], TENT) != LOSING) {
      game_play_move(g, pos[0], pos[1], TENT);
    } else if (game_check_move(g, pos[0], pos[1], GRASS) != LOSING) {
      game_play_move(g, pos[0], pos[1], GRASS);
    } else {
      undo_to_last(g, TENT);
    }
  }
  if (o == 's') {
    return 0;
  }
  if (g->nb_cols != g->nb_rows) {
    cpt_sol = cpt_sol - 1;
  }
  return cpt_sol;
}

// LES DEUX FONCTIONS DU .H
bool game_solve(game g) {
  if (aux_solve_solutions(g, 's') == 1) {
    return true;
  }
  return false;
}
uint game_nb_solutions(game g) {
  assert(g);
  return (aux_solve_solutions(g, 'c'));
}