// LabToolDLL.cpp : Defines the exported functions for the DLL application.
#include "stdafx.h"

//Personal header
#include "functions.h"

//SWRSToys headers
#include "swrs.h"
#include "address.h"
#include "fields.h"

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

/* freopen is obsolete Gifu :( */
#pragma warning (disable : 4996)

/* It is defined in swrs.h but it isn't seen... */
extern const BYTE TARGET_HASH[16];
__declspec(selectany) const BYTE TARGET_HASH[16] =
{
	// ver1.10
	// 0x26, 0x8a, 0xfd, 0x82, 0x76, 0x90, 0x3e, 0x16, 0x71, 0x6c, 0x14, 0x29, 0xc6, 0x95, 0x9c, 0x9d
	// ver1.10a
	0xdf, 0x35, 0xd1, 0xfb, 0xc7, 0xb5, 0x83, 0x31, 0x7a, 0xda, 0xbe, 0x8c, 0xd9, 0xf5, 0x3b, 0x2e
};
#define SWRS_ADDR_1PKEYMAP 0x00898940
#define SWRS_ADDR_2PKEYMAP 0x0089912C

#define CBattleManager_Create(p) \
  Ccall(p, s_origCBattleManager_OnCreate, void*, ())()
#define CBattleManager_Render(p) \
  Ccall(p, s_origCBattleManager_OnRender, void, ())()
#define CBattleManager_Process(p) \
  Ccall(p, s_origCBattleManager_OnProcess, int, ())()
#define CBattleManager_Destruct(p, dyn) \
  Ccall(p, s_origCBattleManager_OnDestruct, void*, (int))(dyn)

static unsigned long s_origCBattleManager_OnCreate;
static unsigned long s_origCBattleManager_OnDestruct;
static unsigned long s_origCBattleManager_OnRender;
static unsigned long s_origCBattleManager_OnProcess;
static DWORD s_origCBattleManager_Size;

#define ADDR_BMGR_P1 0x0C
#define ADDR_BMGR_P2 0x10

static char s_profilePath[1024 + MAX_PATH];
Keys savestate_keys;
Toggles toggle_keys;
//Commands *commands = NULL;

void OpenConsole() {
	if (AllocConsole()) {
		FILE* stream = NULL; // initializing this makes the game crash, but not doing so causes a warning, hence the pragma (deleted on this version since it apparently works now)
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		std::wcin.clear();
		std::cin.clear();
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
	}
	std::cout << "Console allocated:" << std::endl;
}
/*
void readKey(int keyCommand, int keymapIndex) {
	if (keyCommand == 1 || keyCommand == -1) {
		inputs[inputCount].type = INPUT_KEYBOARD;
		inputs[inputCount].ki.wVk = 0;
		inputs[inputCount].ki.wScan = keymap[keymapIndex];
		inputs[inputCount].ki.dwFlags = KEYEVENTF_SCANCODE | ((keyCommand == -1) ? KEYEVENTF_KEYUP : 0);
		inputs[inputCount].ki.time = 0;
		inputs[inputCount].ki.dwExtraInfo = 0;
		inputCount++;
	}
}*/
/*
void ProcessKeys (INPUT *inputs, int& inputCount, Commands *commands, int* keymap) {
	
	readKey(inputs, &inputCount, commands->Up, KeymapIndex::Up);
	readKey(inputs, &inputCount, commands->Down, KeymapIndex::Down);
	readKey(inputs, &inputCount, commands->Left, KeymapIndex::Left);
	readKey(inputs, &inputCount, commands->Right, KeymapIndex::Right);
	readKey(inputs, &inputCount, commands->A, KeymapIndex::A);
	readKey(inputs, &inputCount, commands->B, KeymapIndex::B);
	readKey(inputs, &inputCount, commands->C, KeymapIndex::C);
	readKey(inputs, &inputCount, commands->D, KeymapIndex::D);
	readKey(inputs, &inputCount, commands->Sw, KeymapIndex::Sw);
	readKey(inputs, &inputCount, commands->Sc, KeymapIndex::SC);
}
*/


