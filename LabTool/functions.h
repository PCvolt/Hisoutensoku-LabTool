#pragma once
#define LEFT_CORNER_P1 79.0f
#define	LEFT_NEAR_P1 379.0f
#define	MIDSCREEN_P1 601.0f
#define	RIGHT_NEAR_P1 901.0f
#define	RIGHT_CORNER_P1 1201.0f

#define	LEFT_CORNER_P2 40.0f
#define	LEFT_NEAR_P2 340.0f
#define	MIDSCREEN_P2 640.0f
#define	RIGHT_NEAR_P2 940.0f
#define	RIGHT_CORNER_P2 1201.0f


enum SAVESTATE_MODE {
	NONE,
	CONSERVED,
	CONTINUED
};

typedef struct {
	UINT reset;
	UINT save;
} Keys;

extern Keys savestate_keys;

typedef struct {
	float x;
	float y;
	float xspeed;
	float yspeed;
	float gravity;
} Position;

typedef struct {
	void *p;

	int x_pressed;//left is -, right is +
	int y_pressed;//down is -, up is +
	Position position;

	void* framedata;
	int frameflag;
	short current_frame;
	short current_sequence;

	short health;
	short spirit;

	short untech;
	char card;
} Player;


/* UPDATE FUNCTIONS */
void update_position(Player *);
void update_playerinfo(Player *, int, int);

/* POSITION FUNCTIONS */
Position init_pos(float);
Position save_checkpoint(Player *);
void set_position(Player *, Position, int);
void position_management(Player *, Player *);

/* FRAMECOUNT FUNCTIONS */
void trademash_count(Player *);
void frameadvantage_count(Player *, Player *);

/* MISCELLANEOUS */
void state_display(Player *);
void set_health(Player *, short);
void set_spirit(Player *, short, short);
void print_card(Player *);