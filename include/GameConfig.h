#ifndef GAMECONFIG_H
#define GAMECONFIG_H

// Global game configurations
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int FPS;
extern const int FRAME_DELAY;

// Asset paths
extern const char* BACKGROUND_IMG;
extern const char* FOREGROUND_IMG;
extern const char* CROSSHAIR_IMG;
extern const char* GUNSHOT_IMG;

extern const char* DOG_IDLE_IMG;
extern const char* DOG_DUCK_IMG;

// Duck animations
extern const char* DUCK_UD1_IMG;
extern const char* DUCK_UD2_IMG;
extern const char* DUCK_UD3_IMG;
extern const char* DUCK_F1_IMG;
extern const char* DUCK_F2_IMG;
extern const char* DUCK_F3_IMG;
extern const char* DUCK_SHOT_IMG;
extern const char* DUCK_FALL_IMG;
extern const char* DUCK_FALL_FLIPPED_IMG;

// Nega Duck animations
extern const char* NEGA_DUCK_SP_IMG;
extern const char* NEGA_DUCK_FLAP1_IMG;
extern const char* NEGA_DUCK_FLAP2_IMG;

// Sounds
extern const char* GUNSHOT_SND;
extern const char* QUACK_SND;
extern const char* MUSIC_SND;
extern const char* INTRO_SND;

// Font
extern const char* FONT_FILE;

// Game specific constants
extern const int FOREGROUND_Y_POS;
extern const int DOG_SPAWN_Y_START;
extern const int DOG_RELEASE_Y_TARGET;
extern const int DOG_RETREAT_Y_END;
extern const int DUCK_FLYAWAY_Y_LIMIT;

#endif // GAMECONFIG_H
