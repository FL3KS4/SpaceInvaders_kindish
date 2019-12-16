#include "MainM.h"

static void logic(void);
static void draw(void);
static void drawMenu(void);

static SDL_Texture *menuTexture;
static int reveal = 0;
static int timeout;
static int highscore;

void initTitle(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

	menuTexture = loadTexture("menu.png");

	timeout = FPS * 5;
}

static void logic(void)
{

	if (reveal < SCREEN_HEIGHT)
	{
		reveal++;
	}

	if (--timeout <= 0)
	{
		
	}

	if (app.keyboard[SDL_SCANCODE_SPACE])
	{
		initStage();
	}
	if (app.keyboard[SDL_SCANCODE_ESCAPE])
	{
		exit(0);
	}
}

static void draw(void)
{
	drawMenu();

}

static void drawMenu(void)
{

	FILE* fin = fopen("highscore.txt", "r");

	if (fin != NULL)
	{
		fscanf(fin, "%d", &highscore);
	}

	SDL_Rect r;

	r.x = 0;
	r.y = 0;

	SDL_QueryTexture(menuTexture, NULL, NULL, &r.w, &r.h);

	r.h = min(reveal, r.h);

	applyTextureF(menuTexture, &r, (SCREEN_WIDTH / 2) - (r.w / 2), 100);

	drawText(720, 10, 255, 255, 255, "HIGHSCORE %05d", highscore);

	fclose(fin);
}