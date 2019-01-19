#if !defined DRAGONSHIP_H
#define DRAGONSHIP_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#define WIDTH 900
#define HEIGHT 900

#define WIDTH_GAME 640

struct Dragon 
{
	int 	x;							// Position on the x-axis of the dragon
	int 	y;							// Position on the y-axis of the dragon
	int 	lives;						// Player's lives
	int 	speed;						// Dragon's speed
	int 	boundx;						// Dragon's bound x-axis and it starts from centre
	int 	boundy;						// Dragon's bound y-axis and it starts from centre
	int 	score;						// Player's score

	int 	maxFrame;					// Max number of dragon's frame
	int 	curFrame;					// Current number of dragon's frame
	int 	frameCount;					// When frameCount is greater than frameDelay update curFrame
	int 	frameDelay;
	int 	frameWidth;					// Width of the image	
	int 	frameHeight;				// Height of the image
	int 	animationDirection;			// Direction of animation

	ALLEGRO_BITMAP 	*image;				// Dragon sprite
};

struct Fire 
{
	int 	x;							// Position on the x-axis of the fireball
	int 	y;							// Position on the y-axis of the fireball
	bool 	live;						// True if fireball is alive else false
	int 	speed;						// Fireball speed
	int 	boundx;						// Fireball's bound x-axis and it starts from centre
	int 	boundy;						// Fireball's bound y-axis and it starts from centre

	int 	maxFrame;					// Max number of fireball's frame
	int 	curFrame;					// Current number of fireball's frame
	int 	frameCount;					// When frameCount is greater than frameDelay update curFrame
	int 	frameDelay;
	int 	frameWidth;					// Width of the image	
	int 	frameHeight;				// Height of the image
	int 	animationDirection;			// Direction of animation

	ALLEGRO_BITMAP 	*image;				// Fireball sprite
};

struct Ship 
{
	int 	x;							// Position on the x-axis of the ship
	int 	y;							// Position on the y-axis of the ship
	bool 	live;						// True if sihip is alive else false
	int 	speed;						// Ship speed
	int 	boundx;						// Ship's bound x-axis and it starts from centre
	int 	boundy;						// Ship's bound y-axis and it starts from centre
	
	int 	frameWidth;					// Width of the image	
	int 	frameHeight;				// Height of the image

	ALLEGRO_BITMAP 	*image;				// Ship image
};

struct Explosion 
{
	int 	x;						// Position on the x-axis of the explosion
	int 	y;						// Position on the y-axis of the explosion
	bool 	live;					// True if explosion lives else false

	int 	maxFrame;				// Max number of explosion's frame
	int 	curFrame;				// current number of explosion's frame
	int 	frameCount;				// When frameCount is greater than frameDelay update curFrame
	int 	frameDelay;
	int 	frameWidth;				// Width of the image	
	int 	frameHeight;			// Height of the image
	int 	animationDirection;		// Direction of animation

	ALLEGRO_BITMAP 	*image;			// Explosion sprite
};

void InitDragon(Dragon &dragon, ALLEGRO_BITMAP *image);
void DrawDragon(Dragon &dragon);
void UpdateDragon(Dragon &dragon);
void MoveDragonUp(Dragon &dragon);
void MoveDragonDown(Dragon &dragon);
void MoveDragonRight(Dragon &dragon);
void MoveDragonLeft(Dragon &dragon);

void InitFire(Fire fire[], int size, ALLEGRO_BITMAP *image);
void DrawFire(Fire fire[], int size);
void FireFire(Fire fire[], int size, Dragon &dragon);
void UpdateFire(Fire fire[], int size);
void CollideFire(Fire fire[], int fSize, Ship ships[], int sSize, Dragon &dragon, Explosion explosions[], int eSize);


void InitShips(Ship ships[], int size, ALLEGRO_BITMAP *image);
void DrawShips(Ship ships[], int size);
void StartShips(Ship ships[], int size);
void UpdateShips(Ship ships[], int size);
void CollideShips(Ship ships[], int sSize, Dragon &dragon, Explosion explosions[], int eSize);

void InitExplosions(Explosion explosions[], int size, ALLEGRO_BITMAP *image);
void DrawExplosions(Explosion explosions[], int size);
void StartExplosions(Explosion explosions[], int size, int x, int y);
void UpdateExplosions(Explosion explosions[], int size);

#endif