void* __fastcall CBattleManager_OnCreate(void *This) {
	CBattleManager_Create(This);
	std::cout << "OnCreate Called" << std::endl;

	savestate_keys.save_pos = ::GetPrivateProfileInt("KEYS", "save_pos", 0, s_profilePath);
	savestate_keys.reset_pos = ::GetPrivateProfileInt("KEYS", "reset_pos", 0, s_profilePath);
	savestate_keys.display_states = ::GetPrivateProfileInt("KEYS", "display_states", 0, s_profilePath);
	savestate_keys.randomCH = ::GetPrivateProfileInt("KEYS", "toggle_randomCH", 0, s_profilePath);
	savestate_keys.reset_skills = ::GetPrivateProfileInt("KEYS", "reset_skills", 0, s_profilePath);

	toggle_keys.display_states = true;
	toggle_keys.randomCH = false;


	/*
	int inputCount = 0;
	INPUT inputs[2 * 10];

	commands = new Commands();
	ProcessKeys(inputs, 0, commands, (int*)SWRS_ADDR_1PKEYMAP);
	ProcessKeys(inputs, 10, commands, (int*)SWRS_ADDR_2PKEYMAP);
	if (inputCount > 0) {
		if (GetForegroundWindow() == windowHandle) {
			SendInput(inputCount, inputs, sizeof(INPUT));
		}
	}
	*/
	return This;
}

void __fastcall CBattleManager_OnRender(void *This) {
	CBattleManager_Render(This);
}


int __fastcall CBattleManager_OnProcess(void *This) {
	int ret;
	ret = CBattleManager_Process(This);
	int battleManager = ACCESS_INT(ADDR_BATTLE_MANAGER, 0);


	if (g_mainMode == SWRSMODE_VSPLAYER || g_mainMode == SWRSMODE_PRACTICE)
	{
		Player p1;
		Player p2;
		update_playerinfo(&p1, battleManager, ADDR_BMGR_P1);
		update_playerinfo(&p2, battleManager, ADDR_BMGR_P2);

		position_management(&p1, &p2);
		trademash_count(&p2);
		hjcadvantage_count(&p1, &p2);
		frameadvantage_count(&p1, &p2);

		random_CH(&p2);
		fixed_tech(&p1);
		reset_skills(&p1);
		reset_skills(&p2);

		state_display(&p1);
		state_display(&p2);

		is_tight(&p2);
	}

	return ret;
}



void* __fastcall CBattleManager_OnDestruct(void *This, int mystery, int dyn) {
	void* ret;
	ret = CBattleManager_Destruct(This, dyn);
	std::cout << "OnDestruct Called" << std::endl;

	return ret;
}

extern "C" {
	__declspec(dllexport) bool CheckVersion(const BYTE hash[16])
	{
		return ::memcmp(TARGET_HASH, hash, sizeof TARGET_HASH) == 0;
	}

	__declspec(dllexport) bool Initialize(HMODULE hMyModule, HMODULE hParentModule)
	{
		DWORD old;
		OpenConsole();


		/* .ini */
		::GetModuleFileName(hMyModule, s_profilePath, 1024);
		::PathRemoveFileSpec(s_profilePath);
		::PathAppend(s_profilePath, "LabTool.ini");


		::VirtualProtect((PVOID)text_Offset, text_Size, PAGE_EXECUTE_WRITECOPY, &old);
		s_origCBattleManager_OnCreate =
			TamperNearJmpOpr(CBattleManager_Creater, union_cast<DWORD>(CBattleManager_OnCreate));
		::VirtualProtect((PVOID)text_Offset, text_Size, old, &old);
		::VirtualProtect((PVOID)rdata_Offset, rdata_Size, PAGE_WRITECOPY, &old);
		s_origCBattleManager_OnDestruct =
			TamperDword(vtbl_CBattleManager + 0x00, union_cast<DWORD>(CBattleManager_OnDestruct));
		s_origCBattleManager_OnRender =
			TamperDword(vtbl_CBattleManager + 0x38, union_cast<DWORD>(CBattleManager_OnRender));
		s_origCBattleManager_OnProcess =
			TamperDword(vtbl_CBattleManager + 0x0c, union_cast<DWORD>(CBattleManager_OnProcess));
		::VirtualProtect((PVOID)rdata_Offset, rdata_Size, old, &old);
		::FlushInstructionCache(GetCurrentProcess(), NULL, 0);

		return true;
	}
}


//https://stackoverflow.com/a/32999084
//https://stackoverflow.com/a/33001454
//https://stackoverflow.com/a/8145349

/*
TODO LIST:
	Macros:
*Fixed tech 4/5/6
*Random tech
*BE after X hits toggle
*Reversal options after X hits, either Dash, HJ, or invincible attack
*(same as above, but for wake-up as well)

	Resource options:
Precise health&spirit display

	Input display options:
*Xrd movement display (in my wet dreams)
*Change the position of that input viewer would be nice
*Display the attack (such as 6A, 22B, 5AAA...)
*Combo Recipe

	Cards related options:
*Decks retained when positions are reset so you don't need to flip through meter again
*choosing what level of skill you want
*A way to start training with alts in your deck already on(probably impossible, but it would be nice)

	Weather options:
Set timer for incoming weather or ending weather

	Character specific options:
*Sanae packet selection
*No/Minimal CD Sanae assists
*Save position of alice dolls when you save position of characters
*/