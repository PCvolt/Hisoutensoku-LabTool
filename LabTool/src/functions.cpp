#include "stdafx.h"
#pragma warning(disable:4996)
#define SWRS_USES_HASH
/* Keymap addresses of both players */
#define SWRS_ADDR_1PKEYMAP 0x00898940
#define SWRS_ADDR_2PKEYMAP 0x0089912C

//Personal header
#include "functions.h"
#include "labTool_manager.h"

const auto &get_girl_name(Player *player)
{
  static std::array<std::string, 21U> array_of_girls = {
    {"Reimu",   "Marisa", "Sakuya", "Alice", "Patchouli", "Youmu",
    "Remilia", "Yuyuko", "Yukari", "Suika", "Reisen",    "Aya",
    "Komachi", "Iku",    "Tenshi", "Sanae", "Cirno",     "Meiling",
    "Utsuho",  "Suwako", "Namazu"} };

  return array_of_girls[player->index];
}

/* UPDATE */
void update_position(Player *player)
{
	player->position.x = ACCESS_FLOAT(player->p, CF_X_POS);
	player->position.y = ACCESS_FLOAT(player->p, CF_Y_POS);
	player->position.xspeed = ACCESS_FLOAT(player->p, CF_X_SPEED);
	player->position.yspeed = ACCESS_FLOAT(player->p, CF_Y_SPEED);
	player->position.gravity = ACCESS_FLOAT(player->p, CF_GRAVITY);
	player->position.direction = ACCESS_INT(player->p, CF_DIR);
}

void update_playerinfo(Player *player, int add_bmgr_px)
{
	player->p = ACCESS_PTR(g_pbattleMgr, add_bmgr_px);
	player->index = ACCESS_INT(player->p, CF_CHARACTER_INDEX);

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

void position_management(Player *p1, Player *p2) {
	if (GetKeyState(savestate_keys.save_pos) & 0x8000 && LabToolManager::isHisoutensokuOnTop()) {
		if (!held_keys.save_pos)
		{
			custom_pos = save_checkpoint(p1);
			custom_pos2 = save_checkpoint(p2);
			printf("Positions saved at: (%f, %f) VS (%f, %f)\n", custom_pos.x, custom_pos.y, custom_pos2.x, custom_pos2.y);
		}
		held_keys.save_pos = true;
	}
	else
	{
		held_keys.save_pos= false;
	}


	if (GetKeyState(savestate_keys.reset_pos) & 0x8000 && LabToolManager::isHisoutensokuOnTop())
	{
		if (!held_keys.set_pos)
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
			held_keys.set_pos = true;
		}
	}
	else
	{
		held_keys.set_pos = false;
	}
}


/* FRAMECOUNT */
void frameadvantage_count(Player *p1, Player *p2) {

	if (p1->current_sequence > 10 && p2->current_sequence > 10)
	{
		misc_states.frame_advantage = 0;
		misc_states.blockstring = true;
	}
	if ((p1->current_sequence < 10 || p2->current_sequence < 10) && misc_states.blockstring)
	{
		if (p1->current_sequence < 10 && p2->current_sequence < 10)
		{ //both stay on the ground (works)
			misc_states.blockstring = false;

      if (p1->elapsed_in_subseq < p2->elapsed_in_subseq)
      {
        std::cout << get_girl_name(p1) << " is -" << misc_states.frame_advantage << "F" << std::endl << std::endl;
      }
      else if (p1->elapsed_in_subseq > p2->elapsed_in_subseq)
      {
        std::cout << get_girl_name(p1) << " is +" << misc_states.frame_advantage << "F" << std::endl << std::endl;
      }
      else
      {
        std::cout << get_girl_name(p1) << " is " << 0 << "F" << std::endl << std::endl;
      }
		}

		++misc_states.frame_advantage;
	}
}

