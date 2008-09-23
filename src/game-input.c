/*
 * GNUjump
 * =======
 *
 * Copyright (C) 2005-2008, Juan Pedro Bolivar Puente
 *
 * GNUjump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUjump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include "gnujump.h"
#include "game.h"

extern L_gblOptions gblOps;

int
updateInput (game_t * game)
{
  int done = 0;
  int i;
  SDL_Event event;

  while (SDL_PollEvent (&event))
    {
      switch (event.type)
	{
	  /* A key is pressed */
	case SDL_KEYDOWN:
	  if (event.key.keysym.sym == KEY_QUIT)
	    {
	      done = TRUE;
	    }
	  if (event.key.keysym.sym == SDLK_p
	      || event.key.keysym.sym == SDLK_PAUSE)
	    {
	      done = PAUSED;
	    }
	  for (i = 0; i < game->numHeros; i++)
	    {
	      markHeroKeys (&event, &game->heros[i]);
	    }
	  break;
	  /* A key UP. */
	case SDL_KEYUP:
	  for (i = 0; i < game->numHeros; i++)
	    {
	      unmarkHeroKeys (&event, &game->heros[i]);
	    }
	  break;
	  /* Quit: */
	case SDL_QUIT:
	  done = TRUE;
	  break;
	  /* Default */
	default:
	  break;
	}
    }

  return done;
}

void
initHeroKeys (hero_t * hero, int num)
{
  hero->left.def = gblOps.keys[num][LEFTK];
  hero->left.pressed = FALSE;
  hero->right.def = gblOps.keys[num][RIGHTK];
  hero->right.pressed = FALSE;
  hero->up.def = gblOps.keys[num][JUMPK];
  hero->up.pressed = FALSE;
}

void
markHeroKeys (SDL_Event * event, hero_t * hero)
{
  if (event->key.keysym.sym == hero->left.def)
    {
      hero->left.pressed = 1;
    }
  else if (event->key.keysym.sym == hero->right.def)
    {
      hero->right.pressed = 1;
    }
  else if (event->key.keysym.sym == hero->up.def)
    {
      hero->up.pressed = 1;
    }
}

void
unmarkHeroKeys (SDL_Event * event, hero_t * hero)
{
  if (event->key.keysym.sym == hero->left.def)
    {
      hero->left.pressed = 0;
    }
  else if (event->key.keysym.sym == hero->right.def)
    {
      hero->right.pressed = 0;
    }
  else if (event->key.keysym.sym == hero->up.def)
    {
      hero->up.pressed = 0;
    }
}
