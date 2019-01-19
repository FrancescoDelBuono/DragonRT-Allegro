#include "DragonShip.h"

// Initialize dragon variable
void InitDragon(struct Dragon &dragon, ALLEGRO_BITMAP *image) 
{
	dragon.x = WIDTH_GAME / 2;
	dragon.y = HEIGHT - 65;
	dragon.lives = 3;
	dragon.speed = 7;
	dragon.boundx = 25;
	dragon.boundy = 35;
	dragon.score = 0;

	dragon.maxFrame = 9;
	dragon.curFrame = 0;
	dragon.frameCount = 0;
	dragon.frameDelay = 6;
	dragon.frameWidth = 128;
	dragon.frameHeight = 128;
	dragon.animationDirection = 1;

	dragon.image = image;
}

// Draw dragon on the display
void DrawDragon(Dragon &dragon) 
{	
		int fx = (dragon.curFrame % dragon.maxFrame) * dragon.frameWidth;
		int fy = 0;
		
		al_draw_bitmap_region(dragon.image, fx, fy, dragon.frameWidth, 
			dragon.frameHeight, dragon.x - dragon.frameWidth / 2,
			dragon.y - dragon.frameHeight / 2, 0);
}

// Update dragon animation
void UpdateDragon(Dragon &dragon) 
{	
	if (++dragon.frameCount >= dragon.frameDelay) 
	{
		dragon.curFrame += dragon.animationDirection;
		if (dragon.curFrame >= dragon.maxFrame)
			dragon.curFrame = 0;
		dragon.frameCount = 0;			
	}	
}

void MoveDragonUp(Dragon &dragon) 
{
	dragon.y -= dragon.speed;
	if (dragon.y < 300)
		dragon.y = 300;
}

void MoveDragonDown(Dragon &dragon) 
{
	dragon.y += dragon.speed;
	if (dragon.y > HEIGHT)
		dragon.y = HEIGHT;
}

void MoveDragonRight(Dragon &dragon) 
{
	dragon.x += dragon.speed;
	if (dragon.x > WIDTH_GAME)
		dragon.x = WIDTH_GAME;
}

void MoveDragonLeft(Dragon &dragon) 
{
	dragon.x -= dragon.speed;
	if (dragon.x < 0)
		dragon.x = 0;
}

// Initialize fireballs variable
void InitFire(Fire fire[], int size, ALLEGRO_BITMAP *image) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		fire[i].speed = 8;
		fire[i].live = false;
		fire[i].boundx = 12;
		fire[i].boundy = 23;

		fire[i].maxFrame = 15;
		fire[i].curFrame = 0;
		fire[i].frameCount = 0;
		fire[i].frameDelay = 3;
		fire[i].frameWidth = 48;
		fire[i].frameHeight = 64;
		
		if (rand() % 2)
			fire[i].animationDirection = 1;
		else 
			fire[i].animationDirection = -1;

		fire[i].image = image;
	}
}

// Draw fireball on the display if fireball is alive
void DrawFire(Fire fire[], int size) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (fire[i].live) 
		{
			int fx = (fire[i].curFrame % fire[i].maxFrame) * fire[i].frameWidth;
			int fy = 0;

			al_draw_bitmap_region(fire[i].image, fx, fy, fire[i].frameWidth, 
				fire[i].frameHeight, fire[i].x - fire[i].frameWidth / 2,
				fire[i].y - fire[i].frameHeight / 2, 0);
		}
	}
}

// Shoot fireball, but there is a upper bound of the number of fireball in the screeen
void FireFire(Fire fire[], int size, Dragon &dragon) 
{
	int 	i;
	bool 	shoot = true;

	for (i = 0; i < size; i++) 
	{
		if (!fire[i].live && shoot == true) 
		{

			shoot = false;

			fire[i].curFrame = 0;
			fire[i].x = dragon.x;
			fire[i].y = dragon.y - dragon.boundy;
			fire[i].live = true;
		}
	}
}

// Move fireball, if fireball is alive and update the fireaball image
void UpdateFire(Fire fire[], int size) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (fire[i].live) 
		{
			fire[i].y -= fire[i].speed;
			
			if (++fire[i].frameCount >= fire[i].frameDelay) 
			{
				fire[i].curFrame += fire[i].animationDirection;
				if (fire[i].curFrame >= fire[i].maxFrame)
					fire[i].curFrame = 0;
				else if (fire[i].curFrame <= 0)
					fire[i].curFrame = fire[i].maxFrame - 1;

				fire[i].frameCount = 0;			
			}
			if (fire[i].y < 0)
				fire[i].live = false;
		}
	}
}

