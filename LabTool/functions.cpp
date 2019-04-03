#pragma warning(disable:4996)
#define SWRS_USES_HASH
#include "stdafx.h"

//Personal header
#include "functions.h"

//SWRSToys headers
#include "swrs.h"
#include "fields.h"
#include "address.h"

//Standard C++ headers
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>

//Standard headers
#include <windows.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <vector>
#include <d3d9.h>
#include <sstream>



/* UPDATE */
void update_position(Player *player)
{
	player->position.x = ACCESS_FLOAT(player->p, CF_X_POS);
	player->position.y = ACCESS_FLOAT(player->p, CF_Y_POS);
	player->position.xspeed = ACCESS_FLOAT(player->p, CF_X_SPEED);
	player->position.yspeed = ACCESS_FLOAT(player->p, CF_Y_SPEED);
	player->position.gravity = ACCESS_FLOAT(player->p, CF_GRAVITY);
}

void update_playerinfo(Player *player, int battleManager, int add_bmgr_px)
{
	player->p = ACCESS_PTR(battleManager, add_bmgr_px);

	player->x_pressed = ACCESS_INT(player->p, CF_PRESSED_X_AXIS);
	player->y_pressed = ACCESS_INT(player->p, CF_PRESSED_Y_AXIS);
	update_position(player);

	player->health = ACCESS_SHORT(player->p, CF_CURRENT_HEALTH);
	player->spirit = ACCESS_SHORT(player->p, CF_CURRENT_SPIRIT);

	player->framedata = ACCESS_PTR(player->p, CF_CURRENT_FRAME_DATA);
	player->frameflag = ACCESS_INT(player->framedata, FF_FFLAGS);
	player->current_sequence = ACCESS_SHORT(player->p, CF_CURRENT_SEQ);
	player->elapsed_in_subseq = ACCESS_SHORT(player->p, CF_ELAPSED_IN_SUBSEQ);

	player->card = ACCESS_CHAR(player->p, CF_CARD_SLOTS);
	player->untech = ACCESS_SHORT(player->p, CF_UNTECH);
}


/* POSITION */
Position init_pos(float custom_x) {
	Position pos;
	pos.x = custom_x;
	pos.y = 0.0f;
	pos.xspeed = 0.0f;
	pos.yspeed = 0.0f;
	pos.gravity = 0.0f;

	return pos;
}

Position custom_pos = init_pos(600.0f);
Position custom_pos2 = init_pos(660.0f);
Position LC_P1 = init_pos(LEFT_CORNER_P1);
Position LC_P2 = init_pos(LEFT_CORNER_P2);
Position LN_P1 = init_pos(LEFT_NEAR_P1);
Position LN_P2 = init_pos(LEFT_NEAR_P2);
Position MID_P1 = init_pos(MIDSCREEN_P1);
Position MID_P2 = init_pos(MIDSCREEN_P2);
Position RN_P1 = init_pos(RIGHT_NEAR_P1);
Position RN_P2 = init_pos(RIGHT_NEAR_P2);
Position RC_P1 = init_pos(RIGHT_CORNER_P1);
Position RC_P2 = init_pos(RIGHT_CORNER_P2);

Position save_checkpoint(Player *player)
{
	Position pos;

	pos.x = player->position.x;
	pos.y = player->position.y;
	pos.xspeed = player->position.xspeed;
	pos.yspeed = player->position.yspeed;
	pos.gravity = player->position.gravity;

	return pos;
}

void set_position(Player *player, Position pos, int mode)
{
	ACCESS_FLOAT(player->p, CF_X_POS) = pos.x;
	ACCESS_FLOAT(player->p, CF_Y_POS) = pos.y;

	if (mode == NONE)
	{
		ACCESS_FLOAT(player->p, CF_X_SPEED) = 0.0f;
		ACCESS_FLOAT(player->p, CF_Y_SPEED) = 0.0f;
	}
	else if (mode == CONSERVED)//will move you in the opposite x if you change sides.
	{
		ACCESS_FLOAT(player->p, CF_X_SPEED) = pos.xspeed;
		ACCESS_FLOAT(player->p, CF_Y_SPEED) = pos.yspeed;
	}
	else if (mode == CONTINUED)
	{
		//not much happens
	}
}

