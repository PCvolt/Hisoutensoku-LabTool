#define SWRS_USES_HASH
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

//boost headers
/*
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
*/
//SWRSToys headers
#include "swrs.h"
#include "fields.h"
#include "address.h"

//Personal headers
#include "functions.cpp"

/*
using namespace boost::interprocess;
*/


#define CBattleManager_Create(p) \
  Ccall(p, s_origCBattleManager_OnCreate, void*, ())()
#define CBattleManager_Render(p) \
  Ccall(p, s_origCBattleManager_OnRender, void, ())()
#define CBattleManager_Process(p) \
  Ccall(p, s_origCBattleManager_OnProcess, int, ())()
#define CBattleManager_Destruct(p, dyn) \
  Ccall(p, s_origCBattleManager_OnDestruct, void*, (int))(dyn)

static unsigned long long s_origCBattleManager_OnCreate;
static unsigned long long s_origCBattleManager_OnDestruct;
static unsigned long long s_origCBattleManager_OnRender;
static unsigned long long s_origCBattleManager_OnProcess;
static DWORD s_origCBattleManager_Size;

#define ADDR_BMGR_P1 0x0C
#define ADDR_BMGR_P2 0x10

/*
static shared_memory_object *smo = nullptr;
static mapped_region *region_p1 = nullptr;
static mapped_region *region_p2 = nullptr;
static int SMO_SIZE = 108; //player1 + player2 + savestate_mode
static named_mutex *named_mtx = nullptr;
static named_condition *named_cnd = nullptr;
*/


/*
void create_IPC() {
	shared_memory_object::remove("MySharedMemory");
	smo = new shared_memory_object(create_only, "MySharedMemory", read_write);
	smo->truncate(SMO_SIZE);
	region_p1 = new mapped_region(*smo, read_write, 0, sizeof(Player));
	region_p2 = new mapped_region(*smo, read_write, sizeof(Player), sizeof(Player));
	std::memset(region_p1->get_address(), '0', region_p1->get_size());
	std::memset(region_p2->get_address(), '0', region_p2->get_size());

	named_mtx = new named_mutex{open_or_create, "mtx"};
	named_cnd = new named_condition{open_or_create, "cnd"};
	//lock here ?
}
*/
void OpenConsole() {
	if (AllocConsole()) {
		FILE* stream; // initializing this makes the game crash, but not doing so causes a warning, hence the pragma
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




void* __fastcall CBattleManager_OnCreate(void *This) {
	CBattleManager_Create(This);
	std::cout << "OnCreate Called" << std::endl;
/*
	create_IPC();
	std::cout << "create_IPC called" << std::endl;
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
		//untight_framecount(&p1);
		untight_framecount(&p2);
		frame_advantagecount(&p1, &p2);

		state_display(&p1);
		state_display(&p2);	
	}
/*
	scoped_lock<named_mutex> lock{*named_mtx};
	//get region addresses in smo
	Player *mem = static_cast<Player*>(region_p1->get_address());
	Player *mem2 = static_cast<Player*>(region_p2->get_address());
	//write to regions
	*mem = p1;
	*mem2 = p2;

	named_cnd->wait(lock); //Puts the 1st thread on hold, then the 2nd thread calls with notify_all() and 1st thread can continue
	named_cnd->notify_all();*/
	/* This is where client.exe is supposed to intervene*/


	//read from smo (after)
	//Player *after_mem = static_cast<Player*>(region_p1->get_address());
	//Player *after_mem2 = static_cast<Player*>(region_p2->get_address());
/*
	printf("%hd (%f, %f) VS ", after_mem->spirit, after_mem->position.x, after_mem->position.y);
	printf("%hd (%f, %f)\n", after_mem2->spirit, after_mem2->position.x, after_mem2->position.y);
*/
	return ret;
}


void* __fastcall CBattleManager_OnDestruct(void *This, int mystery, int dyn) {
	void* ret;
	ret = CBattleManager_Destruct(This, dyn);
	std::cout << "OnDestruct Called" << std::endl;
/*
	shared_memory_object::remove("MySharedMemory");
	named_mutex::remove("mtx");
	named_condition::remove("cnd");
	std::cout << "smo, mtx and cnd destroyed" << std::endl << std::endl;
*/
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
/*
Compile with: g++ -m32 LabTool.cpp -shared -o LabTool.dll
*/




/*
2nd tab: Frame advantage
Display frame advantage
Display gap between attacks


3rd tab: Combo
Manual Entry for Health
Manual Entry for Spirit
Manual Entry for Spirit Delay
Damages
Show untech time
Combo recipe (calculator ?)

4rd tab: Deck
Will it even be possible ;m;


5th tab: Macros
Random or Set techs
Random CH
BE2/1/6

6th: Character specific settings
*/
