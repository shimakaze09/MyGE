//
// Created by Admin on 17/03/2025.
//

#include "Components/Hierarchy.h"
#include "Components/Inspector.h"
#include "Components/ProjectViewer.h"
#include "Components/TestInspector.h"

#include "Systems/HierarchySystem.h"
#include "Systems/InspectorSystem.h"
#include "Systems/ProjectViewerSystem.h"

#include "CmptInsepctor.h"

#include <MyGE/App/DX12App/DX12App.h>

#include <MyGE/Render/DX12/MeshLayoutMngr.h>
#include <MyGE/Render/DX12/RsrcMngrDX12.h>
#include <MyGE/Render/DX12/ShaderCBMngrDX12.h>
#include <MyGE/Render/DX12/StdPipeline.h>

#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Asset/Serializer.h>

#include <MyGE/Core/Components/Camera.h>
#include <MyGE/Core/Components/MeshFilter.h>
#include <MyGE/Core/Components/MeshRenderer.h>
#include <MyGE/Core/Components/Name.h>
#include <MyGE/Core/Components/Skybox.h>
#include <MyGE/Core/Components/WorldTime.h>
#include <MyGE/Core/GameTimer.h>
#include <MyGE/Core/HLSLFile.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Mesh.h>
#include <MyGE/Core/Scene.h>
#include <MyGE/Core/Shader.h>
#include <MyGE/Core/ShaderMngr.h>
#include <MyGE/Core/Systems/CameraSystem.h>
#include <MyGE/Core/Systems/WorldTimeSystem.h>
#include <MyGE/Core/Texture2D.h>
#include <MyGE/Core/TextureCube.h>

#include <MyGE/Transform/Transform.h>

#include <MyGE/Core/ImGUIMngr.h>

#include <_deps/imgui/imgui.h>
#include <_deps/imgui/imgui_impl_dx12.h>
#include <_deps/imgui/imgui_impl_win32.h>

#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaCtxMngr.h>
#include <MyGE/ScriptSystem/LuaScript.h>

#include <MyLuaPP/MyLuaPP.h>

#include <dxgidebug.h>

using Microsoft::WRL::ComPtr;

class Editor : public My::MyGE::DX12App {
 public:
  Editor(HINSTANCE hInstance);
  ~Editor();

  bool Initialize();

 private:
  void OnResize();
  virtual void Update() override;
  virtual void Draw() override;

  void OnMouseDown(WPARAM btnState, int x, int y);
  void OnMouseUp(WPARAM btnState, int x, int y);
  void OnMouseMove(WPARAM btnState, int x, int y);

  virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam,
                          LPARAM lParam) override;

  void UpdateCamera();

  void BuildWorld();
  void LoadTextures();
  void BuildShaders();

 private:
  float mTheta = 0.4f * My::PI<float>;
  float mPhi = 1.3f * My::PI<float>;
  float mRadius = 5.0f;

  POINT mLastMousePos;

  My::MyECS::World world;
  std::unique_ptr<My::MyECS::EntityMngr> runningContext;
  My::MyECS::World editorWorld;
  My::MyECS::Entity editorSceneCamera{My::MyECS::Entity::Invalid()};

  void OnGameResize();
  size_t gameWidth, gameHeight;
  ImVec2 gamePos;
  ComPtr<ID3D12Resource> gameRT;
  const DXGI_FORMAT gameRTFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  My::MyDX12::DescriptorHeapAllocation gameRT_SRV;
  My::MyDX12::DescriptorHeapAllocation gameRT_RTV;
  std::unique_ptr<My::MyGE::IPipeline> gamePipeline;

  void OnEditorSceneResize();
  size_t editorSceneWidth, editorSceneHeight;
  ImVec2 editorScenePos;
  ComPtr<ID3D12Resource> editorSceneRT;
  const DXGI_FORMAT editorSceneRTFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  My::MyDX12::DescriptorHeapAllocation editorSceneRT_SRV;
  My::MyDX12::DescriptorHeapAllocation editorSceneRT_RTV;
  std::unique_ptr<My::MyGE::IPipeline> editorScenePipeline;

  bool show_demo_window = true;
  bool show_another_window = false;

  ImGuiContext* mainImGuiCtx = nullptr;
  ImGuiContext* gameImGuiCtx = nullptr;

  enum class GameState {
    NotStart,
    Starting,
    Running,
    Stopping,
  };

  GameState gameState = GameState::NotStart;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Shared(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Context(
    ImGuiContext* ctx, bool ingore_mouse, bool ingore_keyboard, HWND hWnd,
    UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Editor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler_Shared(hwnd, msg, wParam, lParam))
    return 1;

  bool imguiWantCaptureMouse = false;
  bool imguiWantCaptureKeyboard = false;

  if (gameImGuiCtx && mainImGuiCtx) {
    ImGui::SetCurrentContext(gameImGuiCtx);
    auto& gameIO = ImGui::GetIO();
    bool gameWantCaptureMouse = gameIO.WantCaptureMouse;
    bool gameWantCaptureKeyboard = gameIO.WantCaptureKeyboard;
    ImGui::SetCurrentContext(mainImGuiCtx);
    auto& mainIO = ImGui::GetIO();
    bool mainWantCaptureMouse = mainIO.WantCaptureMouse;
    bool mainWantCaptureKeyboard = mainIO.WantCaptureKeyboard;

    if (ImGui_ImplWin32_WndProcHandler_Context(gameImGuiCtx, false, false, hwnd,
                                               msg, wParam, lParam))
      return 1;

    if (ImGui_ImplWin32_WndProcHandler_Context(
            mainImGuiCtx, gameWantCaptureMouse, gameWantCaptureKeyboard, hwnd,
            msg, wParam, lParam))
      return 1;

    imguiWantCaptureMouse = gameWantCaptureMouse || mainWantCaptureMouse;
    imguiWantCaptureKeyboard =
        gameWantCaptureKeyboard || mainWantCaptureKeyboard;
  }

  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
  switch (msg) {
      // WM_ACTIVATE is sent when the window is activated or deactivated.
      // We pause the game when the window is deactivated and unpause it
      // when it becomes active.
    case WM_ACTIVATE:
      if (LOWORD(wParam) == WA_INACTIVE) {
        mAppPaused = true;
        My::MyGE::GameTimer::Instance().Stop();
      } else {
        mAppPaused = false;
        My::MyGE::GameTimer::Instance().Start();
      }
      return 0;

      // WM_SIZE is sent when the user resizes the window.
    case WM_SIZE:
      // Save the new client area dimensions.
      mClientWidth = LOWORD(lParam);
      mClientHeight = HIWORD(lParam);
      if (!myDevice.IsNull()) {
        if (wParam == SIZE_MINIMIZED) {
          mAppPaused = true;
          mMinimized = true;
          mMaximized = false;
        } else if (wParam == SIZE_MAXIMIZED) {
          mAppPaused = false;
          mMinimized = false;
          mMaximized = true;
          OnResize();
        } else if (wParam == SIZE_RESTORED) {

          // Restoring from minimized state?
          if (mMinimized) {
            mAppPaused = false;
            mMinimized = false;
            OnResize();
          }

          // Restoring from maximized state?
          else if (mMaximized) {
            mAppPaused = false;
            mMaximized = false;
            OnResize();
          } else if (mResizing) {
            // If user is dragging the resize bars, we do not resize
            // the buffers here because as the user continuously
            // drags the resize bars, a stream of WM_SIZE messages are
            // sent to the window, and it would be pointless (and slow)
            // to resize for each WM_SIZE message received from dragging
            // the resize bars.  So instead, we reset after the user is
            // done resizing the window and releases the resize bars, which
            // sends a WM_EXITSIZEMOVE message.
          } else  // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
          {
            OnResize();
          }
        }
      }
      return 0;

      // WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
    case WM_ENTERSIZEMOVE:
      mAppPaused = true;
      mResizing = true;
      My::MyGE::GameTimer::Instance().Stop();
      return 0;

      // WM_EXITSIZEMOVE is sent when the user releases the resize bars.
      // Here we reset everything based on the new window dimensions.
    case WM_EXITSIZEMOVE:
      mAppPaused = false;
      mResizing = false;
      My::MyGE::GameTimer::Instance().Start();
      OnResize();
      return 0;

      // WM_DESTROY is sent when the window is being destroyed.
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

      // The WM_MENUCHAR message is sent when a menu is active and the user presses
      // a key that does not correspond to any mnemonic or accelerator key.
    case WM_MENUCHAR:
      // Don't beep when we alt-enter.
      return MAKELRESULT(0, MNC_CLOSE);

      // Catch this message so to prevent the window from becoming too small.
    case WM_GETMINMAXINFO:
      ((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
      ((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
      return 0;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
      if (imguiWantCaptureMouse)
        return 0;
      OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      if (imguiWantCaptureMouse)
        return 0;
      OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_MOUSEMOVE:
      if (imguiWantCaptureMouse)
        return 0;
      OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_KEYUP:
      if (imguiWantCaptureKeyboard)
        return 0;
      if (wParam == VK_ESCAPE) {
        PostQuitMessage(0);
      }

      return 0;
  }

  return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  int rst;
  try {
    Editor theApp(hInstance);
    if (!theApp.Initialize())
      return 1;

    rst = theApp.Run();
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    rst = 1;
  }

#ifdef _DEBUG
  ComPtr<IDXGIDebug> debug;
  DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug));
  debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif  // _DEBUG

  return rst;
}

Editor::Editor(HINSTANCE hInstance) : DX12App(hInstance) {}

Editor::~Editor() {
  if (!myDevice.IsNull())
    FlushCommandQueue();

  My::MyGE::ImGUIMngr::Instance().Clear();

  if (!gameRT_SRV.IsNull())
    My::MyDX12::DescriptorHeapMngr::Instance().GetCSUGpuDH()->Free(
        std::move(gameRT_SRV));
  if (!gameRT_RTV.IsNull())
    My::MyDX12::DescriptorHeapMngr::Instance().GetRTVCpuDH()->Free(
        std::move(gameRT_RTV));
  if (!editorSceneRT_SRV.IsNull())
    My::MyDX12::DescriptorHeapMngr::Instance().GetCSUGpuDH()->Free(
        std::move(editorSceneRT_SRV));
  if (!editorSceneRT_RTV.IsNull())
    My::MyDX12::DescriptorHeapMngr::Instance().GetRTVCpuDH()->Free(
        std::move(editorSceneRT_RTV));

  if (gameState == GameState::Running) {
    world.entityMngr.Swap(*runningContext);
    runningContext.reset();
  }
}

bool Editor::Initialize() {
  if (!InitMainWindow())
    return false;

  if (!InitDirect3D())
    return false;

  gameRT_SRV =
      My::MyDX12::DescriptorHeapMngr::Instance().GetCSUGpuDH()->Allocate(1);
  gameRT_RTV =
      My::MyDX12::DescriptorHeapMngr::Instance().GetRTVCpuDH()->Allocate(1);
  editorSceneRT_SRV =
      My::MyDX12::DescriptorHeapMngr::Instance().GetCSUGpuDH()->Allocate(1);
  editorSceneRT_RTV =
      My::MyDX12::DescriptorHeapMngr::Instance().GetRTVCpuDH()->Allocate(1);

  My::MyGE::MeshLayoutMngr::Instance().Init();

  My::MyGE::ImGUIMngr::Instance().Init(MainWnd(), myDevice.Get(),
                                       NumFrameResources, 2);
  mainImGuiCtx = My::MyGE::ImGUIMngr::Instance().GetContexts().at(0);
  gameImGuiCtx = My::MyGE::ImGUIMngr::Instance().GetContexts().at(1);
  ImGui::SetCurrentContext(gameImGuiCtx);
  ImGui::GetIO().IniFilename = nullptr;

  My::MyGE::AssetMngr::Instance().ImportAssetRecursively(L"..\\assets");

  BuildWorld();

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();
  LoadTextures();
  BuildShaders();
  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(myCmdQueue.Get());

  //OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());

  My::MyGE::IPipeline::InitDesc initDesc;
  initDesc.device = myDevice.Get();
  initDesc.rtFormat = gameRTFormat;
  initDesc.cmdQueue = myCmdQueue.Get();
  initDesc.numFrame = NumFrameResources;
  gamePipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);
  editorScenePipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);

  // Do the initial resize code.
  OnResize();

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void Editor::OnResize() {
  DX12App::OnResize();
}

void Editor::OnGameResize() {
  My::rgbaf background = {0.f, 0.f, 0.f, 1.f};
  auto rtType = My::MyDX12::FG::RsrcType::RT2D(gameRTFormat, gameWidth,
                                               gameHeight, background.data());
  ThrowIfFailed(myDevice->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
      &rtType.desc, D3D12_RESOURCE_STATE_PRESENT, &rtType.clearValue,
      IID_PPV_ARGS(gameRT.ReleaseAndGetAddressOf())));
  myDevice->CreateShaderResourceView(gameRT.Get(), nullptr,
                                     gameRT_SRV.GetCpuHandle());
  myDevice->CreateRenderTargetView(gameRT.Get(), nullptr,
                                   gameRT_RTV.GetCpuHandle());

  assert(gamePipeline);
  D3D12_VIEWPORT viewport;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = static_cast<float>(gameWidth);
  viewport.Height = static_cast<float>(gameHeight);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  gamePipeline->Resize(gameWidth, gameHeight, viewport,
                       {0, 0, (LONG)gameWidth, (LONG)gameHeight});
}

