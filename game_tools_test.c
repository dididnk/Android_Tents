#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

#include "game_private.h"

/* ********************* FONCTION USAGE ************** */
void usage(int argc, char* argv[]) {
  fprintf(stderr, "Usage: %s <testname> [<...>]\n", argv[0]);
  exit(EXIT_FAILURE);
}

/* ********************* FONCTION TESTE DE GAME LOAD ************** */
bool game_load_test(void) {
  game_save(game_default_solution(), "test.tnt");
  game g = game_load("test.tnt");
  return game_equal(game_default_solution(), g);
}

/* ********************* FONCTION TESTE DE GAME SAVE ************** */
bool game_save_test(void) { return true; }
/*
 ********************* FONCTION TESTE DE GAME SOLVE **************
bool game_solve_test()
{
    game g = game_default();
    if(!game_solve(g) && !game_is_over(g))
        return false;
    return true;
}

 ********************* FONCTION TESTE DE GAME NB SOLUTIONS **************
bool game_nb_solutions_test()
{
    game g = game_default();
    assert(g);
    int nb_sol = game_nb_solutions(g);
    if (nb_sol != 1)    return false;
    return true;
}
*/

/* ********************* FONCTION PRINCIPALE MAIN ************** */
int main(int argc, char* argv[]) {
  if (argc == 1 || argc >= 3) {
    usage(argc, argv);
  }
  fprintf(stderr, "=> Start test \"%s\"\n", argv[1]);
  bool test = false;
  if (strcmp("game_load_test", argv[1]) == 0) {
    test = game_load_test();
  } else if (strcmp("game_save_test", argv[1]) == 0) {
    test = game_save_test();
  } /* else if (strcmp("game_solve_test", argv[1]) == 0) {
      test = game_solve_test();
  } else if (strcmp("game_nb_solutions_test", argv[1]) == 0) {
      test = game_nb_solutions_test();
  }*/
  else {
    fprintf(stderr, "Error: test %s not found!\n", argv[1]);
    exit(EXIT_FAILURE);
  }
  if (test) {
    fprintf(stderr, "Test \"%s\" finished: SUCCESS\n", argv[1]);
    return EXIT_SUCCESS;
  } else {
    fprintf(stderr, "Test \"%s\" finished: FAILURE\n", argv[1]);
    return EXIT_FAILURE;
  }
}