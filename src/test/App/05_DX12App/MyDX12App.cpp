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

using Microsoft::WRL::ComPtr;

struct AnimateMeshSystem {
  size_t cnt = 0;

  static void OnUpdate(My::MyECS::Schedule& schedule) {
    schedule.RegisterEntityJob(
        [](My::MyGE::MeshFilter* meshFilter,
           My::MyECS::Latest<My::MyECS::Singleton<My::MyGE::WorldTime>> time) {
          if (time->elapsedTime < 10.f) {
            if (meshFilter->mesh->IsEditable()) {
              auto positions = meshFilter->mesh->GetPositions();
              for (auto& pos : positions)
                pos[1] = 0.2f * (My::rand01<float>() - 0.5f);
              meshFilter->mesh->SetPositions(positions);
            }
          } else
            meshFilter->mesh->SetToNonEditable();
        },
        "AnimateMesh");
    schedule.RegisterCommand([](My::MyECS::World* w) {
      auto time = w->entityMngr.GetSingleton<My::MyGE::WorldTime>();
      if (!time)
        return;

      if (time->elapsedTime < 10.f)
        return;

      w->systemMngr.Deactivate(w->systemMngr.GetIndex<AnimateMeshSystem>());
    });
  }
};

class MyDX12App : public My::MyGE::DX12App {
 public:
  MyDX12App(HINSTANCE hInstance);
  ~MyDX12App();

  virtual bool Init() override;

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

  std::unique_ptr<My::MyGE::PipelineBase> pipeline;
  std::shared_ptr<My::MyGE::Mesh> dynamicMesh;

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

LRESULT MyDX12App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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

  try {
    MyDX12App theApp(hInstance);
    if (!theApp.Init())
      return 1;

    int rst = theApp.Run();
    return rst;
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    return 1;
  }
}

MyDX12App::MyDX12App(HINSTANCE hInstance) : DX12App(hInstance) {}

MyDX12App::~MyDX12App() {
  if (!myDevice.IsNull())
    FlushCommandQueue();

  My::MyGE::ImGUIMngr::Instance().Clear();
}

bool MyDX12App::Init() {
  if (!InitMainWindow())
    return false;

  if (!InitDirect3D())
    return false;

  My::MyGE::ImGUIMngr::Instance().Init(MainWnd(), myDevice.Get(),
                                       NumFrameResources, 1);
  gameImGuiCtx = My::MyGE::ImGUIMngr::Instance().GetContexts().at(0);

  My::MyGE::AssetMngr::Instance().ImportAssetRecursively(L"..\\assets");

  BuildWorld();

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();
  LoadTextures();
  BuildShaders();
  BuildMaterials();

  My::MyGE::PipelineBase::InitDesc initDesc;
  initDesc.device = myDevice.Get();
  initDesc.rtFormat = GetBackBufferFormat();
  initDesc.cmdQueue = myCmdQueue.Get();
  initDesc.numFrame = NumFrameResources;
  pipeline = std::make_unique<My::MyGE::StdPipeline>(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), initDesc);
  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(myCmdQueue.Get());

  // Do the initial resize code.
  OnResize();

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void MyDX12App::OnResize() {
  DX12App::OnResize();

  assert(pipeline);
  pipeline->Resize(mClientWidth, mClientHeight, GetScreenViewport(),
                   GetScissorRect());
}

void MyDX12App::Update() {
  // Start the Dear ImGui frame
  ImGui_ImplDX12_NewFrame();
  ImGui_ImplWin32_NewFrame_Context(gameImGuiCtx, {0, 0}, (float)mClientWidth,
                                   (float)mClientHeight);
  ImGui_ImplWin32_NewFrame_Shared();

  auto& upload = My::MyGE::RsrcMngrDX12::Instance().GetUpload();
  upload.Begin();

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

  // update mesh

  world.RunEntityJob(
      [&](const My::MyGE::MeshFilter* meshFilter,
          const My::MyGE::MeshRenderer* meshRenderer) {
        if (!meshFilter->mesh || meshRenderer->materials.empty())
          return;

        My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
            upload, deleteBatch, myGCmdList.Get(), *meshFilter->mesh);

        for (const auto& material : meshRenderer->materials) {
          if (!material)
            continue;
          for (const auto& [name, property] : material->properties) {
            if (std::holds_alternative<
                    std::shared_ptr<const My::MyGE::Texture2D>>(property)) {
              My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
                  My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
                  *std::get<std::shared_ptr<const My::MyGE::Texture2D>>(
                      property));
            } else if (std::holds_alternative<
                           std::shared_ptr<const My::MyGE::TextureCube>>(
                           property)) {
              My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
                  My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
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
            My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
            *std::get<std::shared_ptr<const My::MyGE::Texture2D>>(property));
      } else if (std::holds_alternative<
                     std::shared_ptr<const My::MyGE::TextureCube>>(property)) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
            My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
            *std::get<std::shared_ptr<const My::MyGE::TextureCube>>(property));
      }
    }
  }

  // commit upload, delete ...
  upload.End(myCmdQueue.Get());
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.Get());
  deleteBatch.Commit(myDevice.Get(), myCmdQueue.Get());

  std::vector<My::MyGE::PipelineBase::CameraData> gameCameras;
  My::MyECS::ArchetypeFilter camFilter{
      {My::MyECS::CmptAccessType::Of<My::MyGE::Camera>}};
  world.RunEntityJob(
      [&](My::MyECS::Entity e) { gameCameras.emplace_back(e, world); }, false,
      camFilter);
  assert(gameCameras.size() == 1);  // now only support 1 camera
  pipeline->BeginFrame({&world}, gameCameras.front());
}

