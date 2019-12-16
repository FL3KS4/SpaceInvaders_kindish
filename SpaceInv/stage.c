#include "stage.h"

static void logic(void);
static void draw(void);
static void initPlayer(void);
static void fireBullet(void);
static void doPlayer(void);
static void doEnemies(void);
static void doShips(void);
static void doBullets(void);
static void drawShips(void);
static void drawBullets(void);
static void spawnEnemies(void);
static int bulletHitShip(Entity *b);
static void fireEnemyBullet(Entity *b);
static void resetStage(void);
static void resetPlayer(void);
static void drawHUD(void);

static Entity *player;
static SDL_Texture *playerTexture;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyTexture;
static SDL_Texture *enemyBulletTexture;
static int enemySpawn;
static int stageReset;
static int highscore;

void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;
	
	memset(&stage, 0, sizeof(Stage));
	stage.fighterTail = &stage.fighterHead;
	stage.bulletTail = &stage.bulletHead;
	
	playerTexture = loadTexture("ship.png");
	bulletTexture = loadTexture("missile.png");
	enemyTexture = loadTexture("alien.png");
	enemyBulletTexture = loadTexture("enemy_missile.png");

	FILE* fin = fopen("highscore.txt", "r");

	if (fin != NULL)
	{
		fscanf(fin, "%d", &highscore);
	}

	fclose(fin);
	resetStage();
}

static void resetStage(void)
{
	Entity *e;

	while (stage.fighterHead.next)
	{
		e = stage.fighterHead.next;
		stage.fighterHead.next = e->next;
		free(e);
	}

	while (stage.bulletHead.next)
	{
		e = stage.bulletHead.next;
		stage.bulletHead.next = e->next;
		free(e);
	}

	memset(&stage, 0, sizeof(Stage));
	stage.fighterTail = &stage.fighterHead;
	stage.bulletTail = &stage.bulletHead;

	initPlayer();

	enemySpawn = 0;

	stage.score = 0;

	stageReset = FPS * 2;
}

static void initPlayer()
{
	player = malloc(sizeof(Entity));
	memset(player, 0, sizeof(Entity));
	stage.fighterTail->next = player;
	stage.fighterTail = player;
	
	player->health = 5;
	player->x = SCREEN_WIDTH /2 - 50;
	player->y = SCREEN_HEIGHT - 100;
	player->side = SIDE_PLAYER;
	player->texture = playerTexture;

	SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
}

static void logic(void)
{
	doPlayer();

	doShips();

	doEnemies();
	
	doBullets();

	spawnEnemies();

	resetPlayer();

	if (player == NULL && --stageReset <= 0)
	{
		resetStage();
	}
}


static void doPlayer(void)
{
	//Controls of your ship
	if (player != NULL)
	{
		player->dx = player->dy = 0;

		if (player->reload > 0)
		{
			player->reload--;
		}

		if (app.keyboard[SDL_SCANCODE_ESCAPE])
		{
			exit(0);
		}
		/*if (app.keyboard[SDL_SCANCODE_UP])
		{
			player->dy = -PLAYER_SPEED;
		}

		if (app.keyboard[SDL_SCANCODE_DOWN])
		{
			player->dy = PLAYER_SPEED;
		}*/

		if (app.keyboard[SDL_SCANCODE_LEFT])
		{
			player->dx = -PLAYER_SPEED;
			if (player->x <= 0)
			{
				player->x = 0;
			}
		}

		if (app.keyboard[SDL_SCANCODE_RIGHT])
		{
			player->dx = PLAYER_SPEED;
			if (player->x >= SCREEN_WIDTH - 100)
			{
				player->x = SCREEN_WIDTH - 100;
			}
		}

		if (app.keyboard[SDL_SCANCODE_SPACE] && player->reload == 0)
		{
			fireBullet();
		}

		player->x += player->dx;
		player->y += player->dy;
	}
}

static void doEnemies(void)
{
	Entity *e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (e != player && player != NULL && --e->reload <= 0)
		{
			fireEnemyBullet(e);
		}
	}
}

static void fireBullet(void)
{
	Entity *bullet;
	
	bullet = malloc(sizeof(Entity));
	memset(bullet, 0, sizeof(Entity));
	stage.bulletTail->next = bullet;
	stage.bulletTail = bullet;
	
	//Setting bullet for player
	bullet->x = player->x + 41;
	bullet->y = player->y;
	bullet->dy = -PLAYER_BULLET_SPEED;
	bullet->health = 1;
	bullet->side = SIDE_PLAYER;
	bullet->texture = bulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);
	
	bullet->y += (player->h / 2) - (bullet->h / 2);
	
	//Change for faster or slower shooting
	player->reload = 16;
}

