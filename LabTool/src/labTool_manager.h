#pragma once
#ifndef labTool_manager_h__
#define labTool_manager_h__

#define RET_IF_FALSE(Func, res)                                                \
  res = Func;                                                                  \
  if (!res) {                                                                  \
    return res;                                                                \
  }

struct PlayerImpl {
  void *_pointer;
  int _index;
};

// Class manager
class LabToolManager {
public:
  static LabToolManager& getInstance();
  static bool isHisoutensokuOnTop();

  bool create();
  void destruct();
  bool isValidMode() const;
  const std::string& getMainGirlName() const;
  const std::string& getSecondGirlName() const;

private:
  LabToolManager() = default;
  ~LabToolManager()  = default;
  LabToolManager(const LabToolManager&) = delete;
  LabToolManager(LabToolManager&&) = delete;
  LabToolManager& operator=(const LabToolManager&) = delete;
  LabToolManager& operator=(LabToolManager&&) = delete;

private:
  enum class EMode
  {
    eUndefined = 0,
    ePractice,
    eVsPlayer,
    eReplay
  };

private:
  bool fetchCurrentMode();

private:
  std::unique_ptr<PlayerImpl> _pPlayerMain;
  std::unique_ptr<PlayerImpl> _pPlayerSecond;
  EMode _currentMode = EMode::eUndefined;
};

#endif // labTool_manager_h__