void MyDX12App::Draw() {
  auto cmdAlloc = GetCurFrameCommandAllocator();
  ThrowIfFailed(myGCmdList->Reset(cmdAlloc, nullptr));

  ImGui::SetCurrentContext(gameImGuiCtx);

  pipeline->Render(CurrentBackBuffer());

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
  ImGui_ImplWin32_EndFrame();
}

void MyDX12App::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(MainWnd());
}

void MyDX12App::OnMouseUp(WPARAM btnState, int x, int y) {
  ReleaseCapture();
}

void MyDX12App::OnMouseMove(WPARAM btnState, int x, int y) {
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

void MyDX12App::UpdateCamera() {
  My::vecf3 eye = {mRadius * sinf(mTheta) * sinf(mPhi), mRadius * cosf(mTheta),
                   mRadius * sinf(mTheta) * cosf(mPhi)};
  auto camera = world.entityMngr.Get<My::MyGE::Camera>(cam);
  camera->fov = 60.f;
  camera->aspect = AspectRatio();
  camera->clippingPlaneMin = 1.0f;
  camera->clippingPlaneMax = 1000.0f;
  auto view =
      My::transformf::look_at(eye.as<My::pointf3>(), {0.f});  // world to camera
  auto c2w = view.inverse();
  world.entityMngr.Get<My::MyGE::Translation>(cam)->value = eye;
  world.entityMngr.Get<My::MyGE::Rotation>(cam)->value =
      c2w.decompose_quatenion();
}

void MyDX12App::BuildWorld() {
  auto indices = world.systemMngr.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      My::MyGE::WorldTimeSystem, AnimateMeshSystem>();
  for (auto idx : indices)
    world.systemMngr.Activate(idx);

  {  // skybox
    auto [e, skybox] = world.entityMngr.Create<My::MyGE::Skybox>();
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(
        xg::Guid{"bba13c3e-87d1-463a-974b-324d997349e3"});
    skybox->material =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(path);
  }

  {
    auto e =
        world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::WorldToLocal,
                                My::MyGE::Camera, My::MyGE::Translation,
                                My::MyGE::Rotation>();
    cam = std::get<My::MyECS::Entity>(e);
  }

  { world.entityMngr.Create<My::MyGE::WorldTime>(); }

  auto quadMesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      "../assets/models/quad.obj");
  auto dynamicCube =
      world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::MeshFilter,
                              My::MyGE::MeshRenderer, My::MyGE::Translation,
                              My::MyGE::Rotation, My::MyGE::Scale>();
  dynamicMesh = std::make_shared<My::MyGE::Mesh>();
  dynamicMesh->SetPositions(quadMesh->GetPositions());
  dynamicMesh->SetNormals(quadMesh->GetNormals());
  dynamicMesh->SetUV(quadMesh->GetUV());
  dynamicMesh->SetIndices(quadMesh->GetIndices());
  dynamicMesh->SetSubMeshCount(quadMesh->GetSubMeshes().size());
  for (size_t i = 0; i < quadMesh->GetSubMeshes().size(); i++)
    dynamicMesh->SetSubMesh(i, quadMesh->GetSubMeshes().at(i));
  std::get<My::MyGE::MeshFilter*>(dynamicCube)->mesh = dynamicMesh;
}

void MyDX12App::LoadTextures() {
  auto tex2dGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
        My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
        *My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(path));
  }

  auto texcubeGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.texcube)"});
  for (const auto& guid : texcubeGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
        My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
        *My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::TextureCube>(
            path));
  }
}

void MyDX12App::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::RsrcMngrDX12::Instance().RegisterShader(*shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}

void MyDX12App::BuildMaterials() {
  auto material = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(
      L"..\\assets\\materials\\iron.mat");
  world.RunEntityJob([=](My::MyGE::MeshRenderer* meshRenderer) {
    meshRenderer->materials.push_back(material);
  });
}
