#include <pthread.h>
#include <semaphore.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>

#include "DragonShip.h"

#define NUM_FIRE 5
#define NUM_SHIPS 10
#define NUM_EXPLOSIONS 5

enum 	KEYS {UP, DOWN, LEFT, RIGHT, SPACE, ENTER};
bool 	keys[6] = {false, false, false, false, false, false};

Dragon 	dragon;								// Player
Fire 	fire[NUM_FIRE];						// Fireballs
Ship 	ships[NUM_SHIPS];					// Enemy's ships
Explosion 	explosions[NUM_EXPLOSIONS];		// Explosion animation

// gestore includes variables and semaphores 
// to ensure mutability between display thread and update thread 
struct gestore_t
{
	sem_t 	m;
	sem_t 	seme;			// Semaphore for explosion thread
	sem_t 	sems;			// Semaphore for ship thread
	sem_t 	semf;			// Semaphore for fire thread
	sem_t 	semd;			// Semaphore for display thread (main)

	int 	be, bs, bd, bf;		// Count for block thread
	int 	ud, ue, us, uf;		// Variable for manage the comunication between thread
} gestore;

void InitGestore (gestore_t *g) 
{
	sem_init(&g->m, 0, 1);
	sem_init(&g->seme, 0, 0);
	sem_init(&g->sems, 0, 0);
	sem_init(&g->semd, 0, 0);
	sem_init(&g->semf, 0, 0);
	
	g->be = 0;
	g->bs = 0;
	g->bd = 0;
	g->bf = 0;
	g->ud = 0;
	g->us = 0;
	g->ue = 0;
	g->uf = 0;
}

// Initialize Allegro addons
void InitAllegro() 
{
	al_install_keyboard();

	al_init_primitives_addon();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

}

//----------------------------------------------------------
// Explosion task manage explosion animation, periodically
// it waits for the signal from the main thread and 
// updates the image of the explosion, if it exists
//----------------------------------------------------------
void *explosiontask (void *arg)
{
	while (1) 
	{
		sem_wait(&gestore.m);
		if(gestore.ue) 
		{
			sem_post(&gestore.seme);
		} else 
		{
			gestore.be++;
		}
		sem_post(&gestore.m);
		sem_wait(&gestore.seme);

		UpdateExplosions(explosions, NUM_EXPLOSIONS);
		
		sem_wait(&gestore.m);
		gestore.ud++;
		gestore.ue--;
		if (gestore.ud == 3 && gestore.bd == 1) 
		{
			gestore.bd--;
			sem_post(&gestore.semd);
		}
		sem_post(&gestore.m);
	}
}

//----------------------------------------------------------
// Ship task manage ship animation and movement, periodically
// it waits for the signal from the main thread and 
// move ship. If ship is not alive, with a random delay it restart
//----------------------------------------------------------
void *shiptask (void *arg)
{
	while (1) 
	{
		sem_wait(&gestore.m);
		if (gestore.us) 
		{
			sem_post(&gestore.sems);
		} else 
		{
			gestore.bs++;
		}
		sem_post(&gestore.m);
		sem_wait(&gestore.sems);

		StartShips(ships, NUM_SHIPS);
		UpdateShips(ships, NUM_SHIPS);

		sem_wait(&gestore.m);
		gestore.ud++;
		gestore.us--;
		if (gestore.ud == 3 && gestore.bd == 1) 
		{
			gestore.bd--;
			sem_post(&gestore.semd);
		}
		sem_post(&gestore.m);
	}
}

//----------------------------------------------------------
// Fire task manage fireball animation and movement, periodically
// it waits for the signal from the main thread and 
// updates the image of the fireball and move it, if it exists
//----------------------------------------------------------
void *firetask(void *arg)
{
	while (1) 
	{
		sem_wait(&gestore.m);
		if (gestore.uf) 
		{
			sem_post(&gestore.semf);
		} else 
		{
			gestore.bf++;
		}
		sem_post(&gestore.m);
		sem_wait(&gestore.semf);

		UpdateFire(fire, NUM_FIRE);

		sem_wait(&gestore.m);
		gestore.ud++;
		gestore.uf--;
		if (gestore.ud == 3 && gestore.bd == 1) 
		{
			gestore.bd--;
			sem_post(&gestore.semd);
		}
		sem_post(&gestore.m);
	}
}