void hjcadvantage_count(Player *p1, Player *p2) {
	if (p1->current_sequence > 10 && p2->current_sequence > 10 && !misc_states.hjc_blockstring)
	{//there was a frame both were acting, we entered a blockstring
		misc_states.hjc_advantage = 0;
		misc_states.hjc_blockstring = true;
	}
	else if (p1->current_sequence <= 10 && p2->current_sequence <= 10 && misc_states.hjc_blockstring)
	{//both went back to idle
		misc_states.hjc_advantage = 0;
		misc_states.hjc_blockstring = false;
	}
	if ((p1->current_sequence != 204 && p1->current_sequence >= 201 && p1->current_sequence <= 215) && misc_states.hjc_blockstring)
	{//p2 is still blocking, p1 is high air
		
		if (p2->current_sequence < 10)
		{ //if p2 now recovered, we display
			misc_states.hjc_blockstring = false;

      if (ACCESS_SHORT(p1->p, CF_ELAPSED_IN_SUBSEQ) < ACCESS_SHORT(p2->p, CF_ELAPSED_IN_SUBSEQ))
        std::cout << get_girl_name(p1) << " is [-" << misc_states.hjc_advantage << "F]" << std::endl << std::endl;
      else
        std::cout << get_girl_name(p1) << " is [+" << misc_states.hjc_advantage << "F]" << std::endl << std::endl;
		}
		++misc_states.hjc_advantage;
	}	
}

void gap_count(Player *player)
{
	if (player->frameflag & FF_GUARDING || !(player->frameflag & FF_GUARD_AVAILABLE))
	{
		if (misc_states.isIdle != -1)
		{
			++misc_states.isIdle;
			if (misc_states.isIdle <= 30)
				std::cout << "Gap: " << misc_states.isIdle << "F" << std::endl;
		}
		misc_states.isIdle = -1;
	}
	else
		++misc_states.isIdle;
}

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
		if (misc_states.untight_nextframe)
		{ //bug in which the first frame of the first hit is considered untight
			std::cout << "Gap: 0F (mashable)" << std::endl;
		}

		misc_states.untight_nextframe = false;
		misc_states.untight_nextframe = untight_check(player);
	}
	else
	{
		misc_states.untight_nextframe = false;
	}
}


/* MACROS */
void random_CH(Player *player)
{

	if (GetAsyncKeyState(savestate_keys.randomCH) & 1 && LabToolManager::isHisoutensokuOnTop())
		toggle_keys.randomCH = !toggle_keys.randomCH;
	
	if (toggle_keys.randomCH)
	{
		bool CH = false;
		int a = rand() % 2;
		if (a == 1)
			CH = true;


		if (player->frameflag & FF_GUARD_AVAILABLE)
		{//if not in CH state already and can guard (to prevent CH mid-combo)
			if (!misc_states.already_CH)
			{
				if (CH)
				{
					ACCESS_INT(player->framedata, FF_FFLAGS) = ACCESS_INT(player->framedata, FF_FFLAGS) xor 64;
					misc_states.already_CH = true;
				}
				else if (!CH)
				{
					misc_states.already_CH = false;
				}
			}
			else if (misc_states.already_CH)
			{
				if (!CH)
				{
					ACCESS_INT(player->framedata, FF_FFLAGS) = ACCESS_INT(player->framedata, FF_FFLAGS) xor 64;
					misc_states.already_CH = false;
				}
				else if (CH)
				{
					misc_states.already_CH = true;
				}
			}
		}
	}
	else if (ACCESS_INT(player->framedata, FF_FFLAGS) & FF_CH_ON_HIT)
	{
		ACCESS_INT(player->framedata, FF_FFLAGS) = ACCESS_INT(player->framedata, FF_FFLAGS) xor 64;
		misc_states.already_CH = false;
	}
}

