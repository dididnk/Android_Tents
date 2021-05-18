#include <SDL.h>
#include <SDL_image.h>  // required to load transparent texture from PNG
#include <SDL_ttf.h>    // required to use TTF fonts

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_tools.h"
#include "model.h"

/* **************************************************************** */

#define FONT "arial.ttf"
#define FONTSIZE 36
#define TENT_PNG "tent.png"
#define TREE_PNG "tree.png"
#define GRASS_PNG "grass.png"
#define LOSINGMOVE_PNG "redtent.png"


/* **************************************************************** */

/* PUT YOUR VARIABLES HERE */
struct Env_t {
  /* PUT YOUR VARIABLES HERE */
  SDL_Texture **tree;
  SDL_Texture **grass;
  SDL_Texture **tent;
  SDL_Texture **losing_move;
  SDL_Texture *losing_message;
  SDL_Texture *BRAVOOO;
  SDL_Texture **rows;
  SDL_Texture **cols;
  SDL_Texture **expected_rows;
  SDL_Texture **expected_cols;
  game stock;
  
};

/* **************************************************************** */

/* PUT YOUR CODE HERE TO INIT TEXTURES, ... */
Env *init(SDL_Window *win, SDL_Renderer *ren, int argc, char *argv[]) {
  Env *env = malloc(sizeof(struct Env_t));

  PRINT("*************GAME TENT*************\n");
  PRINT("Use left click to place a tent.\n");
  PRINT("Use right click to place a grass.\n");
  PRINT("Click 'r' to restart the game.\n");
  PRINT("Press 'z' to undo a move, 'y' to redo it.\n");
  PRINT("Press 's' to solve the game \n");

  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  if (argc == 2) {
    env->stock = game_load(argv[1]);
  } else {
    env->stock = game_load("game_default.tnt");
  }

  // allocation dynamique avec malloc 
  env->rows = malloc(game_nb_rows(env->stock) * sizeof(SDL_Texture *));
  env->cols = malloc(game_nb_cols(env->stock) * sizeof(SDL_Texture *));
  env->expected_rows = malloc(game_nb_rows(env->stock) * sizeof(SDL_Texture *));
  env->expected_cols = malloc(game_nb_cols(env->stock) * sizeof(SDL_Texture *));

  env->tent = malloc(game_nb_cols(env->stock) * game_nb_rows(env->stock) *
                      sizeof(SDL_Texture *));
  env->losing_move = malloc(game_nb_cols(env->stock) * game_nb_rows(env->stock) *
                       sizeof(SDL_Texture *));
  env->tree = malloc(game_nb_cols(env->stock) * game_nb_rows(env->stock) *
                      sizeof(SDL_Texture *));
  env->grass = malloc(game_nb_cols(env->stock) * game_nb_rows(env->stock) *
                        sizeof(SDL_Texture *));

  if (env->rows == NULL || env->cols == NULL || env->expected_rows == NULL ||
      env->expected_cols == NULL || env->tent == NULL || env->tree == NULL ||
      env->grass == NULL) {
    fprintf(stderr, "Error in allocation \n");
    exit(EXIT_FAILURE);
  }

  SDL_Surface *tent_surface = IMG_Load(TENT_PNG);
  if (!tent_surface) ERROR("IMG_LoadTexture: %s\n", TENT_PNG);

  SDL_Surface *background_surface = IMG_Load(LOSINGMOVE_PNG);
  if (!background_surface) ERROR("IMG_LoadTexture: %s\n", LOSINGMOVE_PNG);

  SDL_Surface *tree_surface = IMG_Load(TREE_PNG);
  if (!tree_surface) ERROR("IMG_LoadTexture: %s\n", TREE_PNG);

  SDL_Surface *grass_surface = IMG_Load(GRASS_PNG);
  if (!grass_surface) ERROR("IMG_LoadTexture: %s\n", GRASS_PNG);

  SDL_Surface *losingtent_surface = IMG_Load(LOSINGMOVE_PNG);
  if (!losingtent_surface) ERROR("IMG_LoadTexture: %s\n", LOSINGMOVE_PNG);
  

  SDL_Texture *tent_texture = SDL_CreateTextureFromSurface(ren, tent_surface);
  SDL_Texture *tentr_texture = SDL_CreateTextureFromSurface(ren, background_surface);
  SDL_Texture *tree_texture = SDL_CreateTextureFromSurface(ren, tree_surface);
  SDL_Texture *grass_texture = SDL_CreateTextureFromSurface(ren, grass_surface);

  for (int i = 0; i < game_nb_rows(env->stock); i++) {
    for (int j = 0; j < game_nb_cols(env->stock); j++) {
      env->losing_move[i * game_nb_cols(env->stock) + j] = tentr_texture;
      env->tent[i * game_nb_cols(env->stock) + j] = tent_texture;
      env->tree[i * game_nb_cols(env->stock) + j] = tree_texture;
      env->grass[i * game_nb_cols(env->stock) + j] = grass_texture;
    }
  }

  /* initiation du texte */
  SDL_Color color = {0, 0, 0, 255}; 
  TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font,
                   TTF_STYLE_ITALIC);  

  SDL_Color color_losing = {255, 0, 0};
  SDL_Surface *surf = TTF_RenderText_Blended(
      font, "Losing move !", color_losing);  
  env->losing_message = SDL_CreateTextureFromSurface(ren, surf);

  
  SDL_Color RED = {255, 0, 0};
  TTF_Font *font2 = TTF_OpenFont(FONT, 30);
  SDL_Surface *surf2 =
      TTF_RenderText_Blended(font2, "BRAVOOO !!!", RED);
  env->BRAVOOO = SDL_CreateTextureFromSurface(ren, surf2);
  SDL_FreeSurface(surf2);

  for (int i = 0; i < game_nb_rows(env->stock); i++) {
    int c = i + '0';
    char str[10];
    sprintf(str, "%c", c);
    int i_copy = i;
    int cpt = 0;
    if (i_copy > 9) {
      int cpt_dizaine = 1;
      while (i_copy > 9) {
        cpt = cpt_dizaine + '0';
        i_copy -= 10;
      }
      c = i_copy + '0';
      sprintf(str, "%c", c);
      str[1] = str[0];
      str[0] = cpt;
      str[2] = ' ';
    }
    SDL_Surface *surf = TTF_RenderText_Blended(font, str, color);
    env->rows[i] = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);

    
    int c_exp = game_get_expected_nb_tents_row(env->stock, i) + '0';
    char str_exp[10];
    sprintf(str_exp, "%c", c_exp);
    SDL_Surface *surf_exp = TTF_RenderText_Blended(font, str_exp, color);
    env->expected_rows[i] = SDL_CreateTextureFromSurface(ren, surf_exp);
    SDL_FreeSurface(surf_exp);

    for (int j = 0; j < game_nb_cols(env->stock); j++) {
      int c = j + '0';
      char str[10];
      sprintf(str, "%c", c);
      int j_copy = j;
      int cpt = 0;
      if (j_copy > 9) {
        int cpt_dizaine = 1;
        while (j_copy > 9) {
          cpt = cpt_dizaine + '0';
          j_copy -= 10;
        }
        c = j_copy + '0';
        sprintf(str, "%c", c);
        str[1] = str[0];
        str[0] = cpt;
        str[2] = ' ';
      }
      SDL_Surface *surf = TTF_RenderText_Blended(font, str, color);
      env->cols[j] = SDL_CreateTextureFromSurface(ren, surf);
      SDL_FreeSurface(surf);

      int c_exp = game_get_expected_nb_tents_col(env->stock, j) + '0';
      char str_exp[10];
      sprintf(str_exp, "%c", c_exp);

      SDL_Surface *surf_exp = TTF_RenderText_Blended(font, str_exp, color);
      env->expected_cols[j] = SDL_CreateTextureFromSurface(ren, surf_exp);
      SDL_FreeSurface(surf_exp);
    }
  }
  TTF_CloseFont(font);
  TTF_CloseFont(font2);

  return env;
}

