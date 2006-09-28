/*
 * SDLjump
 * (C) 2005 Juan Pedro Bolíar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * setup.c
 */

/*
    Copyright (C) 2003-2004, Juan Pedro Bolivar Puente

    SDLjump is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    SDLjump is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDLjump; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "sdljump.h"
#include "setup.h"
#include "surface.h"
#include "sprite.h"
#include "tools.h"

extern SDL_Surface *screen;
extern L_gblOptions gblOps;

void initGblOps(void)
{   
    int i;
    char* homeDir;
    char* cfgDir;
    
    gblOps.aa = 0;
    gblOps.useGL = FALSE;
    gblOps.w = 0;
    gblOps.h = 0;
    gblOps.bpp = BPPAUTO;
    
    gblOps.fps = FPS100;
    gblOps.rotMode = ROTFULL;
    gblOps.scrollMode = SOFTSCROLL;
    gblOps.mpLives = 3;
    gblOps.nplayers = 0;
    gblOps.recReplay = 0;
    gblOps.repFps = 40;
    gblOps.sndvolume = 9;
    gblOps.musvolume = 6;
    
#ifndef WIN32
	homeDir = getenv("HOME");
	cfgDir = malloc(sizeof(char) * (strlen(homeDir) + strlen(".sdljump") + 2));
	sprintf(cfgDir, "%s/.sdljump", homeDir);
	if (access(cfgDir, F_OK) < 0) mkdir(cfgDir, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
#endif

	/* Setting up the Default skin as skin and English as the default lang. */
#ifdef WIN32
	gblOps.dataDir  = malloc(sizeof(char)* (strlen("skins/")+strlen(DEFTHEME)+1));
    sprintf(gblOps.dataDir, "skins/%s",DEFTHEME);
    gblOps.langFile = malloc(sizeof(char)* (strlen(DEFLANG)+1));
    sprintf(gblOps.langFile, "%s", DEFTHEME);
	gblOps.repDir = malloc(sizeof(char)* (strlen(".")+1));
    sprintf(gblOps.repDir, ".");
#else
	gblOps.repDir = malloc(sizeof(char)* (strlen(cfgDir)+1));
    sprintf(gblOps.repDir, "%s", cfgDir);
  #ifndef DEVEL
    gblOps.dataDir  = malloc(sizeof(char)* (strlen(DATA_PREFIX) + strlen(PACKAGE) + strlen("/skins/") + strlen(DEFTHEME)+2));
    gblOps.langFile = malloc(sizeof(char)* (strlen(DATA_PREFIX) + strlen(PACKAGE) + strlen(DEFLANG)+3));
    sprintf(gblOps.dataDir, "%s/%s/skins/%s",DATA_PREFIX,PACKAGE,DEFTHEME);
    sprintf(gblOps.langFile, "%s/%s/%s", DATA_PREFIX,PACKAGE,DEFLANG);
  #else
	gblOps.dataDir = malloc(sizeof(char) *(strlen("../skins/") + strlen(DEFTHEME)+1));
    sprintf(gblOps.dataDir, "../skins/%s",DEFTHEME);
    gblOps.langFile = malloc(sizeof(char)*(strlen("../") + strlen(DEFLANG)+1));
    sprintf(gblOps.langFile, "../%s", DEFLANG);
  #endif
#endif
	
    /* Setting up the list of skin and theme folders */
#ifdef WIN32
	gblOps.ntfolders = 1;
    gblOps.nlfolders = 1;
	gblOps.nrfolders = 1;
	gblOps.themeDirs = malloc(sizeof(char*)*gblOps.ntfolders);
	gblOps.langDirs = malloc(sizeof(char*)*gblOps.nlfolders);
	gblOps.repDirs = malloc(sizeof(char*)*gblOps.nrfolders);
	
	gblOps.themeDirs[0] = malloc(sizeof(char) * (strlen("skins")+1));
	strcpy(gblOps.themeDirs[0],"skins");
	gblOps.langDirs[0] = malloc(sizeof(char) * (strlen("lang")+1));
	strcpy(gblOps.langDirs[0],"lang");
	gblOps.repDirs[0] = malloc(sizeof(char) * (strlen(".")+1));
	strcpy(gblOps.repDirs[0],".");
#else
	gblOps.ntfolders = 2;
    gblOps.nlfolders = 2;
    gblOps.nrfolders = 1;
	gblOps.themeDirs = malloc(sizeof(char*)*gblOps.ntfolders);
	gblOps.langDirs = malloc(sizeof(char*)*gblOps.nlfolders);
	gblOps.repDirs = malloc(sizeof(char*)*gblOps.nrfolders);
	
	gblOps.themeDirs[0] = malloc(sizeof(char)*strlen(cfgDir)+1);
  	gblOps.langDirs[0] = malloc(sizeof(char)*strlen(cfgDir)+1);
  	gblOps.repDirs[0] = malloc(sizeof(char)*strlen(cfgDir)+1);
  	strcpy(gblOps.themeDirs[0], cfgDir);
  	strcpy(gblOps.langDirs[0], cfgDir);
  	strcpy(gblOps.repDirs[0], cfgDir);	
  	
  	free(cfgDir);
  #ifndef DEVEL
	gblOps.themeDirs[1] = malloc(sizeof(char) *	(strlen(DATA_PREFIX) + strlen(PACKAGE) + strlen("/skins")  +2));
	sprintf(gblOps.themeDirs[1],"%s/%s/skins",DATA_PREFIX,PACKAGE);
	gblOps.langDirs[1] = malloc(sizeof(char) *	(strlen(DATA_PREFIX) + strlen(PACKAGE) + strlen("/lang")  +2));
	sprintf(gblOps.langDirs[1],"%s/%s/lang",DATA_PREFIX,PACKAGE);
  #else
    gblOps.themeDirs[1] = malloc(sizeof(char) * (strlen("../skins") +2));
	sprintf(gblOps.themeDirs[1],"../skins");
	gblOps.langDirs[1] = malloc(sizeof(char) * (strlen("../lang") +2));
	sprintf(gblOps.langDirs[1],"../lang");
  #endif