Commands commands_p1 = { 0 };
Commands commands_p2 = { 0 };
void send_inputs(Commands commands_p1, Commands commands_p2) {
	int inputCount = 0;
	INPUT inputs[2 * 10];

	auto ProcessKeys = [](INPUT* inputs, int& inputCount, Commands* commands, int* keymap) {
		auto ProcessKey = [&inputs, &inputCount, &keymap](int keyCommand, int keymapIndex) {
			if (keyCommand == 1 || keyCommand == -1)
			{
				inputs[inputCount].type = INPUT_KEYBOARD;
				inputs[inputCount].ki.wVk = 0;
				inputs[inputCount].ki.wScan = keymap[keymapIndex];
				inputs[inputCount].ki.dwFlags = KEYEVENTF_SCANCODE | ((keyCommand == -1) ? KEYEVENTF_KEYUP : 0);
				inputs[inputCount].ki.time = 0;
				inputs[inputCount].ki.dwExtraInfo = 0;
				inputCount++;
			}
		};
		ProcessKey(commands->up, KeymapIndex::up);
		ProcessKey(commands->down, KeymapIndex::down);
		ProcessKey(commands->left, KeymapIndex::left);
		ProcessKey(commands->right, KeymapIndex::right);
		ProcessKey(commands->A, KeymapIndex::A);
		ProcessKey(commands->B, KeymapIndex::B);
		ProcessKey(commands->C, KeymapIndex::C);
		ProcessKey(commands->D, KeymapIndex::D);
		ProcessKey(commands->sw, KeymapIndex::sw);
		ProcessKey(commands->sc, KeymapIndex::sc);
	};

	/* With two Commands structures for each player, we obtain their mappings and construct *inputs */
	ProcessKeys(inputs, inputCount, &commands_p1, (int*)SWRS_ADDR_1PKEYMAP);
	ProcessKeys(inputs, inputCount, &commands_p2, (int*)SWRS_ADDR_2PKEYMAP);

	if (LabToolManager::isHisoutensokuOnTop())
	{
		SendInput(inputCount, inputs, sizeof(INPUT)); //20, array of 20 INPUTs, size of INPUT
	}
}


int forward_wakeup[20] = { 55,53,49,50,48,58,36,64,77,48,46,47,55,89,43,54,55,45,56,82 };
int backward_wakeup[20] = { 55,52,47,50,48,58,36,64,77,48,59,47,55,89,43,59,55,45,56,82 };
int neutral_wakeup[20] = { 34,37,39,30,28,30,30,20,59,36,44,37,48,40,32,26,32,27,21,30 };

int wakeup_time(Player *player)
{	
	if (player->current_sequence == FORWARD_TECH)
		return forward_wakeup[player->index];
	else if (player->current_sequence == BACKWARD_TECH)
		return backward_wakeup[player->index];
	else if (player->current_sequence == NEUTRAL_TECH)
		return neutral_wakeup[player->index];

	return -1;
}

void input_reset(int *direction)
{
	if (*direction == 1)
		*direction = -1;
	else
		*direction = 0;
}

int wakeup_flag;
void wakeup_options(Player *player, Commands *commands, int *wakeup_count)
{
	if (wakeup_time(player) > 0)
		++*wakeup_count;
	else
		*wakeup_count = 0;

	if (wakeup_time(player) > 0)
	{
		int countdown = wakeup_time(player) - *wakeup_count;
		if (misc_states.wakeup_mode == jump)
		{
			if (countdown == 0)
				commands->up = 1;
		}
		else if (misc_states.wakeup_mode == highjump)
		{
			if (countdown == 0)
			{
				commands->D = 1;
				commands->up = 1;
			}
		}
		else if (misc_states.wakeup_mode == backdash)
		{
			if (countdown == 0)
			{
				commands->D = 1;
				if (player->position.direction == 1)
					commands->left = 1;
				else if (player->position.direction == -1)
					commands->right = 1;
			}
		}
		else if (misc_states.wakeup_mode == mash4A)
		{
			if (countdown == 0)
			{
				if (player->position.direction == 1)
					commands->left = 1;
				else if (player->position.direction == -1)
					commands->right = 1;
				commands->A = 1;
			}
		}
		else if (misc_states.wakeup_mode == d623B)
		{
			if (countdown == 4) //left
				commands->left = 1;
			if (countdown == 3) //down
			{
				commands->left = -1;
				commands->down = 1;
			}
			if (countdown == 2) //right
			{
				commands->down = -1;
				commands->right = 1;
			}
			if (countdown == 1) //down
			{
				commands->right = -1;
				commands->down = 1;
			}
			if (countdown == 0) //down + left/right + B
			{
				if (player->position.direction == 1)
					commands->right = 1;
				else if (player->position.direction == -1)
					commands->left = 1;
				commands->B = 1;
			}
		}
		else if (misc_states.wakeup_mode == spellcard)
		{
			if (countdown == 0)
				commands->sc = 1;
		}
		wakeup_flag = 3;
	}
	else if (wakeup_flag > 0)
	{
		input_reset(&commands->up);
		input_reset(&commands->down);
		input_reset(&commands->left);
		input_reset(&commands->right);
		input_reset(&commands->A);
		input_reset(&commands->B);
		input_reset(&commands->D);
		input_reset(&commands->sc);
		--wakeup_flag;
	}
}

