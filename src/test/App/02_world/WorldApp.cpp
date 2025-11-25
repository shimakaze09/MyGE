#include <MyGE/Core/AssetMngr.h>
#include <MyGE/Core/Components/Components.h>
#include <MyGE/Core/GameTimer.h>
#include <MyGE/Core/Systems/Systems.h>
#include <MyGE/Render/Components/Components.h>
#include <MyGE/Render/DX12/GPURsrcMngrDX12.h>
#include <MyGE/Render/DX12/StdPipeline.h>
#include <MyGE/Render/HLSLFile.h>
#include <MyGE/Render/Mesh.h>
#include <MyGE/Render/Shader.h>
#include <MyGE/Render/ShaderMngr.h>
#include <MyGE/Render/Systems/Systems.h>
#include <MyGE/Render/Texture2D.h>
#include <MyGE/Render/TextureCube.h>

#include "../common/d3dApp.h"

#ifndef NDEBUG
#include <dxgidebug.h>
#endif

using Microsoft::WRL::ComPtr;
using namespace DirectX;

const int gNumFrameResources = 3;

struct RotateSystem {
  static void OnUpdate(My::MyECS::Schedule& schedule) {
    My::MyECS::ArchetypeFilter filter;
    filter.all = {My::MyECS::AccessTypeID_of<My::MyGE::MeshFilter>};
    schedule.RegisterEntityJob(
        [](My::MyGE::Rotation* rot, My::MyGE::Translation* trans) {
          rot->value = rot->value *
                       My::quatf{My::vecf3{0, 1, 0}, My::to_radian(2.f)};
          trans->value +=
              0.2f * (My::vecf3{My::rand01<float>(), My::rand01<float>(),
                                  My::rand01<float>()} -
                      My::vecf3{0.5f});
        },
        "rotate", true, filter);
  }
};

class WorldApp : public D3DApp {
 public:
  WorldApp(HINSTANCE hInstance);
  WorldApp(const WorldApp& rhs) = delete;
  WorldApp& operator=(const WorldApp& rhs) = delete;
  ~WorldApp();

  virtual bool Initialize() override;

 private:
  virtual void OnResize() override;
  virtual void Update() override;
  virtual void Draw() override;

  virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
  virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
  virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

  void UpdateCamera();

  void BuildWorld();
  void LoadTextures();
  void BuildShaders();
  void BuildMaterials();

 private:
  float mTheta = 0.4f * XM_PI;
  float mPhi = 1.3f * XM_PI;
  float mRadius = 5.0f;

  POINT mLastMousePos;

  My::MyGE::Texture2D* albedoTex2D;
  My::MyGE::Texture2D* roughnessTex2D;
  My::MyGE::Texture2D* metalnessTex2D;

  My::MyECS::World world;
  My::MyECS::Entity cam{My::MyECS::Entity::Invalid()};

  std::unique_ptr<My::MyGE::PipelineBase> pipeline;

  std::unique_ptr<My::MyDX12::FrameResourceMngr> frameRsrcMngr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
  int rst;

  try {
    WorldApp theApp(hInstance);
    if (!theApp.Initialize()) return 0;

    rst = theApp.Run();
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    rst = 1;
  }

#ifndef NDEBUG
  Microsoft::WRL::ComPtr<IDXGIDebug> debug;
  DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug));
  if (debug) debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
#endif

  return 1;
}

WorldApp::WorldApp(HINSTANCE hInstance) : D3DApp(hInstance) {}

WorldApp::~WorldApp() {
  My::MyGE::GPURsrcMngrDX12::Instance().Clear(myCmdQueue.Get());
  if (!myDevice.IsNull()) FlushCommandQueue();
}

