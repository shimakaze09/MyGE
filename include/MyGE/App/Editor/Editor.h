#include "../DX12App/DX12App.h"

namespace Smkz::MyECS {
class World;
}

namespace Smkz::MyGE {
class Editor : public Smkz::MyGE::DX12App {
 public:
  Editor(HINSTANCE hInstance);
  ~Editor();

  virtual bool Init() override;

  MyECS::World* GetGameWorld();
  MyECS::World* GetSceneWorld();
  MyECS::World* GetEditorWorld();

  MyECS::World* GetCurrentGameWorld();

 private:
  virtual void Update() override;
  virtual void Draw() override;
  virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam,
                          LPARAM lParam) override;

  struct Impl;
  friend struct Impl;
  Impl* pImpl;
};
}  // namespace Smkz::MyGE
