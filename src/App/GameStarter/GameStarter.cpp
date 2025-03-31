#include <MyGE/App/DX12App/DX12App.h>

#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Asset/Serializer.h>

#include <MyGE/Render/Components/Components.h>
#include <MyGE/Render/DX12/RsrcMngrDX12.h>
#include <MyGE/Render/DX12/StdPipeline.h>
#include <MyGE/Render/HLSLFile.h>
#include <MyGE/Render/Mesh.h>
#include <MyGE/Render/Shader.h>
#include <MyGE/Render/ShaderMngr.h>
#include <MyGE/Render/Systems/Systems.h>
#include <MyGE/Render/Texture2D.h>
#include <MyGE/Render/TextureCube.h>

#include <MyGE/Core/Components/Components.h>
#include <MyGE/Core/GameTimer.h>
#include <MyGE/Core/ImGUIMngr.h>
#include <MyGE/Core/Scene.h>
#include <MyGE/Core/Systems/Systems.h>

#include <_deps/imgui/imgui.h>
#include <_deps/imgui/imgui_impl_dx12.h>
#include <_deps/imgui/imgui_impl_win32.h>

#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaCtxMngr.h>
#include <MyGE/ScriptSystem/LuaScript.h>
#include <MyGE/ScriptSystem/LuaScriptQueue.h>
#include <MyGE/ScriptSystem/LuaScriptQueueSystem.h>

#include <MyLuaPP/MyLuaPP.h>

#include <dxgidebug.h>

using Microsoft::WRL::ComPtr;

class GameStarter : public My::MyGE::DX12App {
 public:
  GameStarter(HINSTANCE hInstance);
  ~GameStarter();

  virtual bool Init() override;

 private:
  virtual void OnResize() override;
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
  My::MyECS::Entity cam{My::MyECS::Entity::Invalid()};

  std::unique_ptr<My::MyGE::PipelineBase> pipeline;

  bool show_demo_window = true;
  bool show_another_window = false;

  ImGuiContext* gameImGuiCtx = nullptr;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Shared(
    HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler_Context(
    ImGuiContext* ctx, bool ingore_mouse, bool ingore_keyboard, HWND hWnd,
    UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT GameStarter::MsgProc(HWND hwnd, UINT msg, WPARAM wParam,
                             LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler_Shared(hwnd, msg, wParam, lParam))
    return 1;

  bool imguiWantCaptureMouse = false;
  bool imguiWantCaptureKeyboard = false;

  if (ImGui::GetCurrentContext()) {
    auto& gameIO = ImGui::GetIO();
    bool gameWantCaptureMouse = gameIO.WantCaptureMouse;
    bool gameWantCaptureKeyboard = gameIO.WantCaptureKeyboard;
    if (ImGui_ImplWin32_WndProcHandler_Context(ImGui::GetCurrentContext(),
                                               false, false, hwnd, msg, wParam,
                                               lParam))
      return 1;

    imguiWantCaptureMouse = gameWantCaptureMouse;
    imguiWantCaptureKeyboard = gameWantCaptureKeyboard;
  }

  // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
  // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
  auto imgui_ctx = ImGui::GetCurrentContext();
  switch (msg) {
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

  return DX12App::MsgProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  int rst;
  try {
    GameStarter theApp(hInstance);
    if (!theApp.Init())
      return 1;

    rst = theApp.Run();
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    return 1;
  }

#ifdef _DEBUG
  ComPtr<IDXGIDebug> debug;
  DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug));
  if (debug)
    debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif  // _DEBUG

  return rst;
}

GameStarter::GameStarter(HINSTANCE hInstance) : DX12App(hInstance) {}

GameStarter::~GameStarter() {
  if (!myDevice.IsNull())
    FlushCommandQueue();

  My::MyGE::ImGUIMngr::Instance().Clear();
}

bool GameStarter::Init() {
  if (!InitMainWindow())
    return false;

  if (!InitDirect3D())
    return false;

  My::MyGE::ImGUIMngr::Instance().Init(MainWnd(), myDevice.Get(),
                                       NumFrameResources, 1);
  gameImGuiCtx = My::MyGE::ImGUIMngr::Instance().GetContexts().at(0);

  My::MyGE::AssetMngr::Instance().ImportAssetRecursively(L"..\\assets");

  BuildWorld();

  LoadTextures();
  BuildShaders();

  My::MyGE::PipelineBase::InitDesc initDesc;
  initDesc.device = myDevice.Get();
  initDesc.rtFormat = GetBackBufferFormat();
  initDesc.cmdQueue = myCmdQueue.Get();
  initDesc.numFrame = NumFrameResources;
  pipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);

  My::MyGE::RsrcMngrDX12::Instance().CommitUploadAndDelete(myCmdQueue.Get());

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
  // Start the Dear ImGui frame
  ImGui_ImplDX12_NewFrame();
  ImGui_ImplWin32_NewFrame_Context(gameImGuiCtx, {0, 0}, (float)mClientWidth,
                                   (float)mClientHeight);
  ImGui_ImplWin32_NewFrame_Shared();

  ImGui::SetCurrentContext(gameImGuiCtx);
  ImGui::NewFrame();

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

  UpdateCamera();