bool WorldApp::Initialize() {
  if (!InitMainWindow()) return false;

  if (!InitDirect3D()) return false;

  My::MyGE::GPURsrcMngrDX12::Instance().Init(myDevice.raw.Get());

  My::MyDX12::DescriptorHeapMngr::Instance().Init(myDevice.raw.Get(), 1024,
                                                    1024, 1024, 1024, 1024);

  frameRsrcMngr = std::make_unique<My::MyDX12::FrameResourceMngr>(
      gNumFrameResources, myDevice.raw.Get());
  for (const auto& fr : frameRsrcMngr->GetFrameResources()) {
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    ThrowIfFailed(myDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));
    fr->RegisterResource("CommandAllocator", allocator);
  }

  My::MyGE::AssetMngr::Instance().ImportAssetRecursively(L"..\\assets");

  BuildWorld();

  ThrowIfFailed(myGCmdList->Reset(mDirectCmdListAlloc.Get(), nullptr));

  LoadTextures();
  BuildShaders();
  BuildMaterials();

  // update mesh
  world.RunEntityJob(
      [&](My::MyGE::MeshFilter* meshFilter) {
        My::MyGE::GPURsrcMngrDX12::Instance().RegisterMesh(myGCmdList.Get(),
                                                             *meshFilter->mesh);
      },
      false);

  My::MyGE::PipelineBase::InitDesc initDesc;
  initDesc.device = myDevice.raw.Get();
  initDesc.rtFormat = mBackBufferFormat;
  initDesc.cmdQueue = myCmdQueue.raw.Get();
  initDesc.numFrame = gNumFrameResources;
  pipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);

  // commit upload, delete ...
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.raw.Get());

  My::MyGE::GPURsrcMngrDX12::Instance().CommitUploadAndDelete(
      myCmdQueue.raw.Get());

  // Do the initial resize code.
  OnResize();

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void WorldApp::OnResize() {
  D3DApp::OnResize();

  if (pipeline)
    pipeline->Resize(mClientWidth, mClientHeight, mScreenViewport,
                     mScissorRect);
}

void WorldApp::Update() {
  UpdateCamera();

  world.Update();

  // update mesh, texture ...
  frameRsrcMngr->BeginFrame();

  auto cmdAlloc =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(
              "CommandAllocator");
  cmdAlloc->Reset();
  ThrowIfFailed(myGCmdList->Reset(cmdAlloc.Get(), nullptr));

  world.RunEntityJob(
      [&](My::MyGE::MeshFilter* meshFilter,
          const My::MyGE::MeshRenderer* meshRenderer) {
        if (!meshFilter->mesh || meshRenderer->materials.empty()) return;

        My::MyGE::GPURsrcMngrDX12::Instance().RegisterMesh(myGCmdList.Get(),
                                                             *meshFilter->mesh);

        for (const auto& material : meshRenderer->materials) {
          if (!material) continue;
          for (const auto& [name, property] : material->properties) {
            if (std::holds_alternative<
                    My::MyGE::SharedVar<My::MyGE::Texture2D>>(property.value)) {
              My::MyGE::GPURsrcMngrDX12::Instance().RegisterTexture2D(
                  const_cast<My::MyGE::Texture2D&>(*std::get<My::MyGE::SharedVar<My::MyGE::Texture2D>>(
                      property.value)));
            } else if (std::holds_alternative<
                           My::MyGE::SharedVar<My::MyGE::TextureCube>>(
                           property.value)) {
              My::MyGE::GPURsrcMngrDX12::Instance().RegisterTextureCube(
                  const_cast<My::MyGE::TextureCube&>(*std::get<My::MyGE::SharedVar<My::MyGE::TextureCube>>(
                      property.value)));
            }
          }
        }
      },
      false);

  if (auto skybox = world.entityMngr.ReadSingleton<My::MyGE::Skybox>();
      skybox && skybox->material) {
    for (const auto& [name, property] : skybox->material->properties) {
      if (std::holds_alternative<My::MyGE::SharedVar<My::MyGE::Texture2D>>(
              property.value)) {
        My::MyGE::GPURsrcMngrDX12::Instance().RegisterTexture2D(
            const_cast<My::MyGE::Texture2D&>(*std::get<My::MyGE::SharedVar<My::MyGE::Texture2D>>(property.value)));
      } else if (std::holds_alternative<
                     My::MyGE::SharedVar<My::MyGE::TextureCube>>(
                     property.value)) {
        My::MyGE::GPURsrcMngrDX12::Instance().RegisterTextureCube(
            const_cast<My::MyGE::TextureCube&>(*std::get<My::MyGE::SharedVar<My::MyGE::TextureCube>>(
                property.value)));
      }
    }
  }

  // commit upload, delete ...
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.raw.Get());
  My::MyGE::GPURsrcMngrDX12::Instance().CommitUploadAndDelete(
      myCmdQueue.raw.Get());
  frameRsrcMngr->EndFrame(myCmdQueue.raw.Get());

  std::vector<My::MyGE::PipelineBase::CameraData> gameCameras;
  My::MyECS::ArchetypeFilter camFilter{
      {My::MyECS::AccessTypeID_of<My::MyGE::Camera>}};
  world.RunEntityJob(
      [&](My::MyECS::Entity e) { gameCameras.emplace_back(e, world); }, false,
      camFilter);
  assert(gameCameras.size() == 1);  // now only support 1 camera
  pipeline->BeginFrame({&world}, gameCameras.front());
}

