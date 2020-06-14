#include "stdafx.h"
#include "joystick.h"
#include "labTool_manager.h"

#define ADDR_BMGR_P1 0x0C
#define ADDR_BMGR_P2 0x10


LabToolManager& LabToolManager::getInstance()
{
	static LabToolManager s_manager;
	return s_manager;
}

HWND LabToolManager::getSokuHandle()
{
	static HWND s_windowHandle = FindWindowEx(nullptr, nullptr, "th123_110a", nullptr);
	return s_windowHandle;
}

bool LabToolManager::isHisoutensokuOnTop()
{
	auto t1 = getSokuHandle();
	auto t2 = GetForegroundWindow();
	return getSokuHandle() == GetForegroundWindow();
}

void LabToolManager::create()
{
	RET_VOID_IF_FALSE(fetchCurrentMode());
	RET_VOID_IF_FALSE(isValidMode());

	_pConsole = std::make_unique<LabToolConsole>();
	RET_VOID_IF_FALSE(SetForegroundWindow(getSokuHandle()));
	/*
	_pPlayerMain = std::make_unique<PlayerImpl>();
	_pPlayerMain->setData(ADDR_BMGR_P1);

	_pPlayerSecond = std::make_unique<PlayerImpl>();
	_pPlayerSecond->setData(ADDR_BMGR_P2);
	std::cout << "Girls are ready! " + _pPlayerMain->getGirlName() + " vs " + _pPlayerSecond->getGirlName() + "." << std::endl;
	*/
}

void LabToolManager::destruct()
{
	_pPlayerMain = nullptr;
	_pPlayerSecond = nullptr;
	_currentMode = EMode::eUndefined;
	_pConsole = nullptr;
}

bool LabToolManager::isValidMode() const
{
	return _currentMode == EMode::ePractice || _currentMode == EMode::eVsPlayer || _currentMode == EMode::eReplay;
}

const PlayerImplPtr& LabToolManager::getPlayerMain()
{
	return _pPlayerMain;
}

const PlayerImplPtr& LabToolManager::getPlayerSecond()
{
	return _pPlayerSecond;
}

bool LabToolManager::fetchCurrentMode()
{
	if (g_subMode == SWRSSUBMODE::SWRSSUBMODE_REPLAY)
	{
		_currentMode = EMode::eReplay;
		return true;
	}

	switch (g_mainMode)
	{
	case SWRSMODE::SWRSMODE_PRACTICE:
	{
		_currentMode = EMode::ePractice;
		return true;
	}
	case SWRSMODE::SWRSMODE_VSPLAYER:
	{
		_currentMode = EMode::eVsPlayer;
		return true;
	}
	default:
	{
		return false;
	}
	}
}

void welcomeScreen()
{
	std::cout
		<< "\tWelcome to LabTool 1.1.0!" << std::endl << std::endl
		<< "Practice Mode - VS Mode - Replay Mode" << std::endl
		<< " ===============================" << std::endl
		<< "| 1: Positions reset\t\t|" << std::endl
		<< "| 2: Positions save\t\t|" << std::endl
		<< "| 4: Skills reset\t\t|" << std::endl
		<< "| backspace: State display\t|" << std::endl
		<< "| 0: Configure joystick\t\t|" << std::endl
		<< " ===============================" << std::endl << std::endl;
}

bool configStart = false;
bool pass1, pass2, pass3, pass4 = false;
void configureScreen()
{

	// If 0 is pressed, configuration starts.
	if (GetAsyncKeyState(0x30) & 0x8000 && LabToolManager::isHisoutensokuOnTop())
	{
		system("cls");
		configStart = true;

		std::cout << "Associate button for [Reset position]: ";
	}

	if (configStart)
	{
		int btnNumber = -1;

		//reset pos
		for (int i = 0; i < sizeof(joystick.joypadBuffer.rgbButtons); ++i)
		{
			if (pass1 || joystick.joypadBuffer.rgbButtons[i] == 0x80)
			{
				btnNumber = i;
				std::cout << btnNumber << std::endl;
				pass1 = true;
			}
		}

		//save pos
		for (int i = 0; i < sizeof(joystick.joypadBuffer.rgbButtons); ++i)
		{
			if (pass2 || joystick.joypadBuffer.rgbButtons[i] == 0x80)
			{
				btnNumber = i;
				std::cout << btnNumber << std::endl;
				pass2 = true;

			}
		}
		//reset skills
		for (int i = 0; i < sizeof(joystick.joypadBuffer.rgbButtons); ++i)
		{
			if (pass3 || joystick.joypadBuffer.rgbButtons[i] == 0x80)
			{
				btnNumber = i;
				std::cout << btnNumber << std::endl;
				pass3 = true;
			}
		}
		//display states
		for (int i = 0; i < sizeof(joystick.joypadBuffer.rgbButtons); ++i)
		{
			if (joystick.joypadBuffer.rgbButtons[i] == 0x80)
			{
				btnNumber = i;
				std::cout << btnNumber << std::endl;
				std::cout << "Configuration over" << std::endl;
				pass4 = true;
				configStart = false;
			}
		}
	}
}

LabToolConsole::LabToolConsole()
{
	if (AllocConsole())
	{
		_cinBuffer = std::cin.rdbuf();
		_coutBuffer = std::cout.rdbuf();
		_cerrBuffer = std::cerr.rdbuf();
		_consoleInput.open("CONIN$", std::ios::in);
		_consoleOutput.open("CONOUT$", std::ios::out);
		_consoleError.open("CONOUT$", std::ios::out);
		std::cin.rdbuf(_consoleInput.rdbuf());
		std::cout.rdbuf(_consoleOutput.rdbuf());
		std::cerr.rdbuf(_consoleError.rdbuf());
		std::cin.clear();
		std::cout.clear();
		std::cerr.clear();

		SetConsoleTitle(std::string("LabTool 1.1.0").c_str());

		welcomeScreen();
		return;
	}

	std::cout << "Console allocation problem, restart Hisoutensoku, please." << std::endl;
}

LabToolConsole::~LabToolConsole()
{
	_consoleInput.close();
	_consoleOutput.close();
	_consoleError.close();
	std::cin.rdbuf(_cinBuffer);
	std::cout.rdbuf(_coutBuffer);
	std::cerr.rdbuf(_cerrBuffer);
	_cinBuffer = nullptr;
	_coutBuffer = nullptr;
	_cerrBuffer = nullptr;
	FreeConsole();
}

void PlayerImpl::setData(unsigned offset)
{
	ReadField(g_pbattleMgr, offset, _data);
	ReadField(_data, CF_CHARACTER_INDEX, _index);
}

void* PlayerImpl::data()
{
	return _data;
}

const std::string& PlayerImpl::getGirlName() const
{
	static std::array<std::string, 21U> array_of_girls = {
		{"Reimu",   "Marisa", "Sakuya", "Alice", "Patchouli", "Youmu",
		 "Remilia", "Yuyuko", "Yukari", "Suika", "Reisen",    "Aya",
		 "Komachi", "Iku",    "Tenshi", "Sanae", "Cirno",     "Meiling",
		 "Utsuho",  "Suwako", "Namazu"} };

	return array_of_girls[_index];
}

short& PlayerImpl::currentSeq()
{
	return GetField<short>(_data, CF_CURRENT_SEQ);
}

bool PlayerImpl::isCurrSeqChange() const
{
	short newSeq = 0;
	ReadField(_data, CF_CURRENT_SEQ, newSeq);
	return _currentSeq != newSeq;
}