  world.Update();

  ImGui::SetCurrentContext(nullptr);

  // update mesh, texture ...
  GetFrameResourceMngr()->BeginFrame();

  auto cmdAlloc = GetCurFrameCommandAllocator();
  cmdAlloc->Reset();

  ThrowIfFailed(myGCmdList->Reset(cmdAlloc, nullptr));
  auto& deleteBatch = My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch();

  world.RunEntityJob(
      [&](My::MyGE::MeshFilter* meshFilter,
          const My::MyGE::MeshRenderer* meshRenderer) {
        if (!meshFilter->mesh || meshRenderer->materials.empty())
          return;

        My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(myGCmdList.Get(),
                                                        *meshFilter->mesh);

        for (const auto& material : meshRenderer->materials) {
          if (!material)
            continue;
          for (const auto& [name, property] : material->properties) {
            if (std::holds_alternative<
                    std::shared_ptr<const My::MyGE::Texture2D>>(property)) {
              My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
                  *std::get<std::shared_ptr<const My::MyGE::Texture2D>>(
                      property));
            } else if (std::holds_alternative<
                           std::shared_ptr<const My::MyGE::TextureCube>>(
                           property)) {
              My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
                  *std::get<std::shared_ptr<const My::MyGE::TextureCube>>(
                      property));
            }
          }
        }
      },
      false);

  if (auto skybox = world.entityMngr.GetSingleton<My::MyGE::Skybox>();
      skybox && skybox->material) {
    for (const auto& [name, property] : skybox->material->properties) {
      if (std::holds_alternative<std::shared_ptr<const My::MyGE::Texture2D>>(
              property)) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
            *std::get<std::shared_ptr<const My::MyGE::Texture2D>>(property));
      } else if (std::holds_alternative<
                     std::shared_ptr<const My::MyGE::TextureCube>>(property)) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
            *std::get<std::shared_ptr<const My::MyGE::TextureCube>>(property));
      }
    }
  }

  // commit upload, delete ...
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.Get());
  My::MyGE::RsrcMngrDX12::Instance().CommitUploadAndDelete(myCmdQueue.Get());

  std::vector<My::MyGE::PipelineBase::CameraData> gameCameras;
  My::MyECS::ArchetypeFilter camFilter{
      {My::MyECS::CmptAccessType::Of<My::MyGE::Camera>}};
  world.RunEntityJob(
      [&](My::MyECS::Entity e) { gameCameras.emplace_back(e, world); }, false,
      camFilter);
  assert(gameCameras.size() == 1);  // now only support 1 camera
  pipeline->BeginFrame({&world}, gameCameras.front());
}

void GameStarter::Draw() {
  auto cmdAlloc = GetCurFrameCommandAllocator();
  ThrowIfFailed(myGCmdList->Reset(cmdAlloc, nullptr));

  ImGui::SetCurrentContext(gameImGuiCtx);

  pipeline->Render(CurrentBackBuffer());

  const auto curBack = CurrentBackBufferView();
  myGCmdList.ResourceBarrierTransition(CurrentBackBuffer(),
                                       D3D12_RESOURCE_STATE_PRESENT,
                                       D3D12_RESOURCE_STATE_RENDER_TARGET);
  myGCmdList->OMSetRenderTargets(1, &curBack, FALSE, NULL);
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
  ImGui_ImplWin32_EndFrame();
}

void GameStarter::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(MainWnd());
}

void GameStarter::OnMouseUp(WPARAM btnState, int x, int y) {
  ReleaseCapture();
}

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
  auto systemIDs = world.systemMngr.systemTraits.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      My::MyGE::WorldTimeSystem>();
  for (auto ID : systemIDs)
    world.systemMngr.Activate(ID);

  world.entityMngr.cmptTraits.Register<
      // core
      My::MyGE::Camera, My::MyGE::MeshFilter, My::MyGE::MeshRenderer,
      My::MyGE::WorldTime, My::MyGE::Name, My::MyGE::Skybox,

      // transform
      My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
      My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
      My::MyGE::Scale, My::MyGE::NonUniformScale, My::MyGE::Translation,
      My::MyGE::WorldToLocal>();

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
      .RegisterComponents<
          // core
          My::MyGE::Camera, My::MyGE::MeshFilter, My::MyGE::MeshRenderer,
          My::MyGE::WorldTime, My::MyGE::Name, My::MyGE::Skybox,

          // transform
          My::MyGE::Children, My::MyGE::LocalToParent, My::MyGE::LocalToWorld,
          My::MyGE::Parent, My::MyGE::Rotation, My::MyGE::RotationEuler,
          My::MyGE::Scale, My::MyGE::NonUniformScale, My::MyGE::Translation,
          My::MyGE::WorldToLocal>();
  //OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());
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
          L"..\\assets\\scripts\\MoveRoot.lua");
  solLua["world"] = &world;
  solLua.script(gameLuaScript->GetText());
}

void GameStarter::LoadTextures() {
  auto tex2dGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
        *My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(path));
  }

  auto texcubeGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.texcube)"});
  for (const auto& guid : texcubeGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
        *My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::TextureCube>(
            path));
  }
}

void GameStarter::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::RsrcMngrDX12::Instance().RegisterShader(*shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}