void Editor::OnEditorSceneResize() {
  My::rgbaf background = {0.f, 0.f, 0.f, 1.f};
  auto rtType =
      My::MyDX12::FG::RsrcType::RT2D(editorSceneRTFormat, editorSceneWidth,
                                     editorSceneHeight, background.data());
  ThrowIfFailed(myDevice->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
      &rtType.desc, D3D12_RESOURCE_STATE_PRESENT, &rtType.clearValue,
      IID_PPV_ARGS(editorSceneRT.ReleaseAndGetAddressOf())));
  myDevice->CreateShaderResourceView(editorSceneRT.Get(), nullptr,
                                     editorSceneRT_SRV.GetCpuHandle());
  myDevice->CreateRenderTargetView(editorSceneRT.Get(), nullptr,
                                   editorSceneRT_RTV.GetCpuHandle());

  assert(editorScenePipeline);
  D3D12_VIEWPORT viewport;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = static_cast<float>(editorSceneWidth);
  viewport.Height = static_cast<float>(editorSceneHeight);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  editorScenePipeline->Resize(
      editorSceneWidth, editorSceneHeight, viewport,
      {0, 0, (LONG)editorSceneWidth, (LONG)editorSceneHeight});
}

void Editor::Update() {
  // Start the Dear ImGui frame
  ImGui_ImplDX12_NewFrame();
  ImGui_ImplWin32_NewFrame_Context(mainImGuiCtx, {0.f, 0.f}, mClientWidth,
                                   mClientHeight);
  ImGui_ImplWin32_NewFrame_Context(gameImGuiCtx, gamePos, gameWidth,
                                   gameHeight);
  ImGui_ImplWin32_NewFrame_Shared();

  auto& upload = My::MyGE::RsrcMngrDX12::Instance().GetUpload();
  upload.Begin();

  ImGui::SetCurrentContext(mainImGuiCtx);
  ImGui::NewFrame();  // main ctx

  UpdateCamera();
  editorWorld.Update();

  // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
  if (show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin(
        "Hello, world!");  // Create a window called "Hello, world!" and append into it.

    ImGui::Text(
        "This is some useful text.");  // Display some text (you can use a format strings too)
    ImGui::Checkbox(
        "Demo Window",
        &show_demo_window);  // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f,
                       1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
    //ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

    if (ImGui::Button(
            "Button"))  // Buttons return true when clicked (most widgets return true when edited/activated)
      counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
  }

  // 3. Show another simple window.
  if (show_another_window) {
    ImGui::Begin(
        "Another Window",
        &show_another_window);  // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me"))
      show_another_window = false;
    ImGui::End();
  }

  bool isFlush = false;
  // 4. game window
  if (ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoScrollbar)) {
    auto content_max_minus_local_pos = ImGui::GetContentRegionAvail();
    auto content_max = ImGui::GetWindowContentRegionMax();
    auto game_window_pos = ImGui::GetWindowPos();
    gamePos.x =
        game_window_pos.x + content_max.x - content_max_minus_local_pos.x;
    gamePos.y =
        game_window_pos.y + content_max.y - content_max_minus_local_pos.y;

    auto gameSize = ImGui::GetContentRegionAvail();
    auto w = (size_t)gameSize.x;
    auto h = (size_t)gameSize.y;
    if (gameSize.x > 0 && gameSize.y > 0 && w > 0 && h > 0 &&
        (w != gameWidth || h != gameHeight)) {
      gameWidth = w;
      gameHeight = h;

      if (!isFlush) {
        // Flush before changing any resources.
        FlushCommandQueue();
        isFlush = true;
      }

      OnGameResize();
    }
    ImGui::Image(ImTextureID(gameRT_SRV.GetGpuHandle().ptr),
                 ImGui::GetContentRegionAvail());
  }
  ImGui::End();  // game window

  // 5. editorScene window
  if (ImGui::Begin("Editor Scene", nullptr, ImGuiWindowFlags_NoScrollbar)) {
    auto content_max_minus_local_pos = ImGui::GetContentRegionAvail();
    auto content_max = ImGui::GetWindowContentRegionMax();
    auto editorScene_window_pos = ImGui::GetWindowPos();
    editorScenePos.x = editorScene_window_pos.x + content_max.x -
                       content_max_minus_local_pos.x;
    editorScenePos.y = editorScene_window_pos.y + content_max.y -
                       content_max_minus_local_pos.y;

    auto editorSceneSize = ImGui::GetContentRegionAvail();
    auto w = (size_t)editorSceneSize.x;
    auto h = (size_t)editorSceneSize.y;
    if (editorSceneSize.x > 0 && editorSceneSize.y > 0 && w > 0 && h > 0 &&
        (w != editorSceneWidth || h != editorSceneHeight)) {
      editorSceneWidth = w;
      editorSceneHeight = h;

      if (!isFlush) {
        // Flush before changing any resources.
        FlushCommandQueue();
        isFlush = true;
      }

      OnEditorSceneResize();
    }
    ImGui::Image(ImTextureID(editorSceneRT_SRV.GetGpuHandle().ptr),
                 ImGui::GetContentRegionAvail());
  }
  ImGui::End();  // editorScene window

  // 6.game control
  if (ImGui::Begin("Game Control", nullptr, ImGuiWindowFlags_NoScrollbar)) {
    static std::string startStr = "start";
    if (ImGui::Button(startStr.c_str())) {
      switch (gameState) {
        case GameState::NotStart:
          startStr = "stop";
          gameState = GameState::Starting;
          break;
        case GameState::Running:
          startStr = "start";
          gameState = GameState::Stopping;
          break;
        case GameState::Starting:
        case GameState::Stopping:
        default:
          assert("error" && false);
          break;
      }
    }
  }
  ImGui::End();  // Game Control window

  {  // game update
    ImGui::SetCurrentContext(gameImGuiCtx);
    ImGui::NewFrame();  // game ctx

    switch (gameState) {
      case GameState::NotStart:
        break;
      case GameState::Starting:
        runningContext =
            std::make_unique<My::MyECS::EntityMngr>(world.entityMngr);
        world.entityMngr.Swap(*runningContext);
        gameState = GameState::Running;
        // break;
      case GameState::Running:
        world.Update();
        ImGui::Begin("in game");
        ImGui::Text(
            "This is some useful text.");  // Display some text (you can use a format strings too)
        ImGui::End();
        break;
      case GameState::Stopping:
        world.entityMngr.Swap(*runningContext);
        runningContext.reset();
        gameState = GameState::NotStart;
        break;
      default:
        break;
    }
  }

  ImGui::SetCurrentContext(nullptr);

  // update mesh, texture ...
  GetFrameResourceMngr()->BeginFrame();

  auto cmdAlloc = GetCurFrameCommandAllocator();
  cmdAlloc->Reset();

  ThrowIfFailed(myGCmdList->Reset(cmdAlloc, nullptr));
  auto& deleteBatch = My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch();

  world.RunEntityJob(
      [&](const My::MyGE::MeshFilter* meshFilter,
          const My::MyGE::MeshRenderer* meshRenderer) {
        if (!meshFilter->mesh || meshRenderer->materials.empty())
          return;

        My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
            upload, deleteBatch, myGCmdList.Get(), meshFilter->mesh);

        for (const auto& mat : meshRenderer->materials) {
          for (const auto& [name, tex] : mat->texture2Ds) {
            My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
                My::MyGE::RsrcMngrDX12::Instance().GetUpload(), tex);
          }
          for (const auto& [name, tex] : mat->textureCubes) {
            My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
                My::MyGE::RsrcMngrDX12::Instance().GetUpload(), tex);
          }
        }
      },
      false);

  if (auto skybox = world.entityMngr.GetSingleton<My::MyGE::Skybox>()) {
    for (const auto& [name, tex] : skybox->material->texture2Ds) {
      My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
          My::MyGE::RsrcMngrDX12::Instance().GetUpload(), tex);
    }
    for (const auto& [name, tex] : skybox->material->textureCubes) {
      My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
          My::MyGE::RsrcMngrDX12::Instance().GetUpload(), tex);
    }
  }

  // commit upload, delete ...
  upload.End(myCmdQueue.Get());
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.Get());
  deleteBatch.Commit(myDevice.Get(), myCmdQueue.Get());

  std::vector<My::MyGE::IPipeline::CameraData> gameCameras;
  My::MyECS::ArchetypeFilter camFilter{
      {My::MyECS::CmptAccessType::Of<My::MyGE::Camera>}};
  world.RunEntityJob(
      [&](My::MyECS::Entity e) { gameCameras.emplace_back(e, world); }, false,
      camFilter);
  assert(gameCameras.size() == 1);  // now only support 1 camera
  gamePipeline->BeginFrame(world, gameCameras.front());

  std::vector<My::MyGE::IPipeline::CameraData> editorSceneCameras;
  editorSceneCameras.emplace_back(editorSceneCamera, editorWorld);
  assert(editorSceneCameras.size() == 1);  // now only support 1 camera
  editorScenePipeline->BeginFrame(world, editorSceneCameras.front());
}

