#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "raylib.h"

#define MAX_PATHLEN 4096
#define MAX_BUTTONS 256
#define START_BUTTONS 128
#define SCREEN_HEIGHT 450
#define SCREEN_WIDTH 800
#define CHAR_WIDTH 12
#define TEXT_PADDING 10
#define BUTTON_MARGIN 10
#define BUTTON_HEIGHT 32

const char * DIR = "<path_to_this_project_root_dir>";
const char * SCRIPTS_DIR = "<path_to_where_scripts_are_stored>";

typedef struct Sprite {
  Texture2D texture;
  Rectangle rect;
  Rectangle dest;
  Vector2 position;
} Sprite;

typedef struct Button {
  Vector2 position;
  const char * text;
  const char * soundFile;
} Button;

unsigned buttons_size = 0;
unsigned num_buttons = 0;
Button *buttons = NULL;
int fileLoaded = 0;

Font customFont;
Sprite cursor;

// intialize buttons array
void initButtons(){
  buttons_size = START_BUTTONS;
  buttons = malloc(sizeof(Button) * buttons_size);
}

// Draw text with specified font
void drawText(Font f, const char* text, Vector2 position){
  DrawTextEx(f, text, position, BUTTON_HEIGHT, 0.0f, WHITE);
}

// Create button
void createButton(Button *b, char *buttonText, char *fileLocation, Vector2 position){
  b->position = position;
  b->text = buttonText;
  b->soundFile = fileLocation;
  ++num_buttons;
};

// Draw button
void drawButton(Button *b){
  int len = strlen(b->text);
  DrawRectangle(b->position.x, b->position.y, CHAR_WIDTH * len + TEXT_PADDING, BUTTON_HEIGHT, RED);
  drawText(customFont, TextToUpper(b->text), (Vector2){b->position.x + (float)(TEXT_PADDING / 2.0f), b->position.y});
}

// Draw Buttons array
void drawButtons(){
  for (int i = 0; i < num_buttons; i++){
    drawButton(&buttons[i]);
  }
}

// check mouse / button collision
bool isInButton(Button *b){
  int textSize = strlen(b->text) * CHAR_WIDTH + TEXT_PADDING;
  Rectangle r  = {b->position.x, b->position.y, textSize, BUTTON_HEIGHT};
  return CheckCollisionPointRec(GetMousePosition(), r);
}

// get button data from file
void loadConfig(const char* path){
  FILE *configFile = fopen(path, "r");
  char buffer[4096];
  int lineLength = 0;
  int lineNumber = 0;
  
  while(fgets(buffer, 4096, configFile) != NULL) {
      char buttonText[4096];
      char soundPath[4096];

      
      // get from line
      sscanf((const char *)buffer, "%4096s %4096s", buttonText, soundPath);
      
      // calc new button position
      int buttonLength = CHAR_WIDTH * strlen(buttonText) + TEXT_PADDING + BUTTON_MARGIN;
      lineLength += buttonLength;

      if (lineLength >= SCREEN_WIDTH){
	lineNumber++;
	lineLength = buttonLength;
      }
      
      int xPos = lineLength - buttonLength + BUTTON_MARGIN;
      
      Vector2 position = (Vector2){ xPos, (BUTTON_HEIGHT + BUTTON_MARGIN) * lineNumber + BUTTON_MARGIN };

      // copy and alloc on heap
      char *bText = strdup(buttonText);
      char *sPath = strdup(soundPath);

      // create button
      createButton(&buttons[num_buttons], bText, sPath, position);
      
  }
  
  
  if (configFile != NULL) fclose(configFile);
}

void init(){
  // Raylib stuff
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simple Sound Board");
  SetTargetFPS(60);
  InitAudioDevice();
  HideCursor();
  SetExitKey(KEY_NULL);

  // Engine stuff
  // setup custom font
  char pathBuf[MAX_PATHLEN];
  snprintf(pathBuf, MAX_PATHLEN, "%s/%s", DIR, "font.ttf");
  customFont = LoadFont((const char *) pathBuf);

  // setup cursor texture
  snprintf(pathBuf, MAX_PATHLEN, "%s/%s", DIR, "cursor.png");
  cursor = (Sprite){
    .texture = (Texture2D)LoadTexture((const char *)pathBuf),
    .position = (Vector2){ 0.0f, 0.0f },
    .rect = (Rectangle){ 0.0f, 0.0f, 16.0f, 16.0f },
    .dest = (Rectangle){ 0.0f, 0.0f, 64.0f, 64.0f },
  };  

  // init and load buttonsI 
  initButtons();
  snprintf(pathBuf, MAX_PATHLEN, "%s/%s", DIR, "config.ini");
  loadConfig((const char *)pathBuf);
}

extern char **environ;

int main(){
  init();

  char pBuf[MAX_PATHLEN];

  while (!WindowShouldClose()){
    cursor.dest.x = GetMousePosition().x - 12;
    cursor.dest.y = GetMousePosition().y;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
      for (int i = 0; i < num_buttons; i++){
	if (isInButton(&buttons[i])){
	  snprintf(pBuf, MAX_PATHLEN, "%s/%s", SCRIPTS_DIR, buttons[i].soundFile);
	  pid_t pid = fork();
	  
	  if (pid == 0){
	    char *args[] = {(char *)0};
	    int code = execve(pBuf, args, environ);
	    if (code == -1)
	      exit(0);
	  }
	  
	}
      }
    }

    BeginDrawing();
    ClearBackground(BLACK);
    drawButtons();
    DrawTexturePro(cursor.texture, cursor.rect, cursor.dest, cursor.position, 0.0f, WHITE);
    EndDrawing(); 
  }

  CloseWindow();
}
