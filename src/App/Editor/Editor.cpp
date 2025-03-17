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
  void BuildMaterials();

 private:
  float mTheta = 0.4f * My::PI<float>;
  float mPhi = 1.3f * My::PI<float>;
  float mRadius = 5.0f;

  POINT mLastMousePos;

  My::UECS::World world;
  My::UECS::Entity cam{My::UECS::Entity::Invalid()};

  std::unique_ptr<My::MyGE::IPipeline> pipeline;

  void OnGameResize();
  size_t gameWidth, gameHeight;
  ComPtr<ID3D12Resource> gameRT;
  const DXGI_FORMAT gameRTFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
  const DXGI_FORMAT gameDSFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
  ComPtr<ID3D12Resource> gameDS;
  My::UDX12::DescriptorHeapAllocation gameRT_SRV;
  My::UDX12::DescriptorHeapAllocation gameDS_DSV;

  bool show_demo_window = true;
  bool show_another_window = false;
};

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

LRESULT Editor::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
      if (!uDevice.IsNull()) {
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
    Editor theApp(hInstance);
    if (!theApp.Initialize()) return 1;

    int rst = theApp.Run();
    return rst;
  } catch (My::UDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    return 1;
  }
}

Editor::Editor(HINSTANCE hInstance) : DX12App(hInstance) {}

Editor::~Editor() {
  if (!uDevice.IsNull()) FlushCommandQueue();

  My::MyGE::ImGUIMngr::Instance().Clear();
  if (!gameRT_SRV.IsNull())
    My::UDX12::DescriptorHeapMngr::Instance().GetCSUGpuDH()->Free(
        std::move(gameRT_SRV));
  if (!gameDS_DSV.IsNull())
    My::UDX12::DescriptorHeapMngr::Instance().GetDSVCpuDH()->Free(
        std::move(gameDS_DSV));
}

bool Editor::Initialize() {
  if (!InitMainWindow()) return false;

  if (!InitDirect3D()) return false;

  gameRT_SRV =
      My::UDX12::DescriptorHeapMngr::Instance().GetCSUGpuDH()->Allocate(1);
  gameDS_DSV =
      My::UDX12::DescriptorHeapMngr::Instance().GetDSVCpuDH()->Allocate(1);

  My::MyGE::MeshLayoutMngr::Instance().Init();

  My::MyGE::ImGUIMngr::Instance().Init(MainWnd(), uDevice.Get(),
                                       NumFrameResources);

  My::MyGE::AssetMngr::Instance().ImportAssetRecursively(LR"(..\\assets)");

  BuildWorld();

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();
  LoadTextures();
  BuildShaders();
  BuildMaterials();
  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(uCmdQueue.Get());

  // OutputDebugStringA(My::MyGE::Serializer::Instance().ToJSON(&world).c_str());

  My::MyGE::IPipeline::InitDesc initDesc;
  initDesc.device = uDevice.Get();
  initDesc.rtFormat = gameRTFormat;
  initDesc.depthStencilFormat = gameDSFormat;
  initDesc.cmdQueue = uCmdQueue.Get();
  initDesc.numFrame = NumFrameResources;
  pipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);

  // Do the initial resize code.
  OnResize();

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void Editor::OnResize() { DX12App::OnResize(); }

void Editor::OnGameResize() {
  // Flush before changing any resources.
  FlushCommandQueue();

  My::rgbaf background = {0.f, 0.f, 0.f, 1.f};
  auto rtType = My::UDX12::FG::RsrcType::RT2D(gameRTFormat, gameWidth,
                                              gameHeight, background.data());
  ThrowIfFailed(uDevice->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
      &rtType.desc, D3D12_RESOURCE_STATE_PRESENT, &rtType.clearValue,
      IID_PPV_ARGS(gameRT.ReleaseAndGetAddressOf())));
  // auto rtvDesc = My::UDX12::Desc::SRV::Tex2D(gameRTFormat);
  uDevice->CreateShaderResourceView(gameRT.Get(), nullptr,
                                    gameRT_SRV.GetCpuHandle());

  // Create the depth/stencil buffer and view.
  D3D12_RESOURCE_DESC depthStencilDesc;
  depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
  depthStencilDesc.Alignment = 0;
  depthStencilDesc.Width = mClientWidth;
  depthStencilDesc.Height = mClientHeight;
  depthStencilDesc.DepthOrArraySize = 1;
  depthStencilDesc.MipLevels = 1;

  // Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to
  // read from the depth buffer.  Therefore, because we need to create two views
  // to the same resource:
  //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
  //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
  // we need to create the depth buffer resource with a typeless format.
  depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
  depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

  D3D12_CLEAR_VALUE optClear;
  optClear.Format = gameDSFormat;
  optClear.DepthStencil.Depth = 1.0f;
  optClear.DepthStencil.Stencil = 0;
  ThrowIfFailed(uDevice->CreateCommittedResource(
      &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE,
      &depthStencilDesc, D3D12_RESOURCE_STATE_PRESENT, &optClear,
      IID_PPV_ARGS(gameDS.ReleaseAndGetAddressOf())));
  auto dsvDesc = My::UDX12::Desc::DSV::Basic(gameDSFormat);
  uDevice->CreateDepthStencilView(gameDS.Get(), &dsvDesc,
                                  gameDS_DSV.GetCpuHandle());

  assert(pipeline);
  D3D12_VIEWPORT viewport;
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = static_cast<float>(gameWidth);
  viewport.Height = static_cast<float>(gameHeight);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  pipeline->Resize(gameWidth, gameHeight, viewport,
                   {0, 0, (LONG)gameWidth, (LONG)gameHeight}, gameDS.Get());

  // Flush before changing any resources.
  FlushCommandQueue();
}

