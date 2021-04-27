#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_tools.h"
#include "queue.h"

void write_nb_solution(uint s, char* filename) {
  if (filename == NULL) {
    exit(EXIT_FAILURE);
  }
  FILE* f = fopen(filename, "w");
  if (f == NULL) {
    exit(EXIT_FAILURE);
  }
  fprintf(f, "%u \n", s);
  fclose(f);
}

int main(int argc, char* argv[]) {
  game g = game_load(argv[2]);

  if (argc >= 3 && strcmp(argv[1], "-s") == 0) {
    bool v = game_solve(g);
    if (v == false) {
      exit(EXIT_FAILURE);
    } else if (argc == 4) {
      game_save(g, argv[3]);
    } else {
      game_save(g, "solution.tnt");
    }
  }

  else if (argc >= 3 && strcmp(argv[1], "-c") == 0) {
    if (argc == 4) {
      write_nb_solution(game_nb_solutions(g), argv[3]);
    } else {
      write_nb_solution(game_nb_solutions(g), "nb_solution.tnt");
    }
  }
  game_delete(g);
  return 0;
}