/* **************************************************************** */

/* PUT YOUR CODE HERE TO RENDER TEXTURES, ... */
void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  SDL_Rect rect_row;
  SDL_Rect rect_col;
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  for (int i = 0; i < game_nb_rows(env->stock); i++) {
    for (int j = 0; j < game_nb_cols(env->stock); j++) {
      square s = game_get_square(env->stock, i, j);

      rect_row.x = 100 + (w - 400) / game_nb_cols(env->stock) * j;
      rect_row.y = 100 + (h - 200) / game_nb_rows(env->stock) * i;
      rect_row.w = (w - 400) / game_nb_rows(env->stock);
      rect_row.h = (h - 200) / game_nb_cols(env->stock);
      if (s == TREE) {
        SDL_RenderCopy(ren, env->tree[i * game_nb_cols(env->stock) + j], NULL,
                       &rect_row);
      }
      if (s == GRASS) {
        SDL_RenderCopy(ren, env->grass[i * game_nb_cols(env->stock) + j],
                       NULL, &rect_row);
      }

      if (s == TENT) {
        if (game_check_move(env->stock, i, j, TENT) == LOSING) {
          SDL_RenderCopy(ren, env->losing_move[i * game_nb_cols(env->stock) + j],
                         NULL, &rect_row);
        } else if (game_check_move(env->stock, i, j, TENT) != LOSING)
          SDL_RenderCopy(ren, env->tent[i * game_nb_cols(env->stock) + j],
                         NULL, &rect_row);
      }
    }
  }

  for (int i = 0; i < game_nb_rows(env->stock); i++) {

    SDL_QueryTexture(env->rows[i], NULL, NULL, &rect_row.w, &rect_row.h);
    rect_row.x = 50;
    rect_row.y = 100 + (h - 200) / game_nb_rows(env->stock) * i;
    SDL_RenderCopy(ren, env->rows[i], NULL, &rect_row);

    // Expected tent
    SDL_QueryTexture(env->expected_rows[i], NULL, NULL, &rect_row.w,
                     &rect_row.h);
    rect_row.x = w - 280;
    rect_row.y = 100 + (h - 200) / game_nb_rows(env->stock) * i;
    SDL_RenderCopy(ren, env->expected_rows[i], NULL, &rect_row);
  }
  for (int j = 0; j < game_nb_cols(env->stock); j++) {
    SDL_QueryTexture(env->cols[j], NULL, NULL, &rect_col.w, &rect_col.h);
    rect_col.x = 110 + (w - 400) / game_nb_cols(env->stock) * j;
    rect_col.y = 50;
    SDL_RenderCopy(ren, env->cols[j], NULL, &rect_col);

    // Expected tent
    SDL_QueryTexture(env->expected_cols[j], NULL, NULL, &rect_col.w,
                     &rect_col.h);
    rect_col.x = 100 + (w - 400) / game_nb_cols(env->stock) * j;
    rect_col.y = h - 100;
    SDL_RenderCopy(ren, env->expected_cols[j], NULL, &rect_col);
  }

  // Dessiner la grille avec DrawLine
  SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
  for (int i = 0; i <= game_nb_rows(env->stock); i++) {
    int begX = 100;
    int begY = 100 + (h - 200) / game_nb_rows(env->stock) * i;
    int endX = w - 300;
    SDL_RenderDrawLine(ren, begX, begY, endX, begY);
  }
  for (int j = 0; j <= game_nb_cols(env->stock); j++) {
    int begX = 100 + (w - 400) / game_nb_cols(env->stock) * j;
    int begY = 100;
    int endY = h-100;
    SDL_RenderDrawLine(ren, begX, begY, begX, endY);
  }


  if (game_is_over(env->stock)) {
    SDL_Rect r;
    SDL_QueryTexture(env->BRAVOOO, NULL, NULL, &r.w, &r.h);
    r.x = w - 280;
    r.y = h / 30;
    SDL_RenderCopy(ren, env->BRAVOOO, NULL, &r);
  }
}