void tech(Player *player, int *D, int *direction)
{
	if (*D == -1 && *direction == -1)
	{
		*D = 0;
		*direction = 0;
	}
	else if (*D == 1 && *direction == 1)
	{
		*D = -1;
		*direction = -1;
	}
	else if (player->frameflag & FF_DOWN && player->elapsed_in_subseq == 0)
	{
		*D = 1;
		*direction = 1;
	}
}

void tech_options(Player *player, Commands *commands)
{
	int *direction = nullptr;
	if (misc_states.tech_mode == neutral)
		direction = &commands->down;
	if (misc_states.tech_mode == left)
		direction = &commands->left;
	if (misc_states.tech_mode == right)
		direction = &commands->right;
	if (misc_states.tech_mode == random)
	{
		int a = rand() % 3;

		switch (a)
		{
		case 0: direction = &commands->down;
			break;
		case 1: direction = &commands->left;
			break;
		case 2: direction = &commands->right;
			break;
		default:
			break;
		}
	}
	tech(player, &commands->D, direction);
}

bool has_blocked;
void block_1st_hit(Player *player_blocking, Commands *commands_blocking, Player *player_hitting)
{
	if (player_blocking->current_sequence < 10 && player_hitting->current_sequence < 10)
	{
		has_blocked = false;
	}
	else if (player_blocking->current_sequence >= 150 && player_blocking->current_sequence <= 165)
		has_blocked = true;
	if (misc_states.firstblock_mode != noblocking)
	{
		if (player_blocking->current_sequence < 10 && player_hitting->current_sequence < 10)
		{
			input_reset(&commands_blocking->down);
			input_reset(&commands_blocking->left);
			input_reset(&commands_blocking->right);
		}

		if (player_hitting->current_sequence >= 300 && !has_blocked) //it comes back to false
		{
			if (player_blocking->position.direction == 1)
				commands_blocking->left = 1;
			else
				commands_blocking->right = 1;

			if (misc_states.firstblock_mode == crouching)
				commands_blocking->down = 1;	
		}
		else if (has_blocked)
		{
			if (!(player_blocking->current_sequence >= 154 && player_blocking->current_sequence <= 157))
				input_reset(&commands_blocking->down);
			input_reset(&commands_blocking->left);
			input_reset(&commands_blocking->right);
			
		}
	}
}

bool BE_inputting = false;
int direction_BE;
void BE_1st_hit(Player *player, Commands *commands)
{
	int *direction = nullptr;
	int *second_direction = nullptr;

	if (misc_states.BE_mode == BEdown)
	{
		direction = &commands->down;
	}
	if (misc_states.BE_mode == BEdownside)
	{
		direction = &commands->down;
		if (player->position.direction == 1)
			second_direction = &commands->left;
		else
			second_direction = &commands->right;
	}
	if (misc_states.BE_mode == BEside)
	{
		if (player->position.direction == 1)
			direction = &commands->right;
		else
			direction = &commands->left;
	}
	if (misc_states.BE_mode == BEback)
	{
		if (player->position.direction == 1)
			direction = &commands->left;
		else
			direction = &commands->right;
	}
	if (misc_states.BE_mode != noBE)
	{
		if (player->current_sequence >= 150 && player->current_sequence <= 158)
		{
			if (direction)
				*direction = 1;
			if (second_direction)
				*second_direction = 1;

			BE_inputting = !BE_inputting;
			if (BE_inputting)
				commands->D = 1;
			else
				commands->D = -1;

			direction_BE = 20;
		}
		else if ((player->current_sequence >= 220 && player->current_sequence <= 226) || player->current_sequence == 143 || player->current_sequence == 145 || (player->current_sequence >= 50 && player->current_sequence <= 73)) //Unsure
		{
			if (direction_BE > 0)
			{
				--direction_BE;
			}
			else
			{
				input_reset(&commands->D);
				if (direction)
					input_reset(direction);
				if (second_direction)
					input_reset(second_direction);
			}
		}
	}
}