int main(void) {
	
	int 	FPS = 60;					// Update frequency
	bool 	isGameOver = false;			// True if the player loses
	bool 	redraw = false;				// Variable to update the screen
	bool 	done = false;				// Player quit from game
	bool 	shoot = false;				// Player shoot with space
	bool 	startGame = true;			// PLayer press enter to start game

	pthread_attr_t 	a;		// Thread attribute
	pthread_t 	p;			// Thread id

	ALLEGRO_DISPLAY 	*display = NULL;			// Window ot the game
	ALLEGRO_EVENT_QUEUE 	*event_queue = NULL;	// Event queue
	ALLEGRO_TIMER 	*timer = NULL;					// Timer
	ALLEGRO_FONT 	*font18 = NULL;					// Font of the text
	ALLEGRO_BITMAP 	*dragonImage;					// Dragon image
	ALLEGRO_BITMAP 	*shipImage;						// Ship image
	ALLEGRO_BITMAP 	*expImage;						// Explosion sprite
	ALLEGRO_BITMAP 	*backgroundImage;				// Background
	ALLEGRO_BITMAP 	*fireImage;						// Fireball sprite

	srand(time(NULL));

	InitGestore(&gestore);

	if (!al_init())
		return -1;

	display = al_create_display(WIDTH, HEIGHT);
	if (!display)
		return -1;

	InitAllegro();

	event_queue = al_create_event_queue();
	timer = al_create_timer(1.0 / FPS);

	dragonImage = al_load_bitmap("image/dragon/sprite_sheet_dragon.bmp");
	al_convert_mask_to_alpha(dragonImage, al_map_rgb(106, 76, 48));

	shipImage = al_load_bitmap("image/ship/ship.bmp");
	al_convert_mask_to_alpha(shipImage, al_map_rgb(106, 76, 48));

	expImage = al_load_bitmap("image/ship/sprite_sheet_ship.png");
	al_convert_mask_to_alpha(expImage, al_map_rgb(106, 76, 48));

	fireImage = al_load_bitmap("image/fire/sprite_sheet_fire.bmp");
	al_convert_mask_to_alpha(fireImage, al_map_rgb(0, 0, 0));

	backgroundImage = al_load_bitmap("image/background02.bmp");

	InitDragon(dragon, dragonImage);
	InitFire(fire, NUM_FIRE, fireImage);
	InitShips(ships, NUM_SHIPS, shipImage);
	InitExplosions(explosions, NUM_EXPLOSIONS, expImage);

	font18 = al_load_font("font/arial.ttf", 18, 0);

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	al_draw_bitmap(backgroundImage, 0, 0, 0);

	pthread_attr_init(&a);

	pthread_create(&p, &a, explosiontask, NULL);
	pthread_create(&p, &a, shiptask, NULL);
	pthread_create(&p, &a, firetask, NULL);

	pthread_attr_destroy(&a);

	al_start_timer(timer);

	while (!done) 
	{
		// Wait for any event
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) 
		{
			done = true;
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) 
		{
			switch(ev.keyboard.keycode) 
			{
				case ALLEGRO_KEY_UP:
					keys[UP] = true;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = true;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = true;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = true;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = true;
					shoot = true;
					break;
				case ALLEGRO_KEY_ENTER:
					keys[ENTER] = true;
					break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) 
		{
			switch(ev.keyboard.keycode) 
			{
				case ALLEGRO_KEY_UP:
					keys[UP] = false;
					break;
				case ALLEGRO_KEY_DOWN:
					keys[DOWN] = false;
					break;
				case ALLEGRO_KEY_RIGHT:
					keys[RIGHT] = false;
					break;
				case ALLEGRO_KEY_LEFT:
					keys[LEFT] = false;
					break;
				case ALLEGRO_KEY_SPACE:
					keys[SPACE] = false;
					break;
				case ALLEGRO_KEY_ESCAPE:
					done = true;
					break;
				case ALLEGRO_KEY_ENTER:
					keys[ENTER] = false;
					break;
			}
		}
		else if (ev.type == ALLEGRO_EVENT_TIMER) 
		{
			redraw = true;

			if (keys[UP])
				MoveDragonUp(dragon);
			if (keys[DOWN])
				MoveDragonDown(dragon);
			if (keys[LEFT])
				MoveDragonLeft(dragon);
			if (keys[RIGHT])
				MoveDragonRight(dragon);

			if (keys[ENTER] && startGame) 
			{
				startGame = false;
				isGameOver = false;
				InitDragon(dragon, dragonImage);
				InitFire(fire, NUM_FIRE, fireImage);
				InitShips(ships, NUM_SHIPS, shipImage);
				InitExplosions(explosions, NUM_EXPLOSIONS, expImage);

			}

			// If it isn't game over, we update dragon image and eventually
			// shoot fireball, after  we send a signal to the 
			// explosion, ship and fire thread to update
			if (!isGameOver && !startGame) 
			{
				UpdateDragon(dragon);
				if (shoot) 
				{
					shoot = false;
					FireFire(fire, NUM_FIRE, dragon);
				}

				CollideFire(fire, NUM_FIRE,
				 ships, NUM_SHIPS, dragon, explosions, NUM_EXPLOSIONS);
				CollideShips(ships, NUM_SHIPS, dragon, explosions, NUM_EXPLOSIONS);


				sem_wait(&gestore.m);
				gestore.ud = 0;
				gestore.ue++;
				gestore.us++;
				gestore.uf++;
				if (gestore.be)
				{
					gestore.be--;
					sem_post(&gestore.seme);
				}
				if (gestore.bs) 
				{
					gestore.bs--;
					sem_post(&gestore.sems);
				}
				if (gestore.bf) 
				{
					gestore.bf--;
					sem_post(&gestore.semf);
				}
				sem_post(&gestore.m);
			}
		}
		
		if (redraw && al_is_event_queue_empty(event_queue)) 
		{
			redraw = false;

			// If it's game over, we wait the updates 
			// from the others thread and update the screeen
			if (!isGameOver && !startGame) 
			{
				if (dragon.lives <= 0) {
					isGameOver = true;
					startGame = true;
				}

				sem_wait(&gestore.m);
				if (gestore.ud == 3) 
				{
					sem_post(&gestore.semd);
				}
				else 
				{
					gestore.bd++;
				}
				sem_post(&gestore.m);
				sem_wait(&gestore.semd);

				DrawDragon(dragon);
				DrawFire(fire, NUM_FIRE);
				DrawShips(ships, NUM_SHIPS);
				DrawExplosions(explosions, NUM_EXPLOSIONS);

				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH_GAME + 10, 10, 0,
					"Lives: %i", dragon.lives);
				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH_GAME + 10, 40, 0,
					"Score: %i", dragon.score);
			}
			else if (isGameOver)
			{
				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH_GAME / 2, HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, 
					"GAME OVER");
				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH_GAME / 2, 30 + HEIGHT / 2 , ALLEGRO_ALIGN_CENTRE, 
					"FINAL SCORE: %i", dragon.score);
			}

			if (startGame) 
			{
				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH_GAME / 2, -100 + HEIGHT / 2, ALLEGRO_ALIGN_CENTRE, 
					"PRESS ENTER TO START");
				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH_GAME / 2, -70 + HEIGHT / 2 , ALLEGRO_ALIGN_CENTRE, 
					"UP, DOWN, RIGHT, LEFT to MOVE");
				al_draw_textf(font18, al_map_rgb(255, 255, 255), WIDTH_GAME / 2, -40 + HEIGHT / 2 , ALLEGRO_ALIGN_CENTRE, 
					"SPACE to SHOOT");
			}

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
			al_draw_bitmap(backgroundImage, 0, 0, 0);
		}
	}

	al_destroy_bitmap(backgroundImage);
	al_destroy_bitmap(fireImage);
	al_destroy_bitmap(expImage);
	al_destroy_bitmap(dragonImage);
	al_destroy_bitmap(shipImage);
	al_destroy_timer(timer);
	al_destroy_font(font18);
	al_destroy_event_queue(event_queue);
	al_destroy_display(display);

	return 0;
}