bool held_save_key = false;
bool held_set_key = false;
void position_management(Player *p1, Player *p2) {
	if (GetKeyState(savestate_keys.save_pos) & 0x8000) {
		if (!held_save_key)
		{
			custom_pos = save_checkpoint(p1);
			custom_pos2 = save_checkpoint(p2);
			printf("Positions saved at: (%f, %f) VS (%f, %f)\n", custom_pos.x, custom_pos.y, custom_pos2.x, custom_pos2.y);
		}
		held_save_key = true;
	}
	else
	{
		held_save_key = false;
	}


	if (GetKeyState(savestate_keys.reset_pos) & 0x8000)
	{
		if (!held_set_key)
		{
			if (p1->y_pressed > 0)//DOWN
			{
				set_position(p1, MID_P1, NONE);
				set_position(p2, MID_P2, NONE);

				if (p1->x_pressed < 0)//LEFT+DOWN
				{
					set_position(p1, LN_P1, NONE);
					set_position(p2, LN_P2, NONE);
				}
				else if (p1->x_pressed > 0)//RIGHT+DOWN
				{
					set_position(p1, RN_P1, NONE);
					set_position(p2, RN_P2, NONE);
				}
			}
			else
			{
				if (p1->x_pressed < 0)//LEFT
				{
					set_position(p1, LC_P1, NONE);
					set_position(p2, LC_P2, NONE);
				}
				else if (p1->x_pressed > 0) //RIGHT
				{
					set_position(p1, RC_P1, NONE);
					set_position(p2, RC_P2, NONE);
				}
				else//NOTHING
				{
					if (custom_pos.y > 0)
					{
						ACCESS_SHORT(p1->p, CF_CURRENT_SEQ) = 9;
					} 
					if (custom_pos2.y > 0)
					{
						ACCESS_SHORT(p2->p, CF_CURRENT_SEQ) = 9;
					}//ISSUE: find a way to reset the character to a air animation

					if ((custom_pos.x < custom_pos2.x && p1->position.x > p2->position.x)
						|| (custom_pos.x > custom_pos2.x && p1->position.x < p2->position.x))
					{
						p1->position.xspeed = -p1->position.xspeed;
					}//ISSUE: save the direction from ground to air reset

					set_position(p1, custom_pos, CONSERVED);
					set_position(p2, custom_pos2, CONSERVED);
				}
			}
			held_set_key = true;
		}
	}
	else
	{
		held_set_key = false;
	}
}


/* FRAMECOUNT */
//bug to fix: if the opponent stands or crouches back, the f_a gets inverted (yuyuko 3A is -12F, not +12F)
int frame_advantage = 0;
bool blockstring = false;
void frameadvantage_count(Player *p1, Player *p2) {

	if (p1->current_sequence > 10 && p2->current_sequence > 10)
	{
		frame_advantage = 0;
		blockstring = true;
	}
	if ((p1->current_sequence < 10 || p2->current_sequence < 10) && blockstring)
	{
		if (p1->current_sequence < 10 && p2->current_sequence < 10)
		{ //both stay on the ground (works)
			blockstring = false;

			if (ACCESS_SHORT(p1->p, CF_ELAPSED_IN_SUBSEQ) < ACCESS_SHORT(p2->p, CF_ELAPSED_IN_SUBSEQ))
				std::cout << "P1 is -" << frame_advantage << "F" << std::endl << std::endl;
			else
				std::cout << "P1 is +" << frame_advantage << "F" << std::endl << std::endl;
		}

		++frame_advantage;
	}
}