#endif
        
    /* Default Keys */
    gblOps.keys[0][LEFTK] = KEY_LEFT1;
    gblOps.keys[1][LEFTK] = KEY_LEFT2;
    gblOps.keys[2][LEFTK] = KEY_LEFT3;
    gblOps.keys[3][LEFTK] = KEY_LEFT4;
    
    gblOps.keys[0][RIGHTK] = KEY_RIGHT1;
    gblOps.keys[1][RIGHTK] = KEY_RIGHT2;
    gblOps.keys[2][RIGHTK] = KEY_RIGHT3;
    gblOps.keys[3][RIGHTK] = KEY_RIGHT4;
    
    gblOps.keys[0][JUMPK] = KEY_UP1;
    gblOps.keys[1][JUMPK] = KEY_UP2;
    gblOps.keys[2][JUMPK] = KEY_UP3;
    gblOps.keys[3][JUMPK] = KEY_UP4;
    
    /* Default player names */
    for (i=0; i<MAX_PLAYERS; i++) {
        gblOps.pname[i] = malloc(sizeof(char)*(strlen(PNAME)+1));
        sprintf(gblOps.pname[i],"%s%d",PNAME,i+1);
    }
}

void cleanGblOps(void)
{
    int i;
    
    free(gblOps.dataDir);
    for (i=0; i<MAX_PLAYERS; i++) {
        free(gblOps.pname[i]);
    }
	for (i=0; i<gblOps.ntfolders; i++) {
        free(gblOps.themeDirs[i]);
    }
    for (i=0; i<gblOps.nlfolders; i++) {
        free(gblOps.langDirs[i]);
    }
	free(gblOps.themeDirs);
	free(gblOps.langDirs);
}

int loadConfigFile(char* fname)
{
    FILE* tfile;
    int i;
    char str[MAX_CHAR];
    
    if ((tfile = fopen(fname,"r"))==NULL) {
        fprintf(stderr,"WARNING: Can't open config file (%s). I will create one later.\n",fname);
        return FALSE;
    }
    getValue_str(tfile,"protocol_version",str,FALSE);
    if (strcmp(str,PROT_VERS)!=0) {
        fclose(tfile);
        fprintf(stderr,"WARNING: Config file (%s) is not compatible with this version of sdljump. I will rewrite it later.\n",fname);
        return FALSE;
    }
    
    gblOps.fps = getValue_int(tfile,"fps_limit");
    gblOps.rotMode = getValue_int(tfile,"rotation_mode");
    gblOps.scrollMode = getValue_int(tfile,"scroll_mode");
    gblOps.mpLives = getValue_int(tfile,"multiplayer_lives");
    gblOps.nplayers = getValue_int(tfile,"number_players");
    gblOps.recReplay = getValue_int(tfile,"record_replay");
    gblOps.repFps = getValue_int(tfile, "replay_fps");
    
    gblOps.useGL = getValue_int(tfile,"use_opengl");
    gblOps.bpp = getValue_int(tfile,"bpp");
    gblOps.w = getValue_int(tfile,"screen_width");
    gblOps.h = getValue_int(tfile,"screen_height");
    gblOps.fullsc = getValue_int(tfile,"fullscreen");
    gblOps.aa = getValue_int(tfile,"antialiasing");
    
    gblOps.sndvolume = getValue_int(tfile,"sound_volume");
    gblOps.musvolume = getValue_int(tfile,"music_volume");
    
    gblOps.dataDir = getValue_charp(tfile,"default_skin");	
	gblOps.ntfolders = getValue_int(tfile,"skin_folders");
	gblOps.themeDirs = malloc(sizeof(char*)*gblOps.ntfolders);
	for (i = 0; i< gblOps.ntfolders; i++) {
		gblOps.themeDirs[i] = getValue_charp(tfile,"skin_dir");
	}
    
    gblOps.langFile = getValue_charp(tfile,"default_lang");	
	gblOps.nlfolders = getValue_int(tfile,"lang_folders");
	gblOps.langDirs = malloc(sizeof(char*)*gblOps.nlfolders);
	for (i = 0; i< gblOps.nlfolders; i++) {
		gblOps.langDirs[i] = getValue_charp(tfile,"lang_dir");
	}
	
	gblOps.repDir = getValue_charp(tfile,"replay_save_folder");	
	gblOps.nrfolders = getValue_int(tfile,"replay_folders");
	gblOps.repDirs = malloc(sizeof(char*)*gblOps.nrfolders);
	for (i = 0; i< gblOps.nrfolders; i++) {
		gblOps.repDirs[i] = getValue_charp(tfile,"replay_dir");
	}
    
    for (i=0; i<MAX_PLAYERS; i++) {
        getValue_str(tfile,"player_name",str, FALSE);
        gblOps.pname[i] = malloc(sizeof(char)*(strlen(str)+1));
        strcpy(gblOps.pname[i],str);
        
        gblOps.keys[i][LEFTK] = getValue_int(tfile,"key_left");
        gblOps.keys[i][RIGHTK] = getValue_int(tfile,"key_right");
        gblOps.keys[i][JUMPK] = getValue_int(tfile,"key_jump");
    }
    
    fclose(tfile);
    
    return TRUE;
}