static void fireEnemyBullet(Entity *e)
{
	Entity *bullet;

	bullet = malloc(sizeof(Entity));
	memset(bullet, 0, sizeof(Entity));
	stage.bulletTail->next = bullet;
	stage.bulletTail = bullet;

	//Setting bullet position and texture for enemy bullets
	bullet->x = e->x;
	bullet->y = e->y;
	bullet->dy = ENEMY_BULLET_SPEED;
	bullet->health = 1;
	bullet->texture = enemyBulletTexture;
	bullet->side = e->side;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

	bullet->x += (e->w / 2) - (bullet->w / 2);
	bullet->y += (e->h / 2) - (bullet->h / 2);


	bullet->dx *= ENEMY_BULLET_SPEED;
	bullet->dy *= ENEMY_BULLET_SPEED;

	bullet->side = SIDE_ENEMY;

	e->reload = 300;
}
	
static void doShips()
{
	Entity *e, *prev;

	prev = &stage.fighterHead;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		e->x += e->dx;
		e->y += e->dy;
		//Enemy ship passing your screen 
		if (e != player && e->y > SCREEN_HEIGHT)
		{
			e->health = 0;
			player->health -= 1;
		}
		//reset when you died
		if (e->health == 0)
		{
			if (e == player)
			{
				player = NULL;
			}

			if (e == stage.fighterTail)
			{
				stage.fighterTail = prev;
			}

			prev->next = e->next;
			free(e);
			e = prev;
		}

		prev = e;
	}
}

static void doBullets(void)
{
	Entity *b, *prev;

	prev = &stage.bulletHead;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		b->x += b->dx;
		b->y += b->dy;

		//SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%f ", b->y);
		if (bulletHitShip(b) || b->x < -b->w || b->y <= 0 || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT)
		{
			
			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}

		prev = b;
	}
}

static int bulletHitShip(Entity *b)
{
	//Bullet collision + saving score to file
	FILE *fout = fopen("highscore.txt", "w");
	char buffer[10];
	Entity *e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h))
		{
			b->health = 0;
			e->health -= 1;

			if (e != player)
			{
				stage.score += 10;
				if (stage.score > highscore)
				{
					highscore = stage.score;
					sprintf(buffer, "%d", highscore);
					fputs(buffer, fout);
				}
			}
			return 1;
		}
	}

	
	fclose(fout);
	return 0;
}



static void spawnEnemies()
{
	Entity *enemy;
	int col;

	if (--enemySpawn <= 0)
	{	
		enemy = malloc(sizeof(Entity));
		memset(enemy, 0, sizeof(Entity));
		stage.fighterTail->next = enemy;
		stage.fighterTail = enemy;

		//Enemy are going to spawn in screen area
		col = rand() % SCREEN_WIDTH;
		
		if (col >= SCREEN_WIDTH - 100)
			col = SCREEN_WIDTH - 100;

		//Enemy spawn + applying texture
		enemy->x = col;
		enemy->y = -100;
		enemy->texture = enemyTexture;
		enemy->side = SIDE_ENEMY;
		enemy->health = 1;
		SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

		//Speed of enemy ships
		enemy->dy = 1.25;

		
		//Difficulty grows over your performance
		if (stage.score == 0 || stage.score < 100)
		{
			enemySpawn = 150;
		}
		else if (stage.score == 100 || stage.score < 250)
		{
			enemySpawn = 115;
		}
		else if (stage.score == 250 || stage.score < 750)
		{
			enemySpawn = 75;
		}
		else
		{
			enemySpawn = 50;
		}
		enemy->reload = 100;


		
	}
}

static void resetPlayer(void)
{
	if (player != NULL)
	{
		if (player->x <= 0)
		{
			player->x = 0;
		}


		if (player->x >= SCREEN_WIDTH - 100)
		{
			player->x = SCREEN_WIDTH - 100;
		}
	}
}


//drawing functions
static void draw(void)
{
	drawShips();
	
	drawBullets();

	drawHUD();
}

static void drawBullets(void)
{
	Entity *b;
	
	for (b = stage.bulletHead.next ; b != NULL ; b = b->next)
	{
		applyTexture(b->texture, b->x, b->y);
	}
}

static void drawShips(void)
{
	Entity *e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		applyTexture(e->texture, e->x, e->y);
	}
}



static void drawHUD(void)
{
	

	drawText(10, 10, 255, 255, 255, "SCORE: %05d", stage.score);
	if (player != NULL)
	{
		drawText(400, 10, 255, 0, 0, "LIVES %d", player->health);
	}
	else
		drawText(400, 10, 255, 0, 0, "LIVES 0");

	if (stage.score > 0 && stage.score == highscore)
	{
		drawText(720, 10, 0, 255, 0, "HIGH SCORE: %05d", highscore);
	}
	else
	{
		drawText(720, 10, 255, 255, 255, "HIGHSCORE: %05d", highscore);
	}
}