/* **************************************************************** */

/* PUT YOUR CODE HERE TO PROCESS EVENTS */
bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  int w, h;
  SDL_GetWindowSize(win, &w, &h);

  if (e->type == SDL_QUIT) {
    return true;
  } else if (e->type == SDL_MOUSEBUTTONDOWN) {
    SDL_Point mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    for (int i = 0; i < game_nb_rows(env->stock); i++) {
      for (int j = 0; j < game_nb_cols(env->stock); j++) {
        int begX = 100 + (w - 400) / game_nb_cols(env->stock) * j;
        int endX;
        if (j == game_nb_cols(env->stock) - 1)
          endX = w - 300;
        else
          endX = 100 + (w - 400) / game_nb_cols(env->stock) * (j + 1) - 1;
        int begY = 100 + (h - 200) / game_nb_rows(env->stock) * i;
        int endY;
        if (i == game_nb_rows(env->stock) - 1)
          endY = h - 100;
        else
          endY = 100 + (h - 200) / game_nb_rows(env->stock) * (i + 1) - 1;
        if (mouse.x >= begX && mouse.x <= endX && mouse.y >= begY &&
            mouse.y <= endY) {
          switch (e->button.button) {
            case SDL_BUTTON_LEFT:
              // Affichage de losing move dans le cas d'un mauvais emplacement d'une tente
              if (game_check_move(env->stock, i, j, TENT) == LOSING) {
                PRINT("Losing move ! \n");
                SDL_Rect rect;
                SDL_QueryTexture(env->losing_message, NULL, NULL, &rect.w,
                                 &rect.h);
                rect.x = w - 280;
                rect.y = h / 30;
                SDL_RenderCopy(ren, env->losing_message, NULL, &rect);
                SDL_RenderPresent(ren);
                SDL_Delay(155);
              }
              if (game_get_square(env->stock, i, j) == TENT)
                game_play_move(env->stock, i, j, EMPTY);
              else
                game_play_move(env->stock, i, j, TENT);
              break;
            case SDL_BUTTON_RIGHT:
              if (game_check_move(env->stock, i, j, GRASS) == LOSING) {
                PRINT("Losing move ! \n");
                SDL_Rect rect;
                SDL_QueryTexture(env->losing_message, NULL, NULL, &rect.w,
                                 &rect.h);
                rect.x = w - 280;
                rect.y = h / 30;
                SDL_RenderCopy(ren, env->losing_message, NULL, &rect);
                SDL_RenderPresent(ren);
                SDL_Delay(155);
              }

              if (game_get_square(env->stock, i, j) == GRASS)
                game_play_move(env->stock, i, j, EMPTY);
              else
                game_play_move(env->stock, i, j, GRASS);
              break;
          }
        }
      }
    }
  } else if (e->type == SDL_KEYDOWN) {
    switch (e->key.keysym.sym) {
      case SDLK_z:
        game_undo(env->stock);
        break;
      case SDLK_s:
        game_restart(env->stock);
        game_solve(env->stock);
        break;
      case SDLK_y:
        game_redo(env->stock);
        break;
      case SDLK_r:
        game_restart(env->stock);
        break;
      default:
        break;
    }
  }
  return false;
}

/* **************************************************************** */

/* PUT YOUR CODE HERE TO CLEAN MEMORY */
void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  for (int i = 0; i < game_nb_rows(env->stock); i++) {
    SDL_DestroyTexture(env->rows[i]);
    SDL_DestroyTexture(env->expected_rows[i]);

    for (int j = 0; j < game_nb_cols(env->stock); j++) {
      SDL_DestroyTexture(env->cols[j]);
      SDL_DestroyTexture(env->expected_cols[j]);
      SDL_DestroyTexture(env->grass[i * game_nb_cols(env->stock) + j]);
      SDL_DestroyTexture(env->tent[i * game_nb_cols(env->stock) + j]);
      SDL_DestroyTexture(env->tree[i * game_nb_cols(env->stock) + j]);
    }
  }
  //Free toute la structure
  free(env->rows);
  free(env->expected_rows);
  free(env->cols);
  free(env->expected_cols);
  free(env->grass);
  free(env->tent);
  free(env->losing_move);
  free(env->tree);
  SDL_DestroyTexture(env->losing_message);
  SDL_DestroyTexture(env->BRAVOOO);

  free(env);
}

/* **************************************************************** */