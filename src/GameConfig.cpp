#include "GameConfig.h" // Best practice to include the header in its own .cpp

// Global game configurations
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int FPS = 60;
const int FRAME_DELAY = 1000 / FPS;

// Asset paths
const char* BACKGROUND_IMG = "assets/Background.gif";
const char* FOREGROUND_IMG = "assets/Foreground.gif";
const char* CROSSHAIR_IMG = "assets/Crosshair.gif";
const char* GUNSHOT_IMG = "assets/Gunshot.gif";

const char* DOG_IDLE_IMG = "assets/DogIdle.gif";
const char* DOG_DUCK_IMG = "assets/DogDuck.gif";

// Duck animations
const char* DUCK_UD1_IMG = "assets/DuckUD1.gif";
const char* DUCK_UD2_IMG = "assets/DuckUD2.gif";
const char* DUCK_UD3_IMG = "assets/DuckUD3.gif";
const char* DUCK_F1_IMG = "assets/DuckF1.gif";
const char* DUCK_F2_IMG = "assets/DuckF2.gif";
const char* DUCK_F3_IMG = "assets/DuckF3.gif";
const char* DUCK_SHOT_IMG = "assets/DuckShot.gif";
const char* DUCK_FALL_IMG = "assets/DuckFall.gif";
const char* DUCK_FALL_FLIPPED_IMG = "assets/DuckFall.gif"; // Will be flipped in code

// Nega Duck animations
const char* NEGA_DUCK_SP_IMG = "assets/DarkwingSp.gif";
const char* NEGA_DUCK_FLAP1_IMG = "assets/DarkwingFlap.gif";
const char* NEGA_DUCK_FLAP2_IMG = "assets/DarkwingFlap2.gif";

// Sounds
const char* GUNSHOT_SND = "assets/Gunshot.ogg";
const char* QUACK_SND = "assets/Quack.ogg";
const char* MUSIC_SND = "assets/Music.ogg";
const char* INTRO_SND = "assets/Intro.ogg";

// Font
const char* FONT_FILE = "assets/HelveticaWorld.ttf";

// Game specific constants
const int FOREGROUND_Y_POS = 301;
const int DOG_SPAWN_Y_START = 440;
const int DOG_RELEASE_Y_TARGET = 200;
const int DOG_RETREAT_Y_END = 320;
const int DUCK_FLYAWAY_Y_LIMIT = 500;