void Editor::Draw() {
  auto cmdAlloc = GetCurFrameCommandAllocator();
  ThrowIfFailed(myGCmdList->Reset(cmdAlloc, nullptr));

  ImGui::SetCurrentContext(gameImGuiCtx);
  if (gameRT) {
    gamePipeline->Render(gameRT.Get());
    myGCmdList.ResourceBarrierTransition(gameRT.Get(),
                                         D3D12_RESOURCE_STATE_PRESENT,
                                         D3D12_RESOURCE_STATE_RENDER_TARGET);
    myGCmdList->OMSetRenderTargets(1, &gameRT_RTV.GetCpuHandle(), FALSE, NULL);
    myGCmdList.SetDescriptorHeaps(My::MyDX12::DescriptorHeapMngr::Instance()
                                      .GetCSUGpuDH()
                                      ->GetDescriptorHeap());
    ImGui::Render();
    ImGui_ImplDX12_RenderDrawData(1, ImGui::GetDrawData(), myGCmdList.Get());
    myGCmdList.ResourceBarrierTransition(gameRT.Get(),
                                         D3D12_RESOURCE_STATE_RENDER_TARGET,
                                         D3D12_RESOURCE_STATE_PRESENT);
  } else
    ImGui::EndFrame();

  ImGui::SetCurrentContext(mainImGuiCtx);
  if (editorSceneRT)
    editorScenePipeline->Render(editorSceneRT.Get());

  myGCmdList.ResourceBarrierTransition(CurrentBackBuffer(),
                                       D3D12_RESOURCE_STATE_PRESENT,
                                       D3D12_RESOURCE_STATE_RENDER_TARGET);
  myGCmdList->ClearRenderTargetView(CurrentBackBufferView(),
                                    DirectX::Colors::Black, 0, NULL);
  myGCmdList->OMSetRenderTargets(1, &CurrentBackBufferView(), FALSE, NULL);
  myGCmdList.SetDescriptorHeaps(My::MyDX12::DescriptorHeapMngr::Instance()
                                    .GetCSUGpuDH()
                                    ->GetDescriptorHeap());
  ImGui::Render();
  ImGui_ImplDX12_RenderDrawData(0, ImGui::GetDrawData(), myGCmdList.Get());
  myGCmdList.ResourceBarrierTransition(CurrentBackBuffer(),
                                       D3D12_RESOURCE_STATE_RENDER_TARGET,
                                       D3D12_RESOURCE_STATE_PRESENT);

  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.Get());

  SwapBackBuffer();

  gamePipeline->EndFrame();
  editorScenePipeline->EndFrame();
  GetFrameResourceMngr()->EndFrame(myCmdQueue.Get());
  ImGui_ImplDX12_EndFrame();
}