void macros(Player *p1, Player *p2)
{
	/* WAKEUP OPTIONS */
	if (GetKeyState(savestate_keys.wakeup_macro) & 0x8000 && LabToolManager::isHisoutensokuOnTop()) {
		if (!held_keys.wakeup_macro)
		{
			misc_states.wakeup_mode = (misc_states.wakeup_mode + 1) % 7; //bad magic number
			std::string wakeup_mode_name;
			switch (misc_states.wakeup_mode)
			{
			case 0: wakeup_mode_name = "nothing"; break;
			case 1: wakeup_mode_name = "jump"; break;
			case 2: wakeup_mode_name = "highjump"; break;
			case 3: wakeup_mode_name = "backdash"; break;
			case 4: wakeup_mode_name = "mash4A"; break;
			case 5: wakeup_mode_name = "d623B"; break;
			case 6: wakeup_mode_name = "spellcard"; break;
			default:
				break;
			}
			std::cout << "#At wakeup, players will attempt " << wakeup_mode_name << std::endl;
		}
		held_keys.wakeup_macro = true;
	}
	else
	{
		held_keys.wakeup_macro = false;
	}

	/* TECH */
	if (GetKeyState(savestate_keys.tech_macro) & 0x8000 && LabToolManager::isHisoutensokuOnTop()) {
		if (!held_keys.tech_macro)
		{
			misc_states.tech_mode = (misc_states.tech_mode + 1) % 4; //bad magic number
			std::string tech_mode_name;
			switch (misc_states.tech_mode)
			{
			case 0: tech_mode_name = "neutral"; break;
			case 1: tech_mode_name = "left"; break;
			case 2: tech_mode_name = "right"; break;
			case 3: tech_mode_name = "random"; break;
			default:
				break;
			}
			std::cout << "#Players will be teching " << tech_mode_name << std::endl;
		}
		held_keys.tech_macro = true;
	}
	else
	{
		held_keys.tech_macro = false;
	}
	
	if (GetKeyState(savestate_keys.firstblock_macro) & 0x8000 && LabToolManager::isHisoutensokuOnTop()) {
		if (!held_keys.firstblock_macro)
		{
			misc_states.firstblock_mode = (misc_states.firstblock_mode + 1) % 3; //bad magic number
			std::string firstblock_mode_name;
			switch (misc_states.firstblock_mode)
			{
			case 0: firstblock_mode_name = "not block"; break;
			case 1: firstblock_mode_name = "only block the first hit while standing"; break;
			case 2: firstblock_mode_name = "only block the first hit while crouching"; break;
			default:
				break;
			}
			std::cout << "#Players will " << firstblock_mode_name << std::endl;
		}
		held_keys.firstblock_macro = true;
	}
	else
	{
		held_keys.firstblock_macro = false;
	}

	if (GetKeyState(savestate_keys.BE_macro) & 0x8000 && LabToolManager::isHisoutensokuOnTop()) {
		if (!held_keys.BE_macro)
		{
			misc_states.BE_mode = (misc_states.BE_mode + 1) % 5; //bad magic number
			std::string BE_mode_name;
			switch (misc_states.BE_mode)
			{
			case 0: BE_mode_name = "not BE"; break;
			case 1: BE_mode_name = "BE2"; break;
			case 2: BE_mode_name = "BE1"; break;
			case 3: BE_mode_name = "(j)BE6"; break;
			case 4: BE_mode_name = "(j)BE4"; break;
			default:
				break;
			}
			std::cout << "#Players will " << BE_mode_name << std::endl;
		}
		held_keys.BE_macro = true;
	}
	else
	{
		held_keys.BE_macro = false;
	}

	//wakeup_options(p1, &commands_p1, &misc_states.wakeup_count_p1);
	wakeup_options(p2, &commands_p2, &misc_states.wakeup_count_p2);
	//tech_options(p1, &commands_p1);
	tech_options(p2, &commands_p2);
	//block_1st_hit(p1, &commands_p1, p2);
	block_1st_hit(p2, &commands_p2, p1);
	BE_1st_hit(p1, &commands_p1);
	BE_1st_hit(p2, &commands_p2);

	send_inputs(commands_p1, commands_p2);
}


/* MISCELLANEOUS */
void state_display(Player *player)
{
	int transparency = -1;
	int red = -1;
	int blue = -1;
	int green = -1;

	if (GetAsyncKeyState(savestate_keys.display_states) & 1 && LabToolManager::isHisoutensokuOnTop())
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
		else if (!(player->frameflag & FF_GUARD_AVAILABLE))
		{
			red = 148;
			green = 0;
			blue = 211;
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

	if (GetKeyState(savestate_keys.reset_skills) & 0x8000 && LabToolManager::isHisoutensokuOnTop())
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