int writeConfigFile(char* fname)
{
    FILE* tfile;
    int i;
    
    if ((tfile = fopen(fname,"w"))==NULL) {
        fprintf(stderr,"WARNING: Can't open config file (%s). Make sure that it is not being used by another app.\n",fname);
		return FALSE;
    }
    
    putComment(tfile,"This file has been automatically generated by SDLjump");
    putLine(tfile);
    putValue_str(tfile,"protocol_version",PROT_VERS);
    
    putLine(tfile);
    putComment(tfile,"Game options");
    putValue_int(tfile,"fps_limit",gblOps.fps);
    putValue_int(tfile,"rotation_mode",gblOps.rotMode);
    putValue_int(tfile,"scroll_mode",gblOps.scrollMode);
    putValue_int(tfile,"multiplayer_lives", gblOps.mpLives);
    putValue_int(tfile,"number_players", gblOps.nplayers);
    putValue_int(tfile,"record_replay", gblOps.recReplay);
    putValue_int(tfile,"replay_fps", gblOps.repFps);
    
    putLine(tfile);
    putComment(tfile,"Graphics options");
    putValue_int(tfile,"use_opengl",gblOps.useGL);
    putValue_int(tfile,"bpp",gblOps.bpp);
    putValue_int(tfile,"screen_width",gblOps.w);
    putValue_int(tfile,"screen_height",gblOps.h);
    putValue_int(tfile,"fullscreen",gblOps.fullsc);
    putValue_int(tfile,"antialiasing",gblOps.aa);
    
    putValue_int(tfile,"sound_volume", gblOps.sndvolume);
    putValue_int(tfile,"music_volume", gblOps.musvolume);
    
    putLine(tfile);
    putComment(tfile,"Skin options");
    putValue_str(tfile,"default_skin",gblOps.dataDir);
    putValue_int(tfile,"skin_folders",gblOps.ntfolders);
	for (i = 0; i< gblOps.ntfolders; i++) {
		putValue_str(tfile,"skin_dir",gblOps.themeDirs[i]);
	}
	
	putLine(tfile);
    putComment(tfile,"Lang options");
    putValue_str(tfile,"default_lang",gblOps.langFile);
    putValue_int(tfile,"lang_folders",gblOps.nlfolders);
	for (i = 0; i< gblOps.nlfolders; i++) {
		putValue_str(tfile,"lang_dir",gblOps.langDirs[i]);
	}
	
	putLine(tfile);
    putComment(tfile,"Replay dir options");
    putValue_str(tfile,"replay_save_folder",gblOps.repDir);
    putValue_int(tfile,"replay_folders", gblOps.nrfolders);
	for (i = 0; i< gblOps.nrfolders; i++) {
		putValue_str(tfile,"replay_dir",gblOps.repDirs[i]);
	}
	
    putLine(tfile);
    putComment(tfile,"Player options");
    for (i=0; i<MAX_PLAYERS; i++) {
        putValue_str(tfile,"player_name",gblOps.pname[i]);
        putValue_int(tfile,"key_left",gblOps.keys[i][LEFTK]);
        putValue_int(tfile,"key_right",gblOps.keys[i][RIGHTK]);
        putValue_int(tfile,"key_jump",gblOps.keys[i][JUMPK]);
        putLine(tfile);
    }
    
    fclose(tfile);
    
    return TRUE;
}

char* getThemeComment(char* fname)
{
	FILE* fh;
	char str[MAX_CHAR];
	char* ret = NULL;
	char* file = NULL;
	
	file = malloc(sizeof(char)*(strlen(fname)+strlen(THEMEFILE)+1));
	strcpy (file,fname); strcat(file,THEMEFILE);
	
	if ((fh = fopen(file,"r")) == NULL){
        fprintf(stderr,"WARNING: Can open theme file (%s).\n", file);
		free(file);
        return NULL;
    }
	
	getValue_str(fh,"format",str,FALSE);
    if (strcmp(str,THEME_VERS)!=0) {
        fclose(fh);
        fprintf(stderr,"WARNING: Theme file (%s) is not of the correct format.\n", file);
		free(file);
        return NULL;
    }
	
	ret = getValue_charp(fh,"comment");
	fclose(fh);

	printf("Successfully loaded comment from: %s\n",file);
	free(file);
	
	return ret;
}

char* getLangComment(char* fname)
{
	FILE* fh;
	char str[MAX_CHAR];
	char* ret = NULL;

	if ((fh = fopen(fname,"r")) == NULL){
        fprintf(stderr,"WARNING: Can open lang file (%s).\n", fname);
        return NULL;
    }
	
	getValue_str(fh,"format",str,FALSE);
    if (strcmp(str,LANG_VERS)!=0) {
        fclose(fh);
        fprintf(stderr,"WARNING: Theme file (%s) is not of the correct format.\n", fname);
        return NULL;
    }
	
	ret = getValue_charp(fh,"comment");
	fclose(fh);
	printf("Successfully loaded comment from: %s\n",fname);
	
	return ret;
}