// Check if some fireball collides with some ship
void CollideFire(Fire fire[], int fSize, Ship ships[], int sSize, Dragon &dragon, Explosion explosions[], int eSize) 
{
	int 	i, j;

	for (i = 0; i < fSize; i++) 
	{
		if (fire[i].live) {
			for (j = 0; j < sSize; j++) 
			{
				if (ships[j].live) 
				{
					if (fire[i].x + fire[i].boundx > (ships[j].x - ships[j].boundx) && 
						fire[i].x - fire[i].boundx < (ships[j].x + ships[j].boundx) &&
						fire[i].y + fire[i].boundy > (ships[j].y - ships[j].boundy) &&
						fire[i].y - fire[i].boundy < (ships[j].y + ships[j].boundy)) 
					{
						
						fire[i].live = false;
						ships[j].live = false;

						dragon.score++;

						StartExplosions(explosions, eSize, ships[j].x, ships[j].y);
					}
				}
			}
		}
	}

}

// Initialize ship variable
void InitShips(Ship ships[], int size, ALLEGRO_BITMAP *image) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		ships[i].speed = 3;
		ships[i].live = false;
		ships[i].boundx = 25;
		ships[i].boundy = 40;

		ships[i].frameWidth = 128;
		ships[i].frameHeight = 128;

		ships[i].image = image; 
	}
}

// Draw ship on the display
void DrawShips(Ship ships[], int size) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (ships[i].live) 
		{
			al_draw_bitmap(ships[i].image, ships[i].x - ships[i].frameWidth / 2,
				ships[i].y - ships[i].frameHeight / 2, 0);
		}
	}
}

// Check if ship need to restart. Each ship start with a random delay
void StartShips(Ship ships[], int size) 
{
	bool 	start = true;
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (!ships[i].live && start == true) 
		{
			if  (rand() % 500 == 0) 
			{
				start = false;

				ships[i].live = true;
				ships[i].x = 30 + rand() % (WIDTH_GAME - 60);
				ships[i].y = 0;
			}
		}
	}
}

// Move ship, if ship is alive
void UpdateShips(Ship ships[], int size) {
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (ships[i].live) 
		{
			ships[i].y += ships[i].speed;
		}
	}
}

// Check if some ship collides with dragon
void CollideShips(Ship ships[], int sSize, Dragon &dragon, Explosion explosions[], int eSize) 
{
	int 	i;

	for (i = 0; i < sSize; i++) 
	{
		if (ships[i].live) 
		{
	
			if ((ships[i].x - ships[i].boundx) < (dragon.x + dragon.boundx) && 
				(ships[i].x + ships[i].boundx)  > (dragon.x - dragon.boundx) &&
				(ships[i].y - ships[i].boundy) < dragon.y + dragon.boundy  &&
				(ships[i].y + ships[i].boundy) > dragon.y - dragon.boundy) 
			{
				dragon.lives--;
				ships[i].live = false;
				StartExplosions(explosions, eSize, ships[i].x, ships[i].y);
			} 
			else if (ships[i].y > HEIGHT) 
			{
				dragon.lives--;
				ships[i].live = false;
			}
				
		}
			
	}
}

// Initialize explosion variable
void InitExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		explosions[i].live = false;

		explosions[i].maxFrame = 9;
		explosions[i].curFrame = 0;
		explosions[i].frameCount = 0;
		explosions[i].frameDelay = 4;
		explosions[i].frameWidth = 128;
		explosions[i].frameHeight = 128;
		
		explosions[i].animationDirection = 1;

		explosions[i].image = image;
	}
}

// Draw explosion on the display
void DrawExplosions(Explosion explosions[], int size) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (explosions[i].live) 
		{
			int fx = (explosions[i].curFrame % explosions[i].maxFrame) * explosions[i].frameWidth;
			int fy = 0;

			al_draw_bitmap_region(explosions[i].image, fx, fy, explosions[i].frameWidth, 
				explosions[i].frameHeight, explosions[i].x - explosions[i].frameWidth / 2,
				explosions[i].y - explosions[i].frameHeight / 2, 0);
		}
	}
}

// Start explosion animation
void StartExplosions(Explosion explosions[], int size, int x, int y) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (!explosions[i].live) 
		{
			explosions[i].live = true;
			explosions[i].x = x;
			explosions[i].y = y;
			break;
		}
	}
}

// Update explosion image to create animation
void UpdateExplosions(Explosion explosions[], int size) 
{
	int 	i;

	for (i = 0; i < size; i++) 
	{
		if (explosions[i].live) 
		{
			if (++explosions[i].frameCount >= explosions[i].frameDelay) 
			{
				explosions[i].curFrame += explosions[i].animationDirection;
				if (explosions[i].curFrame >= explosions[i].maxFrame) 
				{
					explosions[i].curFrame = 0;
					explosions[i].live = false;
				}
				explosions[i].frameCount = 0;
			}
		}
	}
}