#pragma once
#define LEFT_CORNER_P1	79.0f
#define	LEFT_NEAR_P1	379.0f
#define	MIDSCREEN_P1	601.0f
#define	RIGHT_NEAR_P1	901.0f
#define	RIGHT_CORNER_P1 1201.0f

#define	LEFT_CORNER_P2	40.0f
#define	LEFT_NEAR_P2	340.0f
#define	MIDSCREEN_P2	640.0f
#define	RIGHT_NEAR_P2	940.0f
#define	RIGHT_CORNER_P2 1240.0f

#define VERYLIGHT_RB_TIME	10
#define LIGHT_RB_TIME		16
#define MEDIUM_RB_TIME		22
#define HEAVY_RB_TIME		28
#define VERYLIGHT_WB_TIME	12
#define LIGHT_WB_TIME		20
#define MEDIUM_WB_TIME		28
#define AIR_B_TIME			20

#define GUARDCRUSH		143 
#define AIR_GUARDCRUSH	145 
#define STAND_VL_RB		150 
#define STAND_L_RB		151 
#define STAND_M_RB		152 
#define STAND_H_RB		153 
#define CROUCH_VL_RB	154 
#define CROUCH_L_RB		155 
#define CROUCH_M_RB		156 
#define CROUCH_H_RB		157 
#define AIRBLOCK		158 
#define STAND_VL_WB		159 
#define STAND_L_WB		160 
#define STAND_M_WB		161 
#define CROUCH_L_WB		164 
#define CROUCH_M_WB		165 

enum SAVESTATE_MODE {
	NONE,
	CONSERVED,
	CONTINUED
};

typedef struct {
	UINT save_pos;
	UINT reset_pos;
	UINT display_states;
	UINT randomCH;
	UINT reset_skills;
} Keys;

typedef struct {
	bool save_pos;
	bool reset_pos;
	bool display_states;
	bool randomCH;
	bool reset_skills;
} Toggles;


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
	short current_sequence;
	short elapsed_in_subseq;

	short health;
	short spirit;

	short untech;
	char card;
} Player;


typedef struct {
	int A;
	int B;
	int C;
	int D;
	int Sw;
	int Sc;
	int Up;
	int Down;
	int Left;
	int Right;
} Commands;

extern Keys savestate_keys;
extern Toggles toggle_keys;
extern Commands commands;

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
void hjcadvantage_count(Player *, Player *);
bool untight_check(Player *);
void is_tight(Player *);

/* MACROS */
void random_CH(Player *);
void fixed_tech(Player *);

/* MISCELLANEOUS */
void state_display(Player *);
void set_health(Player *, short);
void set_spirit(Player *, short, short);
void reset_skills(Player *);