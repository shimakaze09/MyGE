//
// Created by Admin on 17/03/2025.
//

#include <MyGE/App/DX12App/DX12App.h>
#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Asset/Serializer.h>
#include <MyGE/Core/Components/Camera.h>
#include <MyGE/Core/Components/MeshFilter.h>
#include <MyGE/Core/Components/MeshRenderer.h>
#include <MyGE/Core/Components/WorldTime.h>
#include <MyGE/Core/GameTimer.h>
#include <MyGE/Core/HLSLFile.h>
#include <MyGE/Core/ImGUIMngr.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Mesh.h>
#include <MyGE/Core/Scene.h>
#include <MyGE/Core/Shader.h>
#include <MyGE/Core/ShaderMngr.h>
#include <MyGE/Core/Systems/CameraSystem.h>
#include <MyGE/Core/Systems/WorldTimeSystem.h>
#include <MyGE/Core/Texture2D.h>
#include <MyGE/Render/DX12/MeshLayoutMngr.h>
#include <MyGE/Render/DX12/RsrcMngrDX12.h>
#include <MyGE/Render/DX12/ShaderCBMngrDX12.h>
#include <MyGE/Render/DX12/StdPipeline.h>
#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaCtxMngr.h>
#include <MyGE/ScriptSystem/LuaScript.h>
#include <MyGE/Transform/Transform.h>
#include <MyGE/_deps/imgui/imgui.h>
#include <MyGE/_deps/imgui/imgui_impl_dx12.h>
#include <MyGE/_deps/imgui/imgui_impl_win32.h>
#include <MyLuaPP/MyLuaPP.h>

using Microsoft::WRL::ComPtr;

class GameStarter : public My::MyGE::DX12App {
 public:
  GameStarter(HINSTANCE hInstance);
  ~GameStarter();

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
  void BuildMaterials();

 private:
  float mTheta = 0.4f * My::PI<float>;
  float mPhi = 1.3f * My::PI<float>;
  float mRadius = 5.0f;

  POINT mLastMousePos;

  My::MyECS::World world;
  My::MyECS::Entity cam{My::MyECS::Entity::Invalid()};

  std::unique_ptr<My::MyGE::IPipeline> pipeline;

  bool show_demo_window = true;
  bool show_another_window = false;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT GameStarter::MsgProc(HWND hwnd, UINT msg, WPARAM wParam,
                             LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) return true;
  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to
  // your main application.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data
  // to your main application.
  auto imgui_ctx = ImGui::GetCurrentContext();
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
          } else  // API call such as SetWindowPos or
                  // mSwapChain->SetFullscreenState.
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

      // The WM_MENUCHAR message is sent when a menu is active and the user
      // presses a key that does not correspond to any mnemonic or accelerator
      // key.
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
      if (imgui_ctx && ImGui::GetIO().WantCaptureMouse) return 0;
      OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
      if (imgui_ctx && ImGui::GetIO().WantCaptureMouse) return 0;
      OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_MOUSEMOVE:
      if (imgui_ctx && ImGui::GetIO().WantCaptureMouse) return 0;
      OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
      return 0;
    case WM_KEYUP:
      if (imgui_ctx && ImGui::GetIO().WantCaptureKeyboard) return 0;
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

  try {
    GameStarter theApp(hInstance);
    if (!theApp.Initialize()) return 1;

    int rst = theApp.Run();
    return rst;
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    return 1;
  }
}

GameStarter::GameStarter(HINSTANCE hInstance) : DX12App(hInstance) {}

GameStarter::~GameStarter() {
  if (!myDevice.IsNull()) FlushCommandQueue();

  My::MyGE::ImGUIMngr::Instance().Clear();
}

bool GameStarter::Initialize() {
  if (!InitMainWindow()) return false;

  if (!InitDirect3D()) return false;

  My::MyGE::MeshLayoutMngr::Instance().Init();

  My::MyGE::ImGUIMngr::Instance().Init(MainWnd(), myDevice.Get(),
                                       NumFrameResources);

  My::MyGE::AssetMngr::Instance().ImportAssetRecursively(LR"(..\\assets)");

  BuildWorld();

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();
  LoadTextures();
  BuildShaders();
  BuildMaterials();
  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(myCmdQueue.Get());

  // OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());

  My::MyGE::IPipeline::InitDesc initDesc;
  initDesc.device = myDevice.Get();
  initDesc.rtFormat = GetBackBufferFormat();
  initDesc.cmdQueue = myCmdQueue.Get();
  initDesc.numFrame = NumFrameResources;
  pipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);

  // Do the initial resize code.
  OnResize();

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void GameStarter::OnResize() {
  DX12App::OnResize();

  assert(pipeline);
  pipeline->Resize(mClientWidth, mClientHeight, GetScreenViewport(),
                   GetScissorRect());
}

void GameStarter::Update() {
  UpdateCamera();

  world.Update();

  // update mesh, texture ...
  GetFrameResourceMngr()->BeginFrame();

  auto cmdAlloc = GetCurFrameCommandAllocator();
  cmdAlloc->Reset();

  ThrowIfFailed(myGCmdList->Reset(cmdAlloc, nullptr));
  auto& upload = My::MyGE::RsrcMngrDX12::Instance().GetUpload();
  auto& deleteBatch = My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch();
  upload.Begin();

  // update mesh
  world.RunEntityJob(
      [&](const My::MyGE::MeshFilter* meshFilter) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
            upload, deleteBatch, myGCmdList.Get(), meshFilter->mesh);
      },
      false);

  // commit upload, delete ...
  upload.End(myCmdQueue.Get());
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.Get());
  deleteBatch.Commit(myDevice.Get(), myCmdQueue.Get());

  pipeline->UpdateRenderContext(world);
}

