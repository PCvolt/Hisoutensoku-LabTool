#include "stdafx.h"

#include "labTool_manager.h"

#define ADDR_BMGR_P1 0x0C
#define ADDR_BMGR_P2 0x10

const auto &getGirlName(int girlIdx)
{
  static std::array<std::string, 21U> array_of_girls = {
      {"Reimu",   "Marisa", "Sakuya", "Alice", "Patchouli", "Youmu",
       "Remilia", "Yuyuko", "Yukari", "Suika", "Reisen",    "Aya",
       "Komachi", "Iku",    "Tenshi", "Sanae", "Cirno",     "Meiling",
       "Utsuho",  "Suwako", "Namazu"}};

  return array_of_girls[girlIdx];
}

LabToolManager& LabToolManager::getInstance()
{ 
  static LabToolManager s_manager;
  return s_manager;
}

bool LabToolManager::isHisoutensokuOnTop()
{
  static HWND s_windowHandle = FindWindowEx(nullptr, nullptr, "th123_110a", nullptr);
  return s_windowHandle == GetForegroundWindow();
}

bool LabToolManager::create()
{
  auto res = false;
  RET_IF_FALSE(fetchCurrentMode(), res);

  _pPlayerMain = std::make_unique<PlayerImpl>();
  _pPlayerSecond = std::make_unique<PlayerImpl>();

  _pPlayerMain->_pointer = ACCESS_PTR(g_pbattleMgr, ADDR_BMGR_P1);
  _pPlayerMain->_index = ACCESS_INT(_pPlayerMain->_pointer, CF_CHARACTER_INDEX);

  _pPlayerSecond->_pointer = ACCESS_PTR(g_pbattleMgr, ADDR_BMGR_P2);
  _pPlayerSecond->_index = ACCESS_INT(_pPlayerSecond->_pointer, CF_CHARACTER_INDEX);

  return res;
}

void LabToolManager::destruct()
{
  _pPlayerMain = nullptr;
  _pPlayerSecond = nullptr;
  _currentMode = EMode::eUndefined;
}

bool LabToolManager::isValidMode() const
{
  return _currentMode != EMode::eUndefined;
}

const std::string& LabToolManager::getMainGirlName() const
{
  return getGirlName(_pPlayerMain->_index);
}

const std::string& LabToolManager::getSecondGirlName() const
{
  return getGirlName(_pPlayerSecond->_index);
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

