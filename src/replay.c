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
#include "replay.h"
#include "game.h"
#include "tools.h"
#include "effects.h"

extern L_gblOptions gblOps;

const int REPFPS[NREPSPEEDS] = { 5, 10, 20, 40, 80, 160, 320, 640 };

void
repPushUInt32 (replay_t * rep, Uint32 data)
{
  *(Uint32 *) (rep->buf) = data;
  rep->buf += sizeof (Uint32);
}

void
repPushUInt16 (replay_t * rep, Uint16 data)
{
  *(Uint16 *) (rep->buf) = data;
  rep->buf += sizeof (Uint16);
}

void
repPushUInt8 (replay_t * rep, Uint8 data)
{
  *(Uint8 *) (rep->buf) = data;
  rep->buf += sizeof (Uint8);
}

Uint32
repGetUInt32 (replay_t * rep)
{
  rep->buf += sizeof (Uint32);
  return *(Uint32 *) (rep->buf - sizeof (Uint32));
}

Uint16
repGetUInt16 (replay_t * rep)
{
  rep->buf += sizeof (Uint16);
  return *(Uint16 *) (rep->buf - sizeof (Uint16));
}

Uint8
repGetUInt8 (replay_t * rep)
{
  rep->buf += sizeof (Uint8);
  return *(Uint8 *) (rep->buf - sizeof (Uint8));
}

void
endReplay (game_t * game, int totalms)
{
  int i;

  updateReplay (game, 0);
  game->replay.bodysize = game->replay.buf - game->replay.bst;
  game->replay.totalms = totalms;
  game->replay.record = 0;

  for (i = 0; i < game->numHeros; i++)
    {
      if (game->heros[i].floor > game->replay.record)
	game->replay.record = game->heros[i].floor;
    }

  if (game->numHeros > 1)
    game->heros[i].floor /= gblOps.mpLives;
}

void
initReplay (game_t * game)
{
  int i;

  game->replay.timer = 0;
  game->replay.scrolls = 0;
  game->replay.sounds = 0;
  game->replay.bufsize = REP_BUFFER_SIZE;
  game->replay.bst = malloc (game->replay.bufsize);
  game->replay.buf = game->replay.bst;
  game->replay.nframes = 0;
  game->replay.fps = gblOps.repFps;
  game->replay.mspf = 1000.0 / gblOps.repFps;

  for (i = 0; i < game->numHeros; i++)
    game->replay.deadHero[i] = 0;

  for (i = 0; i < GRIDHEIGHT; i++)
    {
      repPushUInt8 (&(game->replay), game->floor_l[i]);
      repPushUInt8 (&(game->replay), game->floor_r[i]);
    }
  repPushUInt8 (&(game->replay), game->numHeros);
  repPushHeros (game);
}

void
updateReplay (game_t * game, float ms)
{
  int d = game->replay.buf - game->replay.bst;

  if (game->replay.bufsize - d < REP_MIN_FREE)
    {
      game->replay.bufsize += REP_BUFFER_SIZE;
      game->replay.bst = realloc (game->replay.bst, game->replay.bufsize);
      game->replay.buf = game->replay.bst + d;
    }

  game->replay.timer += ms;

  if (game->replay.timer >= game->replay.mspf)
    {
      game->replay.timer -= game->replay.mspf;
      game->replay.nframes++;
      repPushUInt8 (&(game->replay), game->replay.sounds);
      game->replay.sounds = 0;
      repPushGrid (game);
      repPushHeros (game);
    }
}

void
repPushGrid (game_t * game)
{
  int i, j;

  repPushUInt8 (&(game->replay), game->scrollCount);
  repPushUInt8 (&(game->replay), game->replay.scrolls);
  for (i = 0, j = game->mapIndex;
       i < game->replay.scrolls; i++, j = (j + 1) % GRIDHEIGHT)
    {
      repPushUInt8 (&(game->replay), game->floor_l[j]);
      repPushUInt8 (&(game->replay), game->floor_r[j]);
    }

  game->replay.scrolls = 0;
}