void EngineInit()
{	
    //Everything starts!!
    if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 ) {
        fprintf(stderr, "ERROR: SDL_Init did not work because: %s\n", SDL_GetError());
    	exit(2);
    }
    
	atexit(SDL_Quit); //this avoids exiting without ending 
        
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
		fprintf(stderr, "ERROR: Mix_OpenAudio: %s\n", Mix_GetError());
	else	
		resetVolumes();
}

void setWindow()
{
	int bpp;
	const SDL_VideoInfo* info = NULL;
	
	switch(gblOps.bpp) {
		case BPPAUTO:
			info = SDL_GetVideoInfo();
			bpp = info->vfmt->BitsPerPixel;
			break;
		case BPP32:
			bpp = 32; break;
		case BPP16:
			bpp = 16; break;
		case BPP8:
			bpp = 8; break;
		default:
			info = SDL_GetVideoInfo();
			bpp = info->vfmt->BitsPerPixel;
			break;
	}
		
	if (!gblOps.aa) gblOps.texFilter = GL_NEAREST;
	else gblOps.texFilter = GL_LINEAR;
				
	if (gblOps.useGL) {
		SetVideoGl(gblOps.w, gblOps.h, gblOps.fullsc, bpp);
		SDL_WM_SetCaption("SDLjump " VERSION " (OpenGL rendering)",NULL);
	} else {
		SetVideoSw(gblOps.w, gblOps.h, gblOps.fullsc, bpp);
		SDL_WM_SetCaption("SDLjump " VERSION " (Sofware rendering)",NULL);
	}
	
	printf("Window created. BPP: %d, Resolution: %dx%d, OpenGL: %d\n",
		screen->format->BitsPerPixel,
		screen->w,screen->h,
		(screen->flags & SDL_OPENGL) == SDL_OPENGL
	);
	
	SDL_ShowCursor(SDL_DISABLE);
}

