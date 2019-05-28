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

// Very Light - Light - Medium - Heavy (shifted compared to the usual notations)
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

#define FORWARD_TECH	197 
#define BACKWARD_TECH	198 
#define NEUTRAL_TECH	199 

/* Keymaps of both players */
#define SWRS_ADDR_1PKEYMAP 0x00898940
#define SWRS_ADDR_2PKEYMAP 0x0089912C

enum SAVESTATE_MODE {
	NONE,
	CONSERVED,
	CONTINUED
};

namespace KeymapIndex { enum KeymapIndex : int { up, down, left, right, A, B, C, D, sw, sc }; }
enum tech_select : int { neutral, left, right, random };
enum reversal_select : int { nothing, jump, highjump, backdash, mash4A,  d623B, spellcard };
enum first_select : int { noblocking, standing, crouching };
enum BE_select : int { noBE, BEdown, BEdownside, BEside, BEback };

/* KEYS */
typedef struct {
	UINT save_pos;
	UINT reset_pos;
	UINT display_states;
	UINT randomCH;
	UINT reset_skills;
	UINT tech_macro;
	UINT wakeup_macro;
	UINT firstblock_macro;
	UINT BE_macro;
} Keys;


typedef struct {
	bool display_states;
	bool save_pos;
	bool reset_pos;
	bool randomCH;
	bool reset_skills;
	int tech_macro;
	int wakeup_macro;
	int firstblock_macro;
	int BE_macro;
} Toggle_key;

typedef struct {
	bool set_pos;
	bool save_pos;
	bool tech_macro;
	bool wakeup_macro;
	bool firstblock_macro;
	bool BE_macro;
} Held_key;

/* PLAYER */
typedef struct {
	int frame_advantage;
	bool blockstring;
	int hjc_advantage;
	bool hjc_blockstring;
	int isIdle;
	bool untight_nextframe;

	bool already_CH;
	int tech_mode;
	int wakeup_mode;
	int firstblock_mode;
	int BE_mode;

	int wakeup_count_p1;
	int wakeup_count_p2;
} Misc_state;

typedef struct {
	int up;
	int down;
	int left;
	int right;
	int A;
	int B;
	int C;
	int D;
	int sw;
	int sc;
} Commands;

typedef struct {
	float x;
	float y;
	float xspeed;
	float yspeed;
	float gravity;
	int direction;
} Position;

typedef struct {
	void *p;
	int index;

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

extern Keys savestate_keys;
extern Toggle_key toggle_keys;
extern Held_key held_keys;
extern Misc_state misc_states;


/* UPDATE FUNCTIONS */
void update_position(Player *);
void update_playerinfo(Player *, int, int);

/* POSITION FUNCTIONS */
Position init_pos(float);
Position save_checkpoint(Player *);
void set_position(Player *, Position, int);
void position_management(Player *, Player *);

/* FRAMECOUNT FUNCTIONS */
void gap_count(Player *);
void frameadvantage_count(Player *, Player *);
void hjcadvantage_count(Player *, Player *);
bool untight_check(Player *);
void is_tight(Player *);

/* MACROS */
void random_CH(Player *);
void block_1st_hit(Player *, int *);
void send_inputs(Commands, Commands);
void tech(Player *, int *, int *);
void macros(Player *, Player *);

/* MISCELLANEOUS */
void state_display(Player *);
void set_health(Player *, short);
void set_spirit(Player *, short, short);
void reset_skills(Player *);