#include "stdafx.h"

//Personal header
#include "functions.h"
#include "labTool_manager.h"

/* freopen is obsolete */
#pragma warning (disable : 4996)

//A couple of thoughts about the battle manager virtual functions table and its memory offsets (by Alexey-Ul)
//0x00 - destructor
//0x04 - after select arena start, best place for initialize girls void*(void* int int)
//0x08 - After exit on selection screen or menu
//0x0c - Progress??
//0x10 - announcement say start round
//0x14 - After load black scree
//0x18 - Progress???
//0x1c - After round
//0x20 - After battle (KO)
//0x24 - After battle girls talk
//0x28 - ??????????
//0x2c - before last hit in round  void*(void* void* int)
//0x30 - after stat round and before round end pure battle()
//0x34 - at the start before "start round" logo int is 1 and it becomes 2 after logo and becomes 3 after the end of round
//       and becomes 5 after final round before logo and and 6 after final and 7 after girls talks states void*(void* void* int) 
//0x38 - After select arena render or draw()
//0x3c - After select arena render????
//0x40 - After select arena render???? Last function???

#define OFFSET_BTLMGR_DESTR 0x00
#define OFFSET_BTLMGR_INIT 0x04
#define OFFSET_BTLMGR_DEINIT 0x08
#define OFFSET_BTLMGR_BATTLE 0x30
#define OFFSET_BTLMGR_RENDER 0x38

#define CBattleManager_Create(p) \
  Ccall(p, s_origCBattleManager_OnCreate, void*, ())()
#define CBattleManager_Render(p) \
  Ccall(p, s_origCBattleManager_OnRender, void, ())()
#define CBattleManager_Process(p) \
  Ccall(p, s_origCBattleManager_OnProcess, int, ())()
#define CBattleManager_Destruct(p, dyn) \
  Ccall(p, s_origCBattleManager_OnDestruct, void*, (int))(dyn)

#define CBattleManager_Initialize(p, dyn) Ccall(p, s_origCBattleManager_Initialize, void *, (int))(dyn)
#define CBattleManager_DeInitialize(p) Ccall(p, s_origCBattleManager_DeInitialize, int, ())()

static unsigned long s_origCBattleManager_OnCreate;
static unsigned long s_origCBattleManager_OnDestruct;
static unsigned long s_origCBattleManager_OnRender;
static unsigned long s_origCBattleManager_OnProcess;
static unsigned long s_origCBattleManager_Initialize;
static unsigned long s_origCBattleManager_DeInitialize;
static DWORD s_origCBattleManager_Size;

#define ADDR_BMGR_P1 0x0C
#define ADDR_BMGR_P2 0x10

static char s_profilePath[1024 + MAX_PATH];
Keys savestate_keys;
Toggle_key toggle_keys;
Held_key held_keys;
Misc_state misc_states;

void* __fastcall CBattleManager_OnInitialize(void* This, void* mystery, int dyn) {
	auto ret = CBattleManager_Initialize(This, dyn);
	LabToolManager::getInstance().create();
	return ret;
}
int __fastcall CBattleManager_OnDeInitialize(void* This) {
	LabToolManager::getInstance().destruct();
	auto ret = CBattleManager_DeInitialize(This);
	return CBattleManager_DeInitialize(This);
}


void* __fastcall CBattleManager_OnCreate(void* This) {
	CBattleManager_Create(This);
	std::cout << "\tEntering Character Select" << std::endl;

	/* .INI */
	savestate_keys.save_pos = ::GetPrivateProfileInt("KEYS", "save_pos", 0, s_profilePath);
	savestate_keys.reset_pos = ::GetPrivateProfileInt("KEYS", "reset_pos", 0, s_profilePath);
	savestate_keys.reset_skills = ::GetPrivateProfileInt("KEYS", "reset_skills", 0, s_profilePath);
	savestate_keys.tech_macro = ::GetPrivateProfileInt("KEYS", "tech_macro", 0, s_profilePath);
	savestate_keys.display_states = ::GetPrivateProfileInt("KEYS", "display_states", 0, s_profilePath);


	held_keys = { false };
	toggle_keys.display_states = false;

	misc_states.frame_advantage = 0;
	misc_states.blockstring = false;
	misc_states.hjc_advantage = 0;
	misc_states.hjc_blockstring = false;
	misc_states.isIdle = -1;
	misc_states.untight_nextframe = false;
	misc_states.wakeup_count_p1 = 0;
	misc_states.wakeup_count_p2 = 0;

	return This;
}