int hjc_advantage = 0;
bool hjc_blockstring = false;
void hjcadvantage_count(Player *p1, Player *p2) {
	if (p1->current_sequence > 10 && p2->current_sequence > 10 && !hjc_blockstring)
	{//there was a frame both were acting, we entered a blockstring
		hjc_advantage = 0;
		hjc_blockstring = true;
	}
	if ((p1->current_sequence != 204 && p1->current_sequence >= 201 && p1->current_sequence <= 215) && hjc_blockstring)
	{//p2 is still blocking, p1 is high air
		
		if (p2->current_sequence < 10)
		{ //if p2 now recovered, we display
			hjc_blockstring = false;

			if (ACCESS_SHORT(p1->p, CF_ELAPSED_IN_SUBSEQ) < ACCESS_SHORT(p2->p, CF_ELAPSED_IN_SUBSEQ))
				std::cout << "P1 is [-" << hjc_advantage << "F]" << std::endl << std::endl;
			else
				std::cout << "P1 is [+" << hjc_advantage << "F]" << std::endl << std::endl;
		}
		++hjc_advantage;
	}	
}


int isIdle = -1;
void trademash_count(Player *player)
{
	if (player->frameflag & FF_GUARDING || !(player->frameflag & FF_GUARD_AVAILABLE))
	{
		if (isIdle != 0)
		{
			++isIdle;
			if (isIdle <= 30)
				std::cout << "Trademash: " << isIdle << "F" << std::endl;
		}
		isIdle = 0;
	}
	else
		++isIdle;
}



bool untight_nextframe = false;
bool untight_check(Player *player)
{
	//RIGHTBLOCK
	if (player->elapsed_in_subseq == VERYLIGHT_RB_TIME - 1 && (player->current_sequence == STAND_VL_RB || player->current_sequence == CROUCH_VL_RB))
		return true;
	else if (player->elapsed_in_subseq == LIGHT_RB_TIME - 1 && (player->current_sequence == STAND_L_RB || player->current_sequence == CROUCH_L_RB))
		return true;
	else if (player->elapsed_in_subseq == MEDIUM_RB_TIME - 1 && (player->current_sequence == STAND_M_RB || player->current_sequence == CROUCH_M_RB))
		return true;
	else if (player->elapsed_in_subseq == HEAVY_RB_TIME - 1 && (player->current_sequence == STAND_H_RB || player->current_sequence == CROUCH_H_RB))
		return true;
	//AIRBLOCK
	else if (player->elapsed_in_subseq == AIR_B_TIME - 1 && player->current_sequence == AIRBLOCK)
		return true;
	//WRONGBLOCK
	else if (player->elapsed_in_subseq == VERYLIGHT_WB_TIME - 1 && player->current_sequence == STAND_VL_WB)
		return true;
	else if (player->elapsed_in_subseq == LIGHT_WB_TIME - 1 && (player->current_sequence == STAND_L_WB || player->current_sequence == CROUCH_L_WB))
		return true;
	else if (player->elapsed_in_subseq == MEDIUM_WB_TIME - 1 && (player->current_sequence == STAND_M_WB || player->current_sequence == CROUCH_M_WB))
		return true;
	else
		return false;
}

void is_tight(Player *player)
{
	if (player->current_sequence >= 143 && player->current_sequence <= 165)
	{
		if (untight_nextframe)
		{ //bug in which the first frame of the first hit is considered untight
			std::cout << "Trademash: instant-F" << std::endl;
		}

		untight_nextframe = false;
		untight_nextframe = untight_check(player);
	}
	else
	{
		untight_nextframe = false;
	}
}

