#include "Includs.h"

extern void applyTexture(SDL_Texture *texture, int x, int y);
extern int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
extern void applyTextureF(SDL_Texture *texture, SDL_Rect *src, int x, int y);
void drawText(int x, int y, int r, int g, int b, char *format, ...);
extern SDL_Texture *loadTexture(char *filename);

extern App app;
extern Stage stage;