void
repPushHeros (game_t * game)
{
  int i;
  for (i = 0; i < game->numHeros; i++)
    {
      if (!game->replay.deadHero[i])
	{
	  repPushUInt16 (&(game->replay), game->heros[i].x);
	  repPushUInt16 (&(game->replay), game->heros[i].y);
	  repPushUInt8 (&(game->replay), game->heros[i].dir);
	  repPushUInt16 (&(game->replay), game->heros[i].angle);
	  repPushUInt8 (&(game->replay), game->heros[i].id);
	  repPushUInt8 (&(game->replay), game->heros[i].dead);
	  repPushUInt8 (&(game->replay), game->heros[i].lives);
	  repPushUInt16 (&(game->replay), game->heros[i].floor);

	  game->replay.deadHero[i] = game->heros[i].dead;
	}
    }
}

void
freeReplay (replay_t * rep)
{
  free (rep->bst);
}

void
getPlayerReplay (hero_t * hero, replay_t * rep)
{
  int newid;

  hero->x = repGetUInt16 (rep);
  hero->y = repGetUInt16 (rep);
  hero->dir = repGetUInt8 (rep);
  hero->angle = repGetUInt16 (rep);
  newid = repGetUInt8 (rep);
  hero->previd = hero->id;
  if (newid != hero->id)
    {
      hero->sprite[newid].elpTime = hero->sprite[newid].frame = 0;
    }
  hero->id = newid;
  hero->dead = repGetUInt8 (rep);
  hero->prevLives = hero->lives;
  hero->lives = repGetUInt8 (rep);
  if (hero->lives > 4)
    hero->lives = -1;
  hero->prevFloor = hero->floor;
  hero->floor = repGetUInt16 (rep);

  if (gblOps.trailMode != NOTRAIL)
    updateTrails (hero, RECMS);
  if (gblOps.useGL && gblOps.blur)
    updateBlurs (hero, RECMS);
}

void
initGameReplay (game_t * game, data_t * gfx, replay_t * rep)
{
  int i, j;

  rep->buf = rep->bst;
  rep->speed = REP_1X;

  game->floorTop = GRIDHEIGHT - 4;
  game->mapIndex = 0;
  game->scrollCount = 0;
  game->scrollTotal = 0;

  for (i = 0; i < GRIDHEIGHT; i++)
    {
      game->floor_l[i] = repGetUInt8 (rep);
      game->floor_r[i] = repGetUInt8 (rep);
    }
  game->numHeros = repGetUInt8 (rep);
  game->heros = malloc (sizeof (hero_t) * game->numHeros);
  for (i = 0; i < game->numHeros; i++)
    {
      for (j = 0; j < HEROANIMS; j++)
	{
	  initializeSpriteCtlRot (&game->heros[i].sprite[j],
				  gfx->heroSprite[i][j]);
	}
      game->heros[i].trail = NULL;
      game->heros[i].blur = NULL;
      getPlayerReplay (&(game->heros[i]), rep);
      game->heros[i].prevLives = -1;
    }
}

void
scrollReplay (game_t * game, data_t * gfx, replay_t * rep)
{
  int scrolls;
  int i, j;
  int ds;

  ds = game->scrollCount;
  game->scrollCount = repGetUInt8 (rep);
  ds = game->scrollCount - ds;

  scrolls = repGetUInt8 (rep);
  if (scrolls > 0)
    ds += BLOCKSIZE * (scrolls);

  game->mapIndex -= scrolls;
  game->floorTop += scrolls;

  game->scrollTotal += ds;

  if (game->mapIndex < 0)
    game->mapIndex += GRIDHEIGHT;
  for (i = game->mapIndex, j = 0; j < scrolls; j++, i = (i + 1) % GRIDHEIGHT)
    {
      game->floor_l[i] = repGetUInt8 (rep);
      game->floor_r[i] = repGetUInt8 (rep);
    }

  for (i = 0; i < game->numHeros; i++)
    {
      scrollTrails (&(game->heros[i]), ds);
      scrollBlurs (&(game->heros[i]), ds);
    }
}