/* MACROS */
bool already_CH = false;
void random_CH(Player *player)
{

	if (GetAsyncKeyState(savestate_keys.randomCH) & 1)
		toggle_keys.randomCH = !toggle_keys.randomCH;
	
	if (toggle_keys.randomCH)
	{
		bool CH = false;
		int a = rand() % 2;
		if (a == 1)
			CH = true;


		if (player->frameflag & FF_GUARD_AVAILABLE)
		{//if not in CH state already and can guard (to prevent CH mid-combo)
			if (!already_CH)
			{
				if (CH)
				{
					ACCESS_INT(player->framedata, FF_FFLAGS) = ACCESS_INT(player->framedata, FF_FFLAGS) xor 64;
					already_CH = true;
				}
				else if (!CH)
				{
					already_CH = false;
				}
			}
			else if (already_CH)
			{
				if (!CH)
				{
					ACCESS_INT(player->framedata, FF_FFLAGS) = ACCESS_INT(player->framedata, FF_FFLAGS) xor 64;
					already_CH = false;
				}
				else if (CH)
				{
					already_CH = true;
				}
			}
		}
	}
	else if (ACCESS_INT(player->framedata, FF_FFLAGS) & FF_CH_ON_HIT)
	{
		ACCESS_INT(player->framedata, FF_FFLAGS) = ACCESS_INT(player->framedata, FF_FFLAGS) xor 64;
		already_CH = false;
	}
}


void fixed_tech(Player *player)
{/*
	if (player->frameflag & FF_DOWN)
	{
		ACCESS_INT(player->p, CF_PRESSED_X_AXIS1) = 1;
		//dstd::cout << ACCESS_INT(player->p, CF_PRESSED_X_AXIS1) << std::endl;
		ACCESS_INT(player->p, CF_PRESSED_D_1) = 1;
	}*/
}


/* MISCELLANEOUS */
void state_display(Player *player)
{
	int transparency = -1;
	int red = -1;
	int blue = -1;
	int green = -1;

	if (GetAsyncKeyState(savestate_keys.display_states) & 1)
		toggle_keys.display_states = !toggle_keys.display_states;

	if (toggle_keys.display_states)
	{
		if (player->frameflag & FF_GRAZE)
			transparency = 100;
		if (player->frameflag & FF_MELEE_INVINCIBLE)
		{
			red = 60;
			green = 60;
		}
		else if (player->frameflag & FF_CH_ON_HIT)
		{
			blue = 60;
			green = 60;
		}

		ACCESS_CHAR(player->p, CF_COLOR_R) = red;
		ACCESS_CHAR(player->p, CF_COLOR_B) = blue;
		ACCESS_CHAR(player->p, CF_COLOR_G) = green;
		ACCESS_CHAR(player->p, CF_COLOR_A) = transparency;
	}
	else
	{
		ACCESS_CHAR(player->p, CF_COLOR_R) = red;
		ACCESS_CHAR(player->p, CF_COLOR_B) = blue;
		ACCESS_CHAR(player->p, CF_COLOR_G) = green;
		ACCESS_CHAR(player->p, CF_COLOR_A) = transparency;
	}
}

void set_health(Player *player, short health)
{
	ACCESS_SHORT(player->p, CF_CURRENT_HEALTH) = health;
}

void set_spirit(Player *player, short spirit, short delay)
{
	ACCESS_SHORT(player->p, CF_CURRENT_SPIRIT) = spirit; //1000 divided by 200
	ACCESS_SHORT(player->p, CF_SPIRIT_REGEN_DELAY) = delay;
}

void reset_skills(Player *player)
{
	int nb_skills = 4;

	if (ACCESS_CHAR(player->p, CF_CHARACTER_INDEX) == PATCHOULI)
		nb_skills = 5;

	if (GetKeyState(savestate_keys.reset_skills) & 0x8000)
	{
		for (int i = 0; i < nb_skills; ++i)
		{
			ACCESS_CHAR(player->p, CF_SKILL_LEVELS_1 + i) = 0;
			ACCESS_CHAR(player->p, CF_SKILL_LEVELS_2 + i) = 0;
		}

		for (int i = nb_skills; i < 32; ++i)
		{
			ACCESS_CHAR(player->p, CF_SKILL_LEVELS_1 + i) = -1;
			ACCESS_CHAR(player->p, CF_SKILL_LEVELS_2 + i) = -1;
		}
	}
}
