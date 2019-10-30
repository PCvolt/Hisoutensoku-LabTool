// LabToolDLL.cpp : Defines the exported functions for the DLL application.
#include "stdafx.h"

//Personal header
#include "functions.h"

//SWRSToys headers
#include "address.h"
#include "fields.h"
#include "swrs.h"

//Standard C++ headers
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

//Standard headers
#include <d3d9.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <sstream>
#include <vector>
#include <windows.h>

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
Toggle_key toggle_keys;
Held_key held_keys;
Misc_state misc_states;

void OpenConsole() {
	if (AllocConsole()) {
		FILE* stream = nullptr; // initializing this makes the game crash, but not doing so causes a warning, hence the pragma (deleted on this version since it apparently works now)
		freopen("CONIN$", "r", stdin);
		freopen("CONOUT$", "w", stdout);
		freopen("CONOUT$", "w", stderr);
		std::wcin.clear();
		std::cin.clear();
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
    std::cerr.clear();
    std::cout << "Console allocated:" << std::endl;
	}
}


void* __fastcall CBattleManager_OnCreate(void *This) {
	CBattleManager_Create(This);
	std::cout << "OnCreate Called" << std::endl;

	/* .INI */
	savestate_keys.save_pos = ::GetPrivateProfileInt("KEYS", "save_pos", 0, s_profilePath);
	savestate_keys.reset_pos = ::GetPrivateProfileInt("KEYS", "reset_pos", 0, s_profilePath);
	savestate_keys.display_states = ::GetPrivateProfileInt("KEYS", "display_states", 0, s_profilePath);
	savestate_keys.randomCH = ::GetPrivateProfileInt("KEYS", "toggle_randomCH", 0, s_profilePath);
	savestate_keys.reset_skills = ::GetPrivateProfileInt("KEYS", "reset_skills", 0, s_profilePath);

	savestate_keys.tech_macro = ::GetPrivateProfileInt("KEYS", "tech_macro", 0, s_profilePath);
	savestate_keys.wakeup_macro = ::GetPrivateProfileInt("KEYS", "wakeup_macro", 0, s_profilePath);
	savestate_keys.firstblock_macro = ::GetPrivateProfileInt("KEYS", "firstblock_macro", 0, s_profilePath);
	savestate_keys.BE_macro = ::GetPrivateProfileInt("KEYS", "BE_macro", 0, s_profilePath);

	held_keys = { false };
	toggle_keys.display_states = false;
	toggle_keys.randomCH = false;

	misc_states.frame_advantage = 0;
	misc_states.blockstring = false;
	misc_states.hjc_advantage = 0;
	misc_states.hjc_blockstring = false;
	misc_states.isIdle = -1;
	misc_states.untight_nextframe = false;
	misc_states.already_CH = false;
	misc_states.wakeup_count_p1 = 0;
	misc_states.wakeup_count_p2 = 0;

	return This;
}

void __fastcall CBattleManager_OnRender(void *This) {
	CBattleManager_Render(This);
}



int __fastcall CBattleManager_OnProcess(void *This) {
	int ret;
	ret = CBattleManager_Process(This);
	int battleManager = ACCESS_INT(ADDR_BATTLE_MANAGER, 0);


	if ((g_mainMode == SWRSMODE_VSPLAYER || g_mainMode == SWRSMODE_PRACTICE) && g_subMode != SWRSSUBMODE_REPLAY)
	{
		Player p1;
		Player p2;
		update_playerinfo(&p1, battleManager, ADDR_BMGR_P1);
		update_playerinfo(&p2, battleManager, ADDR_BMGR_P2);

		position_management(&p1, &p2);
		gap_count(&p2);
		hjcadvantage_count(&p1, &p2);
		frameadvantage_count(&p1, &p2);
		is_tight(&p2);

		random_CH(&p2);
		macros(&p1, &p2);
		
		reset_skills(&p1);
		reset_skills(&p2);


		state_display(&p1);
		state_display(&p2);
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
		::FlushInstructionCache(GetCurrentProcess(), nullptr, 0);

		return true;
	}
}


//https://stackoverflow.com/a/32999084
//https://stackoverflow.com/a/33001454
//https://stackoverflow.com/a/8145349

/*
find a better way for buttons for:
HWND windowHandle = FindWindowEx(NULL, NULL, "th123_110a", NULL);

TODO LIST:
	Macros:
wakeup options but after x blocks

	Resource options:
Precise health&spirit display (d3dx9)

	Input display options:
*Xrd movement display (in my wet dreams) (d3dx9)
*Change the position of that input viewer would be nice for a nearer future
*Display the attack (such as 6A, 22B, 5AAA...) ?
*Combo Recipe

	Cards related options:
*Decks retained when positions are reset so you don't need to flip through meter again

	Character specific options:
*Sanae packet selection
*No/Minimal CD Sanae assists
*Save position of alice dolls when you save position of characters
*/