void Editor::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(MainWnd());
}

void Editor::OnMouseUp(WPARAM btnState, int x, int y) {
  ReleaseCapture();
}

void Editor::OnMouseMove(WPARAM btnState, int x, int y) {
  if ((btnState & MK_LBUTTON) != 0) {
    // Make each pixel correspond to a quarter of a degree.
    float dx = My::to_radian(0.25f * static_cast<float>(x - mLastMousePos.x));
    float dy = My::to_radian(0.25f * static_cast<float>(y - mLastMousePos.y));

    // Update angles based on input to orbit camera around box.
    mTheta -= dy;
    mPhi -= dx;

    // Restrict the angle mPhi.
    mTheta = std::clamp(mTheta, 0.1f, My::PI<float> - 0.1f);
  } else if ((btnState & MK_RBUTTON) != 0) {
    // Make each pixel correspond to 0.2 unit in the scene.
    float dx = 0.05f * static_cast<float>(x - mLastMousePos.x);
    float dy = 0.05f * static_cast<float>(y - mLastMousePos.y);

    // Update the camera radius based on input.
    mRadius += dx - dy;

    // Restrict the radius.
    mRadius = std::clamp(mRadius, 5.0f, 150.0f);
  }

  mLastMousePos.x = x;
  mLastMousePos.y = y;
}

