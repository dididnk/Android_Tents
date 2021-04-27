#include <stdbool.h>
#include "game.h"

enum {
  UP= 0, /**< square UP */
  LEFT = 1,  /**< square left */
  RIGHT = 2,  /**< square right */
  LOW = 3 /**< square low */
};

bool high_diag_square(cgame g, uint i, uint j);
bool low_diag_square(cgame g, uint i, uint j);
int* squares_around_of_square(cgame g, int* t, uint i, uint j, square s, square s1,int indice);
bool square_adjacent(int* tab);
bool tent_adja_Or_not_tree_around_tent(cgame g, uint i, uint j);


