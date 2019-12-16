#include "Includs.h"

extern void applyTextureF(SDL_Texture *texture, SDL_Rect *src, int x, int y);
void drawText(int x, int y, int r, int g, int b, char *format, ...);
extern void initStage(void);
extern SDL_Texture *loadTexture(char *filename);

extern App app;

extern App app;