void Editor::UpdateCamera() {
  My::vecf3 eye = {mRadius * sinf(mTheta) * sinf(mPhi), mRadius * cosf(mTheta),
                   mRadius * sinf(mTheta) * cosf(mPhi)};
  auto camera = editorWorld.entityMngr.Get<My::MyGE::Camera>(editorSceneCamera);
  camera->aspect = gameWidth / (float)gameHeight;
  auto view =
      My::transformf::look_at(eye.as<My::pointf3>(), {0.f});  // world to camera
  auto c2w = view.inverse();
  editorWorld.entityMngr.Get<My::MyGE::Translation>(editorSceneCamera)->value =
      eye;
  editorWorld.entityMngr.Get<My::MyGE::Rotation>(editorSceneCamera)->value =
      c2w.decompose_quatenion();
}

void Editor::BuildWorld() {
  My::MyGE::CmptInspector::Instance()
      .RegisterCmpts<
          // core
          My::MyGE::Camera, My::MyGE::MeshFilter, My::MyGE::MeshRenderer,
          My::MyGE::WorldTime, My::MyGE::Name, My::MyGE::Skybox,

          // transform
          My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
          My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
          My::MyGE::Scale, My::MyGE::Translation, My::MyGE::WorldToLocal,

          My::MyGE::TestInspector>();

  editorWorld.systemMngr.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      My::MyGE::WorldTimeSystem,

      My::MyGE::HierarchySystem, My::MyGE::InspectorSystem,
      My::MyGE::ProjectViewerSystem>();
  editorWorld.cmptTraits.Register<
      // core
      My::MyGE::Camera, My::MyGE::MeshFilter, My::MyGE::MeshRenderer,
      My::MyGE::WorldTime, My::MyGE::Name, My::MyGE::Skybox,

      // transform
      My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
      My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
      My::MyGE::Scale, My::MyGE::Translation, My::MyGE::WorldToLocal,

      // editor
      My::MyGE::Hierarchy, My::MyGE::Inspector, My::MyGE::ProjectViewer,
      My::MyGE::TestInspector>();

  {  // editor camera
    auto [e, l2w, w2l, cam, t, rot] = editorWorld.entityMngr.Create<
        My::MyGE::LocalToWorld, My::MyGE::WorldToLocal, My::MyGE::Camera,
        My::MyGE::Translation, My::MyGE::Rotation>();
    editorSceneCamera = e;
  }
  {  // hierarchy
    auto [e, hierarchy] = editorWorld.entityMngr.Create<My::MyGE::Hierarchy>();
    hierarchy->world = &world;
  }
  {  // inspector, project viewer
    editorWorld.entityMngr.Create<My::MyGE::Inspector>();
    editorWorld.entityMngr.Create<My::MyGE::ProjectViewer>();
  }

  world.systemMngr.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      My::MyGE::WorldTimeSystem>();
  world.cmptTraits.Register<
      // core
      My::MyGE::Camera, My::MyGE::MeshFilter, My::MyGE::MeshRenderer,
      My::MyGE::WorldTime, My::MyGE::Name, My::MyGE::Skybox,

      // transform
      My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
      My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
      My::MyGE::Scale, My::MyGE::Translation, My::MyGE::WorldToLocal,

      My::MyGE::TestInspector>();

  {  // test inspector

    auto [e, test, name] =
        world.entityMngr.Create<My::MyGE::TestInspector, My::MyGE::Name>();
    name->value = "Test Inspector";
  }

  /*world.entityMngr.Create<My::MyGE::WorldTime>();

	auto e0 = world.entityMngr.Create<
		My::MyGE::LocalToWorld,
		My::MyGE::WorldToLocal,
		My::MyGE::Camera,
		My::MyGE::Translation,
		My::MyGE::Rotation
	>();
	cam = std::get<My::MyECS::Entity>(e0);

	auto quadMesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>("../assets/models/quad.obj");
	auto dynamicCube = world.entityMngr.Create<
		My::MyGE::LocalToWorld,
		My::MyGE::MeshFilter,
		My::MyGE::MeshRenderer,
		My::MyGE::Translation,
		My::MyGE::Rotation,
		My::MyGE::Scale
	>();
	std::get<My::MyGE::MeshFilter*>(dynamicCube)->mesh = quadMesh;*/

  My::MyGE::Serializer::Instance()
      .Register<
          // core
          My::MyGE::Camera, My::MyGE::MeshFilter, My::MyGE::MeshRenderer,
          My::MyGE::WorldTime, My::MyGE::Name, My::MyGE::Skybox,

          // transform
          My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
          My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
          My::MyGE::Scale, My::MyGE::Translation, My::MyGE::WorldToLocal,

          My::MyGE::TestInspector>();
  //OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());
  auto scene = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Scene>(
      L"..\\assets\\scenes\\Game.scene");
  My::MyGE::Serializer::Instance().ToWorld(&world, scene->GetText());
  OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());

  auto mainLua = My::MyGE::LuaCtxMngr::Instance().Register(&world)->Main();
  sol::state_view solLua(mainLua);
  auto gameLuaScript =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::LuaScript>(
          L"..\\assets\\scripts\\Game.lua");
  solLua["world"] = &world;
  solLua.script(gameLuaScript->GetText());
}

void Editor::LoadTextures() {
  auto tex2dGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
        My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(path));
  }

  auto texcubeGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.texcube)"});
  for (const auto& guid : texcubeGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
        My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::TextureCube>(path));
  }
}

void Editor::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::RsrcMngrDX12::Instance().RegisterShader(shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}