void
playRepSounds (data_t * gfx, replay_t * rep, int mute)
{
  int sounds = repGetUInt8 (rep);
  if (!mute)
    {
      if ((sounds & S_JUMP) == S_JUMP && gfx->gjump)
	Mix_PlayChannel (-1, gfx->gjump, 0);
      if ((sounds & S_FALL) == S_FALL && gfx->gfall)
	Mix_PlayChannel (-1, gfx->gfall, 0);
      if ((sounds & S_DIE) == S_DIE   && gfx->gdie)
	Mix_PlayChannel (-1, gfx->gdie, 0);
    }
}

void
drawRepHud (data_t * gfx, replay_t * rep)
{
  char *str;
  str = malloc (sizeof (char) * strlen (_("Speed: ")) + 8);

  strcpy (str, _(" Speed: "));
  switch (rep->speed)
    {
    case REP_OX:
      strcat (str, "1/8 X ");
      break;
    case REP_QX:
      strcat (str, "1/4 X ");
      break;
    case REP_HX:
      strcat (str, "1/2 X ");
      break;
    case REP_1X:
      strcat (str, "  1 X ");
      break;
    case REP_2X:
      strcat (str, "  2 X ");
      break;
    case REP_4X:
      strcat (str, "  4 X ");
      break;
    case REP_8X:
      strcat (str, "  8 X ");
      break;
    case REP_16X:
      strcat (str, " 16 X ");
      break;
    default:
      break;
    }

  JPB_drawSquare (gfx->gcolor, gfx->galpha,
		  gfx->gameX + BLOCKSIZE,
		  gfx->gameY,
		  SFont_TextWidth (gfx->textfont, str),
		  SFont_TextHeight (gfx->textfont));

  SFont_Write (gfx->textfont, gfx->gameX + BLOCKSIZE, gfx->gameY, str);

  free (str);
}

void
updateGameReplay (game_t * game, data_t * gfx, replay_t * rep, float ms,
		  int mute)
{
  int i;

  playRepSounds (gfx, rep, mute);
  scrollReplay (game, gfx, rep);
  for (i = 0; i < game->numHeros; i++)
    {
      if (game->heros[i].dead == FALSE)
	{
	  getPlayerReplay (&(game->heros[i]), rep);
	  animateSpriteRot (&(game->heros[i].sprite[game->heros[i].id]), ms);
	}
    }
}

int
updateInputReplay (replay_t * rep, L_timer * time)
{
  int ret = 0;
  SDL_Event event;

  while (SDL_PollEvent (&event))
    {
      switch (event.type)
	{
	  /* A key is pressed */
	case SDL_KEYDOWN:
	  if (event.key.keysym.sym == SDLK_RIGHT)
	    {
	      rep->speed++;
	      if (rep->speed >= NREPSPEEDS)
		rep->speed--;
	      setFpsTimer (time, REPFPS[rep->speed]);
	    }
	  if (event.key.keysym.sym == SDLK_LEFT)
	    {
	      rep->speed--;
	      if (rep->speed < 0)
		rep->speed++;
	      setFpsTimer (time, REPFPS[rep->speed]);
	    }
	  if (event.key.keysym.sym == KEY_QUIT)
	    {
	      ret = TRUE;
	    }
	  if (event.key.keysym.sym == SDLK_p
	      || event.key.keysym.sym == SDLK_PAUSE)
	    {
	      ret = PAUSED;
	    }
	  break;
	  /* Quit: */
	case SDL_QUIT:
	  ret = TRUE;
	  break;
	  /* Default */
	default:
	  break;
	}
    }

  return ret;
}