void GameStarter::Draw() {
  // Start the Dear ImGui frame
  ImGui_ImplDX12_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();

  // 1. Show the big demo window (Most of the sample code is in
  // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
  // ImGui!).
  if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

  // 2. Show a simple window that we create ourselves. We use a Begin/End pair
  // to created a named window.
  {
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!"
                                    // and append into it.

    ImGui::Text("This is some useful text.");  // Display some text (you can use
                                               // a format strings too)
    ImGui::Checkbox(
        "Demo Window",
        &show_demo_window);  // Edit bools storing our window open/close state
    ImGui::Checkbox("Another Window", &show_another_window);

    ImGui::SliderFloat("float", &f, 0.0f,
                       1.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
    // ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats
    // representing a color

    if (ImGui::Button("Button"))  // Buttons return true when clicked (most
                                  // widgets return true when edited/activated)
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
        &show_another_window);  // Pass a pointer to our bool variable (the
                                // window will have a closing button that will
                                // clear the bool when clicked)
    ImGui::Text("Hello from another window!");
    if (ImGui::Button("Close Me")) show_another_window = false;
    ImGui::End();
  }

  pipeline->Render(CurrentBackBuffer());

  auto cmdAlloc = GetCurFrameCommandAllocator();
  ThrowIfFailed(myGCmdList->Reset(cmdAlloc, nullptr));
  myGCmdList.ResourceBarrierTransition(CurrentBackBuffer(),
                                       D3D12_RESOURCE_STATE_PRESENT,
                                       D3D12_RESOURCE_STATE_RENDER_TARGET);
  myGCmdList->OMSetRenderTargets(1, &CurrentBackBufferView(), FALSE, NULL);
  myGCmdList.SetDescriptorHeaps(My::MyDX12::DescriptorHeapMngr::Instance()
                                    .GetCSUGpuDH()
                                    ->GetDescriptorHeap());
  ImGui::Render();
  ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), myGCmdList.Get());
  myGCmdList.ResourceBarrierTransition(CurrentBackBuffer(),
                                       D3D12_RESOURCE_STATE_RENDER_TARGET,
                                       D3D12_RESOURCE_STATE_PRESENT);
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.Get());

  SwapBackBuffer();

  pipeline->EndFrame();
  GetFrameResourceMngr()->EndFrame(myCmdQueue.Get());
}

void GameStarter::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(MainWnd());
}

void GameStarter::OnMouseUp(WPARAM btnState, int x, int y) { ReleaseCapture(); }

void GameStarter::OnMouseMove(WPARAM btnState, int x, int y) {
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

void GameStarter::UpdateCamera() {
  My::vecf3 eye = {mRadius * sinf(mTheta) * sinf(mPhi), mRadius * cosf(mTheta),
                   mRadius * sinf(mTheta) * cosf(mPhi)};
  auto camera = world.entityMngr.Get<My::MyGE::Camera>(cam);
  camera->aspect = AspectRatio();
  auto view =
      My::transformf::look_at(eye.as<My::pointf3>(), {0.f});  // world to camera
  auto c2w = view.inverse();
  world.entityMngr.Get<My::MyGE::Translation>(cam)->value = eye;
  world.entityMngr.Get<My::MyGE::Rotation>(cam)->value =
      c2w.decompose_quatenion();
}

void GameStarter::BuildWorld() {
  world.systemMngr.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      My::MyGE::WorldTimeSystem>();
  world.entityMngr.cmptTraits.Register<
      // core
      My::MyGE::Camera, My::MyGE::MeshFilter, My::MyGE::MeshRenderer,
      My::MyGE::WorldTime,

      // transform
      My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
      My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
      My::MyGE::Scale, My::MyGE::Translation, My::MyGE::WorldToLocal>();

  /*world.entityMngr.Create<My::MyGE::WorldTime>();

  auto e0 = world.entityMngr.Create<
          My::MyGE::LocalToWorld,
          My::MyGE::WorldToLocal,
          My::MyGE::Camera,
          My::MyGE::Translation,
          My::MyGE::Rotation
  >();
  cam = std::get<My::MyECS::Entity>(e0);

  auto quadMesh =
  My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>("../assets/models/quad.obj");
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
          My::MyGE::WorldTime,

          // transform
          My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
          My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
          My::MyGE::Scale, My::MyGE::Translation, My::MyGE::WorldToLocal>();
  // OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());
  auto scene = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Scene>(
      L"..\\assets\\scenes\\Game.scene");
  My::MyGE::Serializer::Instance().ToWorld(&world, scene->GetText());
  cam = world.entityMngr
            .GetEntityArray({{My::MyECS::CmptAccessType::Of<My::MyGE::Camera>}})
            .front();
  OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());

  auto mainLua = My::MyGE::LuaCtxMngr::Instance().Register(&world)->Main();
  sol::state_view solLua(mainLua);
  auto gameLuaScript =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::LuaScript>(
          L"..\\assets\\scripts\\Game.lua");
  solLua["world"] = &world;
  solLua.script(gameLuaScript->GetText());
}

void GameStarter::LoadTextures() {
  auto tex2dGUIDs =
      My::MyGE::AssetMngr::Instance().FindAssets(std::wregex{LR"(.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
        My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(path));
  }
}

void GameStarter::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::RsrcMngrDX12::Instance().RegisterShader(shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}

void GameStarter::BuildMaterials() {
  /*auto material = My::MyGE::AssetMngr::Instance()
          .LoadAsset<My::MyGE::Material>(L"..\\assets\\materials\\iron.mat");
  world.RunEntityJob([=](My::MyGE::MeshRenderer* meshRenderer) {
          meshRenderer->materials.push_back(material);
  });*/
}
