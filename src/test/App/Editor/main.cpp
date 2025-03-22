#include <MyECS/World.h>
#include <MyGE/App/Editor/Editor.h>
#include <MyGE/App/Editor/InspectorRegistry.h>

#include "TestInspector.h"

#ifndef NDEBUG
#include <dxgidebug.h>
#endif

using namespace My::MyGE;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
  int rst;
  try {
    Editor app(hInstance);
    if (!app.Init()) return 1;

    app.GetGameWorld()->entityMngr.cmptTraits.Register<TestInspector>();
    InspectorRegistry::Instance().RegisterCmpts<TestInspector>();

    rst = app.Run();
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    rst = 1;
  }

#ifndef NDEBUG
  Microsoft::WRL::ComPtr<IDXGIDebug> debug;
  DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug));
  debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif

  return rst;
}