void __fastcall CBattleManager_OnRender(void* This) {
	CBattleManager_Render(This);
}

int __fastcall CBattleManager_OnProcess(void* This) {
	auto& labToolMgr = LabToolManager::getInstance();

	if (labToolMgr.isValidMode())
	{
		Player p1;
		Player p2;
		update_playerinfo(&p1, ADDR_BMGR_P1);
		update_playerinfo(&p2, ADDR_BMGR_P2);

		position_management(&p1, &p2);
		gap_count(&p2);
		hjcadvantage_count(&p1, &p2);
		frameadvantage_count(&p1, &p2);
		is_tight(&p2);

		reset_skills(&p1);
		reset_skills(&p2);
		state_display(&p1);
		state_display(&p2);
	}

	auto ret = CBattleManager_Process(This);
	int battleManager = ACCESS_INT(ADDR_BATTLE_MANAGER, 0);

	return ret;
}


void* __fastcall CBattleManager_OnDestruct(void* This, int mystery, int dyn) {
	void* ret;
	ret = CBattleManager_Destruct(This, dyn);
	system("cls");
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
		//OpenConsole();

		/* .ini */
		GetModuleFileName(hMyModule, s_profilePath, 1024);
		PathRemoveFileSpec(s_profilePath);
		PathAppend(s_profilePath, "LabTool.ini");


		if (!VirtualProtect((PVOID)text_Offset, text_Size, PAGE_EXECUTE_WRITECOPY, &old))
		{
			return false;
		}

		s_origCBattleManager_OnCreate =
			TamperNearJmpOpr(CBattleManager_Creater, union_cast<DWORD>(CBattleManager_OnCreate));

		if (!VirtualProtect((PVOID)text_Offset, text_Size, old, &old))
		{
			return false;
		}

		if (!VirtualProtect((PVOID)rdata_Offset, rdata_Size, PAGE_WRITECOPY, &old))
		{
			return false;
		}

		s_origCBattleManager_OnDestruct =
			TamperDword(vtbl_CBattleManager + OFFSET_BTLMGR_DESTR, union_cast<DWORD>(CBattleManager_OnDestruct));

		s_origCBattleManager_Initialize =
			TamperDword(vtbl_CBattleManager + OFFSET_BTLMGR_INIT, union_cast<DWORD>(CBattleManager_OnInitialize));

		s_origCBattleManager_DeInitialize =
			TamperDword(vtbl_CBattleManager + OFFSET_BTLMGR_DEINIT, union_cast<DWORD>(CBattleManager_OnDeInitialize));

		s_origCBattleManager_OnProcess =
			TamperDword(vtbl_CBattleManager + OFFSET_BTLMGR_BATTLE, union_cast<DWORD>(CBattleManager_OnProcess));

		s_origCBattleManager_OnRender =
			TamperDword(vtbl_CBattleManager + OFFSET_BTLMGR_RENDER, union_cast<DWORD>(CBattleManager_OnRender));

		if (!VirtualProtect((PVOID)rdata_Offset, rdata_Size, old, &old))
		{
			return false;
		}

		if (!FlushInstructionCache(GetCurrentProcess(), nullptr, 0))
		{
			return false;
		}

		return true;
	}
}


//https://stackoverflow.com/a/32999084
//https://stackoverflow.com/a/33001454
//https://stackoverflow.com/a/8145349
// shlwapi.lib user32.lib

/*
TODO LIST:
Precise health&spirit display (d3dx9) in windowname
controller messes up the key presses + implement controller presses
*/