int
playReplay (data_t * gfx, replay_t * rep)
{
  L_timer timer;
  game_t game;
  int done = FALSE;
  int r, i = 0, j;
  /* int timegap; */
  int skipf = 0;
  int lskipf = 0;
  /*int lskipf2 = 0; */
  Uint32 mymscount = 0;
  float tdelay, adelay, ladelay = 0;

  if (gfx->musgame)
    Mix_PlayMusic (gfx->musgame, -1);

  drawBg (gfx->gameBg, 0, 0, gblOps.w, gblOps.h);
  initGameReplay (&game, gfx, rep);
  initTimer (&timer, rep->fps);
  FlipScreen ();

  updateTimer (&timer);
  while (!done)
    {
      if ((r = updateInputReplay (rep, &timer)))
	{
	  if (r == PAUSED)
	    {
	      done = pauseGame (gfx, &game, _("PAUSE"));
	    }
	  else
	    {
	      done =
		yesNoQuestion (gfx, &game,
			       _
			       ("Are you sure you want to stop playing this replay? (Y/n)"));
	    }
	  continueTimer (&timer);
	}
      /*timegap = */
      updateTimer (&timer);

      tdelay = (1000.0 / (float) REPFPS[rep->speed]);
      adelay = (float) timer.ms / (skipf + 1);
      if (lskipf <= skipf && ladelay < adelay)
	skipf--;
      if (tdelay < adelay)
	skipf++;
      else
	skipf--;

      if (skipf < 0)
	skipf = 0;
      if (skipf > REPFPS[rep->speed] / REPFPS[REP_1X])
	skipf = REPFPS[rep->speed] / REPFPS[REP_1X];
      ladelay = adelay;
      lskipf = skipf;
      /* Cutremode on */
      //skipf = ceil((float)timegap/(1000.0/(float)REPFPS[rep->speed]));

      updateGameReplay (&game, gfx, rep, timer.ms, 0);

      /* Be careful when it takes longer than what we want to reach... */
      /*if (lskipf < skipf && lskipf2 < lskipf) {
         skipf = lskipf;//-1; if (skipf < 0) skipf = 0;
         } else {
         lskipf2 = lskipf;
         lskipf = skipf;
         } */

      //printf("skipping %d frames | %f, %f\n", skipf, adelay, tdelay);
      mymscount += (skipf + 1) * (1000.0 / (float) REPFPS[REP_1X]);
      for (j = 0; j < skipf && i < rep->nframes; i++, j++)
	{
	  updateGameReplay (&game, gfx, rep, 0, 1);
	}

      drawGame (gfx, &game);
      drawRepHud (gfx, rep);
      updateScore (gfx, &game, mymscount);
      FlipScreen ();

      if (++i >= rep->nframes)
	done = ENDMATCH;
    }
  if (done == ENDMATCH)
    {
      r =
	yesNoQuestion (gfx, &game,
		       _("Do you want to watch this replay again? (Y/n)"));
    }
  else
    {
      r = FALSE;
    }

  freeGameReplay (&game);

  return r;
}

void
freeGameReplay (game_t * game)
{
  int i;
  for (i = 0; i < game->numHeros; i++)
    free (game->heros[i].trail);
  free (game->heros);
}

void
fputUint32 (Uint32 data, FILE * fh)
{
  fputc (data >> 24, fh);
  fputc ((data << 8) >> 24, fh);
  fputc ((data << 16) >> 24, fh);
  fputc ((data << 24) >> 24, fh);
}

