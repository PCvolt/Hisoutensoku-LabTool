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

bool held_save_key = FALSE;
bool held_set_key = FALSE;
void position_management(Player *p1, Player *p2) {
	if (GetKeyState(savestate_keys.save) & 0x8000) {
		if (!held_save_key)
		{
			custom_pos = save_checkpoint(p1);
			custom_pos2 = save_checkpoint(p2);
			printf("Positions saved at: (%f, %f) VS (%f, %f)\n", custom_pos.x, custom_pos.y, custom_pos2.x, custom_pos2.y);
		}
		held_save_key = TRUE;
	}
	else
	{
		held_save_key = FALSE;
	}


	if (GetKeyState(savestate_keys.reset) & 0x8000)
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
					set_position(p1, custom_pos, CONSERVED);
					set_position(p2, custom_pos2, CONSERVED);
				}//conserved momentum is killed if you reset the air position from ground
			}
			held_set_key = TRUE;
		}
	}
	else
	{
		held_set_key = FALSE;
	}
}


/* FRAMECOUNT */
int frame_advantage = 0;
bool blockstring = FALSE;
void frameadvantage_count(Player *p1, Player *p2) {

	if (p1->current_sequence > 10 && p2->current_sequence > 10)
	{
		frame_advantage = 0;
		blockstring = TRUE;
	}
	if ((p1->current_sequence < 10 || p2->current_sequence < 10) && blockstring)
	{
		if (p1->current_sequence < 10 && p2->current_sequence < 10)
		{ //both stay on the ground (works)
			blockstring = FALSE;

			if (ACCESS_SHORT(p1->p, CF_ELAPSED_IN_SUBSEQ) < ACCESS_SHORT(p2->p, CF_ELAPSED_IN_SUBSEQ))
				std::cout << "P1 is -" << frame_advantage << "F" << std::endl << std::endl;
			else
				std::cout << "P1 is +" << frame_advantage << "F" << std::endl << std::endl;
		}

		++frame_advantage;
	}
}

int isIdle = -1;
void trademash_count(Player *player)
{
	if (player->frameflag & FF_GUARDING) //GUARDING TRUE/FALSE
	{
		if (isIdle != 0)
		{
			++isIdle;
			if (isIdle <= 60)
				std::cout << "Trade-mashable in " << isIdle << "F" << std::endl;
		}
		isIdle = 0;
	}
	else
		++isIdle;
}//Does not work with instantly mashable strings.


/* MISCELLANEOUS */
void state_display(Player *player)
{
	int transparency = -1;
	int red = -1;
	int blue = -1;
	int green = -1;

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

void set_health(Player *player, short health)
{
	ACCESS_SHORT(player->p, CF_CURRENT_HEALTH) = health;
}

void set_spirit(Player *player, short spirit, short delay)
{
	ACCESS_SHORT(player->p, CF_CURRENT_SPIRIT) = spirit; //1000 divided by 200
	ACCESS_SHORT(player->p, CF_SPIRIT_REGEN_DELAY) = delay;
}

void print_card(Player *player)
{
	printf("%d ", player->card);//prints the number of cards
}