void SetVideoGl(int w, int h, int use_fullscreen,int bpp)
{
	
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
	if (use_fullscreen) {
		screen = SDL_SetVideoMode(w, h, bpp, SDL_FULLSCREEN|SDL_OPENGL);
		if (screen == NULL) {
			fprintf(stderr,
				"WARNING: I could not set up fullscreen video for "
				"640x480 mode."
				"The Simple DirectMedia error that occured was:"
				"%s\n", SDL_GetError());
			use_fullscreen = 0;
		}
	} else {
		screen = SDL_SetVideoMode(w, h, bpp, SDL_OPENGL);
	
		if (screen == NULL) {
			fprintf(stderr,
					"ERROR: I could not set up video for 640x480 mode."
				" The Simple DirectMedia error that occured was: "
				"%s\n", SDL_GetError());
			exit(1);
		}
	}
	
	/*
	* Set up OpenGL for 2D rendering.
	*/
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	glViewport(0, 0, screen->w, screen->h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0f, screen->w, screen->h, 0.0f, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void SetVideoSw(int w, int h, int fullscreen,int bpp)
{
    if (fullscreen == 1) {
        screen = SDL_SetVideoMode(w,h,bpp, 
                          SDL_ANYFORMAT |
                          SDL_FULLSCREEN |
                          SDL_SWSURFACE);
		if (screen == NULL) {
            printf("ERROR: The screen wasn't initialized beacause: %s\n", SDL_GetError());
        }
    } else { //Fullscreen OFF
        screen = SDL_SetVideoMode(w,h,bpp,SDL_HWSURFACE /* | SDL_DOUBLEBUF*/);
        if (screen == NULL) {
            printf("ERROR: The screen wasn't initialized beacause: %s\n", SDL_GetError());
        }
    }
}

void resetVolumes()
{
	Mix_Volume(-1, ((float)gblOps.sndvolume/9.0)*MIX_MAX_VOLUME);
	Mix_VolumeMusic(((float)gblOps.musvolume/9.0)*MIX_MAX_VOLUME);
}

int loadSounds(data_t* data, char* fname)
{
    FILE* fh;
    char str[MAX_CHAR];
	char* file = NULL;
	
	file = malloc(sizeof(char)*(strlen(fname)+strlen(SOUNDFILE)+1));
	strcpy (file,fname); strcat(file,SOUNDFILE);
	
	printf("Loading sounds: %s\n",file);
	data->soundloaded = TRUE;
	
	if ((fh = fopen(file,"r")) == NULL){
        fprintf(stderr,"ERROR: Can open sounds file (%s).\n", file);
		free(file);
        return 0;
    }
    
	getValue_str(fh,"format",str,FALSE);
    if (strcmp(str,SOUND_VERS)!=0) {
        fclose(fh);
        fprintf(stderr,"ERROR: Sound file (%s) is not of the correct format.\n", file);
		free(file);
        return 0;
    }
    
    getValue_str(fh,"mus_game",str,fname);
    data->musgame = Mix_LoadMUS(str);
    
    getValue_str(fh,"snd_die",str,fname);
    data->gdie = Mix_LoadWAV(str);
    getValue_str(fh,"snd_fall",str,fname);
    data->gfall = Mix_LoadWAV(str);
    getValue_str(fh,"snd_jump",str,fname);
    data->gjump = Mix_LoadWAV(str);
    getValue_str(fh,"snd_question",str,fname);
    data->gquestion = Mix_LoadWAV(str);
    getValue_str(fh,"snd_record",str,fname);
    data->grecord = Mix_LoadWAV(str);
    
    getValue_str(fh,"mus_menu",str,fname);
    data->musmenu = Mix_LoadMUS(str);
    getValue_str(fh,"snd_click",str,fname);
    data->mclick = Mix_LoadWAV(str);
    getValue_str(fh,"snd_back",str,fname);
    data->mback = Mix_LoadWAV(str);
    
    fclose(fh);
	
	free(file);
	
    return TRUE;
}

void freeSounds(data_t* data)
{
	Mix_FreeChunk(data->gdie);
	Mix_FreeChunk(data->gfall);
	Mix_FreeChunk(data->gjump);
	Mix_FreeChunk(data->gquestion);
	Mix_FreeChunk(data->grecord);
	Mix_FreeChunk(data->mclick);
	Mix_FreeChunk(data->mback);
	Mix_FreeMusic(data->musgame);
	Mix_FreeMusic(data->musmenu);
	data->soundloaded = 0;
}

int loadGraphics(data_t* data, char* fname)
{
    FILE* fh;
    char str[MAX_CHAR];
    SDL_Surface* surf;
    int i;
    Uint8 b,g,r;
	char* file = NULL;
	
	file = malloc(sizeof(char)*(strlen(fname)+strlen(THEMEFILE)+1));
	strcpy (file,fname); strcat(file,THEMEFILE);
	
	printf("Loading theme: %s\n",file);

	if ((fh = fopen(file,"r")) == NULL){
        fprintf(stderr,"ERROR: Can open theme file (%s).\n", file);
		free(file);
        return 0;
    }
	
	/* 
	 * Global options 
	 */
	getValue_str(fh,"format",str,FALSE);
    if (strcmp(str,THEME_VERS)!=0) {
        fclose(fh);
        fprintf(stderr,"ERROR: Theme file (%s) is not of the correct format.\n", file);
		free(file);
        return 0;
    }
		
	skipValueStr(fh); /* Comment */
	
    gblOps.w = getValue_int(fh,"window_width");
    gblOps.h = getValue_int(fh,"window_height");
    
    setWindow();
	
	getValue_str(fh,"sound_theme",str,FALSE);
	if (data->soundloaded) freeSounds(data);
	if (!strcmp(str, "default")) loadSounds(data, DEFSOUND);
	else loadSounds(data, str);
	
	/*
	 * Mouse icon
	 */
	getValue_str(fh,"mouse_idle",str,fname);
    data->mouse[M_IDLE] = loadSpriteData(str, 1, fname);
    getValue_str(fh,"mouse_over",str,fname);
    data->mouse[M_OVER] = loadSpriteData(str, 1, fname);
	getValue_str(fh,"mouse_down",str,fname);
    data->mouse[M_DOWN] = loadSpriteData(str, 1, fname);
    
	data->mouseX = getValue_int(fh,"mouse_x");
	data->mouseY = getValue_int(fh,"mouse_y");
	
	/* 
	 * Menu data 
	 */
    getValue_str(fh,"menu_bg",str,fname);
    data->menuBg = JPB_LoadImg( str, gblOps.useGL, 0, 0, 0);
    getValue_str(fh,"menu_dwarrow",str,fname);
    data->dwArrow = JPB_LoadImg( str, gblOps.useGL, 1, 0, 0);
    getValue_str(fh,"menu_uparrow",str,fname);
    data->upArrow = JPB_LoadImg( str, gblOps.useGL, 1, 0, 0);
	
	getValue_str(fh,"menu_font",str,fname);
    surf = IMG_Load(str);    
    data->menufont = SFont_InitFont(str, surf, gblOps.useGL, 1);
    SDL_FreeSurface(surf);
	
	getValue_str(fh,"tip_font",str,fname);
    surf = IMG_Load(str);    
    data->tipfont = SFont_InitFont(str, surf, gblOps.useGL, 1);
    SDL_FreeSurface(surf);
	
	data->mAlign = getValue_int(fh,"menu_align");
    data->tAlign = getValue_int(fh,"tip_align");
	
	data->menuX = getValue_int(fh,"menu_x");
    data->menuY = getValue_int(fh,"menu_y");
	data->menuW = getValue_int(fh,"menu_width");
	data->mMaxOps = getValue_int(fh,"menu_max_options");
	
	data->mDwArrowX = getValue_int(fh,"menu_dwarrow_x");
    data->mDwArrowY = getValue_int(fh,"menu_dwarrow_y");
    data->mUpArrowX = getValue_int(fh,"menu_uparrow_x");
    data->mUpArrowY = getValue_int(fh,"menu_uparrow_y");
	
	data->mMargin = getValue_int(fh,"menu_margin");

	data->tipX = getValue_int(fh,"tip_x");
    data->tipY = getValue_int(fh,"tip_y");
	data->tipW = getValue_int(fh,"tip_width");
	data->tipH = getValue_int(fh,"tip_height");
	
	r = getValue_int(fh,"hl_red");
    g = getValue_int(fh,"hl_green");
    b = getValue_int(fh,"hl_blue");
	data->hlalpha = getValue_int(fh,"hl_alpha");
    data->hlcolor = SDL_MapRGB(screen->format, r, g, b);
	
	/* 
	 * In-game data
	 */
    getValue_str(fh,"game_bg",str,fname);
    data->gameBg = JPB_LoadImg( str, gblOps.useGL, 0, 0, 0);

	getValue_str(fh,"live_pic",str,fname);
    data->livePic = JPB_LoadImg( str, gblOps.useGL, 1, 0, 0);
	data->liveAlign = getValue_int(fh,"live_align");
	
    getValue_str(fh,"score_font",str,fname);
    surf = IMG_Load(str);    
    data->scorefont = SFont_InitFont(str, surf, gblOps.useGL, 1);
    SDL_FreeSurface(surf);

    getValue_str(fh,"timer_font",str,fname);
    surf = IMG_Load(str);    
    data->timefont = SFont_InitFont(str, surf, gblOps.useGL, 1);
    SDL_FreeSurface(surf);
    
	getValue_str(fh,"game_font",str,fname);
    surf = IMG_Load(str);    
    data->textfont = SFont_InitFont(str, surf, gblOps.useGL, 1);
    SDL_FreeSurface(surf);
	
	r = getValue_int(fh,"g_red");
    g = getValue_int(fh,"g_green");
    b = getValue_int(fh,"g_blue");
	data->galpha = getValue_int(fh,"g_alpha");
    data->gcolor = SDL_MapRGB(screen->format, r, g, b);
	
    data->gameX = getValue_int(fh,"game_x");
    data->gameY = getValue_int(fh,"game_y");
    
    data->timeX = getValue_int(fh,"timer_x");
    data->timeY = getValue_int(fh,"timer_y");
    
    for (i=0; i<MAX_PLAYERS; i++) {
        data->scoreX[i] = getValue_int(fh,"score_x");
        data->scoreY[i] = getValue_int(fh,"score_y");
    	data->livesX[i] = getValue_int(fh,"lives_x");
        data->livesY[i] = getValue_int(fh,"lives_y");
    }
    
    getValue_str(fh,"floor_left",str,fname);
    data->floorL = JPB_LoadImg( str, gblOps.useGL, 1, 0, 0);
    getValue_str(fh,"floor_right",str,fname);
    data->floorR = JPB_LoadImg( str, gblOps.useGL, 1, 0, 0);
    getValue_str(fh,"floor_center",str,fname);
    data->floorC = JPB_LoadImg( str, gblOps.useGL, 1, 0, 0);
    
    for (i=0; i<MAX_PLAYERS; i++) {
        getValue_str(fh,"hero_stand_anim",str,fname);
        data->heroSprite[i][H_STAND] = loadSpriteDataRot(str, 2, fname);
        getValue_str(fh,"hero_run_anim",str,fname);
        data->heroSprite[i][H_WALK] = loadSpriteDataRot(str, 2, fname);
        getValue_str(fh,"hero_jump_anim",str,fname);
        data->heroSprite[i][H_JUMP] = loadSpriteDataRot(str, 2, fname);
    }
    
    fclose(fh);
	
	free(file);
	
    return TRUE;
}


void freeGraphics(data_t* data)
{
    int i,j;
    for (j=0; j<M_STATES; j++)
        freeSpriteData(data->mouse[j]);
    JPB_FreeSurface(data->gameBg);
    JPB_FreeSurface(data->livePic);
    JPB_FreeSurface(data->floorR);
    JPB_FreeSurface(data->floorL);
    JPB_FreeSurface(data->floorC);
    JPB_FreeSurface(data->menuBg);
    JPB_FreeSurface(data->dwArrow);
    JPB_FreeSurface(data->upArrow);
    SFont_FreeFont(data->scorefont);
    SFont_FreeFont(data->textfont);
    SFont_FreeFont(data->menufont);
	SFont_FreeFont(data->tipfont);
	for (i=0; i<MAX_PLAYERS; i++) {
    	for (j=0; j<HEROANIMS; j++)
        	freeSpriteDataRot(data->heroSprite[i][j]);
    }
}

void freeLanguage(data_t* data)
{
	int i;
	for (i = 0; i<MSG_COUNT; i++) {
		free(data->msg[i]);
	}
	for (i = 0; i<TIP_COUNT; i++) {
		free(data->tip[i]);
	}
	for (i = 0; i<TXT_COUNT; i++) {
		free(data->txt[i]);
	}
	for (i = 0; i<OPT_COUNT; i++) {
		free(data->opt[i]);
	}
}

int loadLanguage(data_t* data, char* fname)
{
    FILE* fh;
	//char* file = NULL;
	char str[512];
	
/*	for (i = 0; i<MSG_COUNT; i++) {
		data->msg[i] = NULL;
	}
	for (i = 0; i<TIP_COUNT; i++) {
		data->tip[i] = NULL;
	}
	for (i = 0; i<TXT_COUNT; i++) {
		data->txt[i] = NULL;
	}
	for (i = 0; i<OPT_COUNT; i++) {
		data->opt[i] = NULL;
	}
*/

    printf("Loading language: %s\n",fname);
    
	if ((fh = fopen(fname,"r")) == NULL){
        fprintf(stderr,"ERROR: Can open language file (%s).\n", fname);
        return 0;
    }
    
    /* 
	 * Global options 
	 */
	getValue_str(fh,"format",str,FALSE);
    if (strcmp(str,LANG_VERS)!=0) {
        fclose(fh);
        fprintf(stderr,"ERROR: Language file (%s) is not of the correct format.\n", fname);
        return 0;
    }
    
    skipValueStr(fh); /* Comment */
    
	/*
	 * In-game messages
	 */
	data->txt[txt_name] = getValue_charp(fh, "txt_name");
	data->txt[txt_floor] = getValue_charp(fh, "txt_floor");
	data->txt[txt_mode] = getValue_charp(fh, "txt_mode");
	data->txt[txt_time] = getValue_charp(fh, "txt_time");
	data->txt[txt_date] = getValue_charp(fh, "txt_date");
	data->txt[txt_hscnote] = getValue_charp(fh, "txt_hscnote");
	data->txt[txt_newhsc] = getValue_charp(fh, "txt_newhsc");
	data->txt[txt_gameover] = getValue_charp(fh, "txt_gameover");
	data->txt[txt_askquit] = getValue_charp(fh, "txt_askquit");
	data->txt[txt_askreplay] = getValue_charp(fh, "txt_askreplay");
	data->txt[txt_pause] = getValue_charp(fh, "txt_pause");
	data->txt[txt_askquitrep] = getValue_charp(fh, "txt_askquitrep");
	data->txt[txt_askrepagain] = getValue_charp(fh, "txt_askrepagain");
	
	/*
	 * Menu messages
	 */
	data->msg[msg_newgame] = getValue_charp(fh, "msg_newgame");
	data->msg[msg_options] = getValue_charp(fh, "msg_options");
	data->msg[msg_highscores] = getValue_charp(fh, "msg_highscores");
	data->msg[msg_replays] = getValue_charp(fh, "msg_replays");
	data->msg[msg_quit] = getValue_charp(fh, "msg_quit");
	
	data->msg[msg_back] = getValue_charp(fh, "msg_back");
	
	data->msg[msg_startgame] = getValue_charp(fh, "msg_startgame");
	data->msg[msg_players] = getValue_charp(fh, "msg_players");
	data->msg[msg_mplives] = getValue_charp(fh, "msg_mplives");
	data->msg[msg_recreplay] = getValue_charp(fh, "msg_recreplay");
	data->msg[msg_configplayers] = getValue_charp(fh, "msg_configplayers");
	
	data->msg[msg_player] = getValue_charp(fh, "msg_player");
	data->msg[msg_name] = getValue_charp(fh, "msg_name");
	data->msg[msg_leftkey] = getValue_charp(fh, "msg_leftkey");
	data->msg[msg_rightkey] = getValue_charp(fh, "msg_rightkey");
	data->msg[msg_jumpkey] = getValue_charp(fh, "msg_jumpkey");
	
	data->msg[msg_addfolder] = getValue_charp(fh, "msg_addthemefolder");
	data->msg[msg_deletefolder] = getValue_charp(fh, "msg_deletefolder");
	data->msg[msg_editfolder] = getValue_charp(fh, "msg_editfolder");
	
	data->msg[msg_themes] = getValue_charp(fh, "msg_themes");
	data->msg[msg_lang] = getValue_charp(fh, "msg_lang");
	data->msg[msg_gameoptions] = getValue_charp(fh, "msg_gameoptions");
	data->msg[msg_graphicoptions] = getValue_charp(fh, "msg_graphicoptions");
	data->msg[msg_soundoptions] = getValue_charp(fh, "msg_soundoptions");
	data->msg[msg_folders] = getValue_charp(fh, "msg_folders");
	
	data->msg[msg_themefolders] = getValue_charp(fh, "msg_themefolders");
	data->msg[msg_langfolders] = getValue_charp(fh, "msg_langfolders");
	data->msg[msg_repfolders] = getValue_charp(fh, "msg_repfolders");
	data->msg[msg_repsavefolder] = getValue_charp(fh, "msg_repsavefolder");
	
	data->msg[msg_fpslimit] = getValue_charp(fh, "msg_fpslimit");
	data->msg[msg_jumpingrot] = getValue_charp(fh, "msg_jumpingrot");
	data->msg[msg_scrollmode] = getValue_charp(fh, "msg_scrollmode");
	
	data->msg[msg_opengl] = getValue_charp(fh, "msg_opengl");
	data->msg[msg_bpp] = getValue_charp(fh, "msg_bpp");
	data->msg[msg_fullscreen] = getValue_charp(fh, "msg_fullscreen");
	data->msg[msg_antialiasing] = getValue_charp(fh, "msg_antialiasing");
	
	data->msg[msg_sndvolume] = getValue_charp(fh, "msg_sndvolume");
	data->msg[msg_musvolume] = getValue_charp(fh, "msg_musvolume");
	
	data->msg[msg_repname] = getValue_charp(fh, "msg_repname");
	data->msg[msg_repcomment] = getValue_charp(fh, "msg_repcomment");
	data->msg[msg_repplay] = getValue_charp(fh, "msg_repplay");
	data->msg[msg_repsave] = getValue_charp(fh, "msg_repsave");
	data->msg[msg_cancel] = getValue_charp(fh, "msg_cancel");
	
	/*
	 * Menu tips
	 */
	data->tip[tip_newgame] = getValue_charp(fh, "tip_newgame");
	data->tip[tip_options] = getValue_charp(fh, "tip_options");
	data->tip[tip_highscores] = getValue_charp(fh, "tip_highscores");
	data->tip[tip_replays] = getValue_charp(fh, "tip_replays");
	data->tip[tip_quit] = getValue_charp(fh, "tip_quit");
	
	data->tip[tip_back] = getValue_charp(fh, "tip_back");
	
	data->tip[tip_startgame] = getValue_charp(fh, "tip_startgame");
	data->tip[tip_players] = getValue_charp(fh, "tip_players");
	data->tip[tip_mplives] = getValue_charp(fh, "tip_mplives");
	data->tip[tip_recreplay] = getValue_charp(fh, "tip_recreplay");
	data->tip[tip_configplayers] = getValue_charp(fh, "tip_configplayers");
	
	data->tip[tip_player] = getValue_charp(fh, "tip_player");
	data->tip[tip_name] = getValue_charp(fh, "tip_name");
	data->tip[tip_leftkey] = getValue_charp(fh, "tip_leftkey");
	data->tip[tip_rightkey] = getValue_charp(fh, "tip_rightkey");
	data->tip[tip_jumpkey] = getValue_charp(fh, "tip_jumpkey");
	
	data->tip[tip_addfolder] = getValue_charp(fh, "tip_addthemefolder");
	data->tip[tip_folder] = getValue_charp(fh, "tip_themefolder");
	data->tip[tip_deletefolder] = getValue_charp(fh, "tip_deletefolder");
	data->tip[tip_editfolder] = getValue_charp(fh, "tip_editfolder");
	data->tip[tip_writefolder] = getValue_charp(fh, "tip_writefolder");
	
	data->tip[tip_themes] = getValue_charp(fh, "tip_themes");
	data->tip[tip_lang] = getValue_charp(fh, "tip_lang");
	data->tip[tip_gameoptions] = getValue_charp(fh, "tip_gameoptions");
	data->tip[tip_graphicoptions] = getValue_charp(fh, "tip_graphicoptions");
	data->tip[tip_soundoptions] = getValue_charp(fh, "tip_soundoptions");
	data->tip[tip_folders] = getValue_charp(fh, "tip_folders");
	
	data->tip[tip_themefolders] = getValue_charp(fh, "tip_themefolders");
	data->tip[tip_langfolders] = getValue_charp(fh, "tip_langfolders");
	data->tip[tip_repfolders] = getValue_charp(fh, "tip_repfolders");
	data->tip[tip_repsavefolder] = getValue_charp(fh, "tip_repsavefolder");
	
	data->tip[tip_fpslimit] = getValue_charp(fh, "tip_fpslimit");
	data->tip[tip_jumpingrot] = getValue_charp(fh, "tip_jumpingrot");
	data->tip[tip_scrollmode] = getValue_charp(fh, "tip_scrollmode");
	
	data->tip[tip_opengl] = getValue_charp(fh, "tip_opengl");
	data->tip[tip_bpp] = getValue_charp(fh, "tip_bpp");
	data->tip[tip_fullscreen] = getValue_charp(fh, "tip_fullscreen");
	data->tip[tip_antialiasing] = getValue_charp(fh, "tip_antialiasing");

	data->tip[tip_sndvolume] = getValue_charp(fh, "tip_sndvolume");
	data->tip[tip_musvolume] = getValue_charp(fh, "tip_musvolume");

	data->tip[tip_repname] = getValue_charp(fh, "tip_repname");
	data->tip[tip_repcomment] = getValue_charp(fh, "tip_repcomment");
	data->tip[tip_repplay] = getValue_charp(fh, "tip_repplay");
	data->tip[tip_repsave] = getValue_charp(fh, "tip_repsave");
	data->tip[tip_cancel] = getValue_charp(fh, "tip_cancel");
	
	/* MENU OPTIONS */
	data->opt[opt_40fps] = getValue_charp(fh, "opt_40fps");
	data->opt[opt_100fps] = getValue_charp(fh, "opt_100fps");
	data->opt[opt_300fps] = getValue_charp(fh, "opt_300fps");
	data->opt[opt_nolimit] = getValue_charp(fh, "opt_nolimit");
	
	data->opt[opt_norot] = getValue_charp(fh, "opt_norot");
	data->opt[opt_orginalrot] = getValue_charp(fh, "opt_orginalrot");
	data->opt[opt_fullrot] = getValue_charp(fh, "opt_fullrot");
	
	data->opt[opt_softscroll] = getValue_charp(fh, "opt_softscroll");
	data->opt[opt_hardscroll] = getValue_charp(fh, "opt_hardscroll");
	
	data->opt[opt_8bpp] = getValue_charp(fh, "opt_8bpp");
	data->opt[opt_16bpp] = getValue_charp(fh, "opt_16bpp");
	data->opt[opt_24bpp] = getValue_charp(fh, "opt_24bpp");
	data->opt[opt_32bpp] = getValue_charp(fh, "opt_32bpp");
	data->opt[opt_autobpp] = getValue_charp(fh, "opt_autobpp");
	
	data->opt[opt_on] = getValue_charp(fh, "opt_on");
	data->opt[opt_off] = getValue_charp(fh, "opt_off");
	
    fclose(fh);
	
    return TRUE;
}