int
saveReplay (replay_t * rep, char *fname, char *comment)
{
  char *fullname = NULL;
  char buf[64];
  time_t timt;
  struct tm *tims;
  FILE *fh = NULL;
  int i;

  fullname =
    malloc (sizeof (char *) *
	    (strlen (fname) + strlen (gblOps.repDir) + strlen (REPEXT) + 2));
  sprintf (fullname, "%s/%s%s", gblOps.repDir, fname, REPEXT);

  if ((fh = fopen (fullname, "wb")) == NULL)
    {
      fprintf (stderr, _("ERROR: Could not save replay to %s\n"), fullname);
      free (fullname);
      return FALSE;
    }

  timt = time (0);
  tims = localtime (&timt);

  /* Header */
  fputUint32 (REP_VERS, fh);

  /* Comment */
  fputs (comment, fh);
  sprintf (buf, "Floor: %d", rep->record);
  fputs (" | ", fh);
  fputs (buf, fh);
  sprintf (buf, "Time: %d", rep->totalms / 1000);
  fputs (" | ", fh);
  fputs (buf, fh);
  strftime (buf, 64, "%H:%M %d/%m/%y", tims);
  fputs (" | ", fh);
  fputs (buf, fh);

  fputc ('\0', fh);

  /* The rest of the header */
  fputUint32 (rep->record, fh);
  fputUint32 (rep->totalms, fh);
  fputUint32 (rep->fps, fh);
  fputUint32 (rep->nframes, fh);

  /* Body */
  for (i = 0; i < rep->bodysize; i++)
    {
      fputc (((char *) (rep->bst))[i], fh);
    }

  free (fullname);

  fclose (fh);

  return TRUE;
}

Uint32
fgetUint32 (FILE * fh)
{
  Uint32 data = 0;
  data |= fgetc (fh) << 24;
  data |= fgetc (fh) << 16;
  data |= fgetc (fh) << 8;
  data |= fgetc (fh);
  return data;
}

int
loadReplay (data_t * gfx, char *file)
{
  FILE *fh = NULL;
  int version;
  int size;
  replay_t rep;
  char *comment;
  int i;

  size = getFileSize (file);

  if ((fh = fopen (file, "rb")) == NULL)
    {
      fprintf (stderr, _("ERROR: Could not open replay %s\n"), file);
      return FALSE;
    }

  version = fgetUint32 (fh);
  if (version != REP_VERS)
    {
      fprintf (stderr, _("ERROR: Replay %s is of a different version.\n"),
	       file);
      return FALSE;
    }

  i = 0;
  comment = malloc (sizeof (char) * MAX_CHAR);
  while ((comment[i++] = fgetc (fh)) != '\0');

  rep.record = fgetUint32 (fh);
  rep.totalms = fgetUint32 (fh);
  rep.fps = fgetUint32 (fh);
  rep.nframes = fgetUint32 (fh);
  rep.buf = rep.bst = malloc (size);
  rep.bufsize = size;

  for (rep.bodysize = 0; !feof (fh); rep.bodysize++)
    {
      ((char *) (rep.buf))[rep.bodysize] = fgetc (fh);
    }

  fclose (fh);

  printf (_
	  ("Replay loaded: Size: %d FPS: %d Frames: %d Record: %d Time: %d Comment: %s\n"),
	  size, rep.fps, rep.nframes, rep.record, rep.totalms / 1000,
	  comment);
  while (playReplay (gfx, &rep));

  freeReplay (&rep);
  free (comment);

  return TRUE;
}

char *
getReplayComment (char *file)
{
  FILE *fh = NULL;
  int version;
  char *comment = NULL;
  int i;

  if ((fh = fopen (file, "rb")) == NULL)
    {
      fprintf (stderr, _("WARNING: Could not open replay %s\n"), file);
      return NULL;
    }

  version = fgetUint32 (fh);
  if (version != REP_VERS)
    {
      fprintf (stderr, _("WARNING: Replay %s is of a different version.\n"),
	       file);
      return NULL;
    }

  i = 0;
  comment = malloc (sizeof (char) * MAX_CHAR);
  while ((comment[i++] = fgetc (fh)) != '\0');

  return comment;
}