void WorldApp::Draw() {
  pipeline->Render(CurrentBackBuffer());
  // Swap the back and front buffers
  ThrowIfFailed(mSwapChain->Present(0, 0));
  mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
  pipeline->EndFrame();
}

void WorldApp::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(mhMainWnd);
}

void WorldApp::OnMouseUp(WPARAM btnState, int x, int y) { ReleaseCapture(); }

void WorldApp::OnMouseMove(WPARAM btnState, int x, int y) {
  if ((btnState & MK_LBUTTON) != 0) {
    // Make each pixel correspond to a quarter of a degree.
    float dx =
        XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
    float dy =
        XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

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

void WorldApp::UpdateCamera() {
  My::vecf3 eye = {mRadius * sinf(mTheta) * sinf(mPhi),
                     mRadius * cosf(mTheta),
                     mRadius * sinf(mTheta) * cosf(mPhi)};
  auto camera = world.entityMngr.WriteComponent<My::MyGE::Camera>(cam);
  camera->fov = 60.f;
  camera->aspect = AspectRatio();
  camera->clippingPlaneMin = 1.0f;
  camera->clippingPlaneMax = 1000.0f;
  auto view = My::transformf::look_at(eye.as<My::pointf3>(),
                                        {0.f});  // world to camera
  auto c2w = view.inverse();
  world.entityMngr.WriteComponent<My::MyGE::Translation>(cam)->value = eye;
  world.entityMngr.WriteComponent<My::MyGE::Rotation>(cam)->value =
      c2w.decompose_quatenion();
}

void WorldApp::BuildWorld() {
  auto systemIDs = world.systemMngr.systemTraits.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      RotateSystem>();
  for (auto ID : systemIDs) world.systemMngr.Activate(ID);

  {  // skybox
    std::vector<My::TypeID> types = {My::TypeID_of<My::MyGE::Skybox>};
    auto e = world.entityMngr.Create(types);
    auto skybox = world.entityMngr.WriteComponent<My::MyGE::Skybox>(e);
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(
        xg::Guid{"bba13c3e-87d1-463a-974b-324d997349e3"});
    skybox->material =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(path);
  }

  std::vector<My::TypeID> camTypes = {
      My::TypeID_of<My::MyGE::LocalToWorld>,
      My::TypeID_of<My::MyGE::WorldToLocal>,
      My::TypeID_of<My::MyGE::Camera>,
      My::TypeID_of<My::MyGE::Translation>,
      My::TypeID_of<My::MyGE::Rotation>};
  cam = world.entityMngr.Create(camTypes);

  int num = 11;
  auto cubeMesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      L"..\\assets\\models\\cube.obj");
  std::vector<My::TypeID> cubeTypes = {
      My::TypeID_of<My::MyGE::LocalToWorld>,
      My::TypeID_of<My::MyGE::MeshFilter>,
      My::TypeID_of<My::MyGE::MeshRenderer>,
      My::TypeID_of<My::MyGE::Translation>,
      My::TypeID_of<My::MyGE::Rotation>,
      My::TypeID_of<My::MyGE::Scale>};
  for (int i = 0; i < num; i++) {
    for (int j = 0; j < num; j++) {
      auto cube = world.entityMngr.Create(cubeTypes);
      auto t = world.entityMngr.WriteComponent<My::MyGE::Translation>(cube);
      auto s = world.entityMngr.WriteComponent<My::MyGE::Scale>(cube);
      s->value = 0.2f;
      t->value = {0.5f * (i - num / 2), 0.5f * (j - num / 2), 0};
      world.entityMngr.WriteComponent<My::MyGE::MeshFilter>(cube)->mesh = cubeMesh;
    }
  }
}

void WorldApp::LoadTextures() {
  auto tex2dGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::GPURsrcMngrDX12::Instance().RegisterTexture2D(
        *My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            path));
  }

  auto texcubeGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.texcube)"});
  for (const auto& guid : texcubeGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::GPURsrcMngrDX12::Instance().RegisterTextureCube(
        *My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::TextureCube>(
            path));
  }
}

void WorldApp::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::GPURsrcMngrDX12::Instance().RegisterShader(*shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}

void WorldApp::BuildMaterials() {
  auto material =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(
          L"..\\assets\\materials\\iron.mat");
  world.RunEntityJob([=](My::MyGE::MeshRenderer* meshRenderer) {
    meshRenderer->materials.push_back(material);
  });
}

