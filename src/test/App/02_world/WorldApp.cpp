#include "../common/d3dApp.h"

#include <MyGE/Asset/AssetMngr.h>

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
#include <MyGE/Core/Scene.h>
#include <MyGE/Core/Systems/Systems.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

const int gNumFrameResources = 3;

struct RotateSystem {
  static void OnUpdate(My::MyECS::Schedule& schedule) {
    My::MyECS::ArchetypeFilter filter;
    filter.all = {My::MyECS::CmptAccessType::Of<My::MyGE::MeshFilter>};
    schedule.RegisterEntityJob(
        [](My::MyGE::Rotation* rot, My::MyGE::Translation* trans) {
          rot->value =
              rot->value * My::quatf{My::vecf3{0, 1, 0}, My::to_radian(2.f)};
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

  try {
    WorldApp theApp(hInstance);
    if (!theApp.Initialize())
      return 0;

    int rst = theApp.Run();
    My::MyGE::RsrcMngrDX12::Instance().Clear();
    return rst;
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    My::MyGE::RsrcMngrDX12::Instance().Clear();
    return 0;
  }
}

WorldApp::WorldApp(HINSTANCE hInstance) : D3DApp(hInstance) {}

WorldApp::~WorldApp() {
  if (!myDevice.IsNull())
    FlushCommandQueue();
}

bool WorldApp::Initialize() {
  if (!InitMainWindow())
    return false;

  if (!InitDirect3D())
    return false;

  My::MyGE::RsrcMngrDX12::Instance().Init(myDevice.raw.Get());

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
  auto& upload = My::MyGE::RsrcMngrDX12::Instance().GetUpload();
  auto& deleteBatch = My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch();

  upload.Begin();

  LoadTextures();
  BuildShaders();
  BuildMaterials();

  // update mesh
  world.RunEntityJob(
      [&](const My::MyGE::MeshFilter* meshFilter) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
            upload, deleteBatch, myGCmdList.Get(), meshFilter->mesh);
      },
      false);

  // commit upload, delete ...
  upload.End(myCmdQueue.raw.Get());
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.raw.Get());
  deleteBatch.Commit(myDevice.raw.Get(), myCmdQueue.raw.Get());

  My::MyGE::PipelineBase::InitDesc initDesc;
  initDesc.device = myDevice.raw.Get();
  initDesc.rtFormat = mBackBufferFormat;
  initDesc.cmdQueue = myCmdQueue.raw.Get();
  initDesc.numFrame = gNumFrameResources;
  pipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);

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
  auto& upload = My::MyGE::RsrcMngrDX12::Instance().GetUpload();
  upload.Begin();

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
  auto& deleteBatch = My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch();

  world.RunEntityJob(
      [&](const My::MyGE::MeshFilter* meshFilter,
          const My::MyGE::MeshRenderer* meshRenderer) {
        if (!meshFilter->mesh || meshRenderer->materials.empty())
          return;

        My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
            upload, deleteBatch, myGCmdList.Get(), meshFilter->mesh);

        for (const auto& material : meshRenderer->materials) {
          if (!material)
            continue;
          for (const auto& [name, property] : material->properties) {
            if (std::holds_alternative<const My::MyGE::Texture2D*>(property)) {
              My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
                  My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
                  std::get<const My::MyGE::Texture2D*>(property));
            } else if (std::holds_alternative<const My::MyGE::TextureCube*>(
                           property)) {
              My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
                  My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
                  std::get<const My::MyGE::TextureCube*>(property));
            }
          }
        }
      },
      false);

  if (auto skybox = world.entityMngr.GetSingleton<My::MyGE::Skybox>();
      skybox && skybox->material) {
    for (const auto& [name, property] : skybox->material->properties) {
      if (std::holds_alternative<const My::MyGE::Texture2D*>(property)) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
            My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
            std::get<const My::MyGE::Texture2D*>(property));
      } else if (std::holds_alternative<const My::MyGE::TextureCube*>(
                     property)) {
        My::MyGE::RsrcMngrDX12::Instance().RegisterTextureCube(
            My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
            std::get<const My::MyGE::TextureCube*>(property));
      }
    }
  }

  // commit upload, delete ...
  upload.End(myCmdQueue.raw.Get());
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.raw.Get());
  deleteBatch.Commit(myDevice.raw.Get(), myCmdQueue.raw.Get());
  frameRsrcMngr->EndFrame(myCmdQueue.raw.Get());

  std::vector<My::MyGE::PipelineBase::CameraData> gameCameras;
  My::MyECS::ArchetypeFilter camFilter{
      {My::MyECS::CmptAccessType::Of<My::MyGE::Camera>}};
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

void WorldApp::OnMouseUp(WPARAM btnState, int x, int y) {
  ReleaseCapture();
}

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

void WorldApp::BuildWorld() {
  world.systemMngr.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      RotateSystem>();

  {  // skybox
    auto [e, skybox] = world.entityMngr.Create<My::MyGE::Skybox>();
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(
        xg::Guid{"bba13c3e-87d1-463a-974b-324d997349e3"});
    skybox->material =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(path);
  }

  auto e0 =
      world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::WorldToLocal,
                              My::MyGE::Camera, My::MyGE::Translation,
                              My::MyGE::Rotation>();
  cam = std::get<My::MyECS::Entity>(e0);

  int num = 11;
  auto cubeMesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      L"..\\assets\\models\\cube.obj");
  for (int i = 0; i < num; i++) {
    for (int j = 0; j < num; j++) {
      auto cube =
          world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::MeshFilter,
                                  My::MyGE::MeshRenderer, My::MyGE::Translation,
                                  My::MyGE::Rotation, My::MyGE::Scale>();
      auto t = std::get<My::MyGE::Translation*>(cube);
      auto s = std::get<My::MyGE::Scale*>(cube);
      s->value = 0.2f;
      t->value = {0.5f * (i - num / 2), 0.5f * (j - num / 2), 0};
      std::get<My::MyGE::MeshFilter*>(cube)->mesh = cubeMesh;
    }
  }
}

void WorldApp::LoadTextures() {
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

void WorldApp::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::RsrcMngrDX12::Instance().RegisterShader(shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}

void WorldApp::BuildMaterials() {
  auto material = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(
      L"..\\assets\\materials\\iron.mat");
  world.RunEntityJob([=](My::MyGE::MeshRenderer* meshRenderer) {
    meshRenderer->materials.push_back(material);
  });
}