void Editor::Update() {
  UpdateCamera();

  world.Update();

  // update mesh, texture ...
  GetFrameResourceMngr()->BeginFrame();

  auto cmdAlloc = GetCurFrameCommandAllocator();
  cmdAlloc->Reset();

  ThrowIfFailed(uGCmdList->Reset(cmdAlloc, nullptr));
  auto& upload = My::MyGE::RsrcMngrDX12::Instance().GetUpload();
  auto& deleteBatch = My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch();
  upload.Begin();

  // update mesh
  world.RunEntityJob(
      [&](const My::MyGE::MeshFilter* meshFilter) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
            upload, deleteBatch, uGCmdList.Get(), meshFilter->mesh);
      },
      false);

  // commit upload, delete ...
  upload.End(uCmdQueue.Get());
  deleteBatch.Commit(uDevice.Get(), uCmdQueue.Get());
  uGCmdList->Close();
  uCmdQueue.Execute(uGCmdList.Get());
  GetFrameResourceMngr()->EndFrame(uCmdQueue.Get());

  pipeline->UpdateRenderContext(world);
}

void Editor::Draw() {
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

  // 4. game window
  if (ImGui::Begin("output", nullptr, ImGuiWindowFlags_NoScrollbar)) {
    auto tex = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
        L"..\\assets\\textures\\chessboard.tex2d");
    auto gameSize = ImGui::GetContentRegionAvail();
    auto w = (size_t)gameSize.x;
    auto h = (size_t)gameSize.y;
    if (w != gameWidth || h != gameHeight) {
      gameWidth = w;
      gameHeight = h;
      OnGameResize();
    }

    ImGui::Image(
        ImTextureID(gameRT_SRV.GetGpuHandle().ptr),
        //{ float(tex->image->width.get()), float(tex->image->height.get()) }
        ImGui::GetContentRegionAvail());
  }
  ImGui::End();

  pipeline->Render(gameRT.Get());

  auto cmdAlloc = GetCurFrameCommandAllocator();
  ThrowIfFailed(uGCmdList->Reset(cmdAlloc, nullptr));

  D3D12_RESOURCE_BARRIER barrier = {};
  barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
  barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
  barrier.Transition.pResource = CurrentBackBuffer();
  barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

  uGCmdList->ResourceBarrier(1, &barrier);
  uGCmdList->ClearRenderTargetView(CurrentBackBufferView(),
                                   DirectX::Colors::LightSteelBlue, 0, NULL);
  uGCmdList->OMSetRenderTargets(1, &CurrentBackBufferView(), FALSE, NULL);
  auto heap = My::UDX12::DescriptorHeapMngr::Instance()
                  .GetCSUGpuDH()
                  ->GetDescriptorHeap();
  uGCmdList->SetDescriptorHeaps(1, &heap);
  ImGui::Render();
  ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), uGCmdList.Get());
  barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
  barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
  uGCmdList->ResourceBarrier(1, &barrier);
  uGCmdList->Close();
  uCmdQueue.Execute(uGCmdList.Get());

  SwapBackBuffer();

  pipeline->EndFrame();
}

void Editor::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(MainWnd());
}

void Editor::OnMouseUp(WPARAM btnState, int x, int y) { ReleaseCapture(); }

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
  auto camera = world.entityMngr.Get<My::MyGE::Camera>(cam);
  camera->aspect = gameWidth / (float)gameHeight;
  auto view =
      My::transformf::look_at(eye.as<My::pointf3>(), {0.f});  // world to camera
  auto c2w = view.inverse();
  world.entityMngr.Get<My::MyGE::Translation>(cam)->value = eye;
  world.entityMngr.Get<My::MyGE::Rotation>(cam)->value =
      c2w.decompose_quatenion();
}

void Editor::BuildWorld() {
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
  cam = std::get<My::UECS::Entity>(e0);

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
            .GetEntityArray({{My::UECS::CmptAccessType::Of<My::MyGE::Camera>}})
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

void Editor::LoadTextures() {
  auto tex2dGUIDs =
      My::MyGE::AssetMngr::Instance().FindAssets(std::wregex{LR"(.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
        My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(path));
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

void Editor::BuildMaterials() {
  /*auto material = My::MyGE::AssetMngr::Instance()
          .LoadAsset<My::MyGE::Material>(L"..\\assets\\materials\\iron.mat");
  world.RunEntityJob([=](My::MyGE::MeshRenderer* meshRenderer) {
          meshRenderer->materials.push_back(material);
  });*/
}
