//
// Created by Admin on 16/03/2025.
//

#include <MyDX12/UploadBuffer.h>
#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Core/Components/Camera.h>
#include <MyGE/Core/Components/MeshFilter.h>
#include <MyGE/Core/Components/MeshRenderer.h>
#include <MyGE/Core/HLSLFile.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Mesh.h>
#include <MyGE/Core/Shader.h>
#include <MyGE/Core/Systems/CameraSystem.h>
#include <MyGE/Core/Texture2D.h>
#include <MyGE/Render/DX12/MeshLayoutMngr.h>
#include <MyGE/Render/DX12/ShaderCBMngrDX12.h>
#include <MyGE/Render/DX12/StdPipeline.h>
#include <MyGE/Transform/Transform.h>
#include <MyGM/MyGM.h>

#include "../common/d3dApp.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

struct RotateSystem : My::MyECS::System {
  using My::MyECS::System::System;

  virtual void OnUpdate(My::MyECS::Schedule& schedule) override {
    My::MyECS::ArchetypeFilter filter;
    filter.all = {My::MyECS::CmptAccessType::Of<My::MyGE::MeshFilter>};
    schedule.RegisterEntityJob(
        [](My::MyGE::Rotation* rot) {
          rot->value =
              rot->value * My::quatf{My::vecf3{0, 1, 0}, My::to_radian(2.f)};
        },
        "rotate", filter);
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

  void OnKeyboardInput();
  void UpdateCamera();

  void BuildWorld();
  void LoadTextures();
  void BuildShapeGeometry();
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

  std::unique_ptr<My::MyGE::IPipeline> pipeline;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  try {
    WorldApp theApp(hInstance);
    if (!theApp.Initialize()) return 0;

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
  if (!myDevice.IsNull()) FlushCommandQueue();
}

bool WorldApp::Initialize() {
  if (!InitMainWindow()) return false;

  if (!InitDirect3D()) return false;

  My::MyGE::RsrcMngrDX12::Instance().Init(myDevice.raw.Get());

  My::MyDX12::DescriptorHeapMngr::Instance().Init(myDevice.raw.Get(), 1024,
                                                  1024, 1024, 1024, 1024);

  My::MyGE::IPipeline::InitDesc initDesc;
  initDesc.device = myDevice.raw.Get();
  initDesc.backBufferFormat = mBackBufferFormat;
  initDesc.depthStencilFormat = mDepthStencilFormat;
  initDesc.cmdQueue = myCmdQueue.raw.Get();
  initDesc.numFrame = gNumFrameResources;
  pipeline = std::make_unique<My::MyGE::StdPipeline>(initDesc);

  My::MyGE::MeshLayoutMngr::Instance().Init();

  // Do the initial resize code.
  OnResize();

  BuildWorld();

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();
  LoadTextures();
  BuildShapeGeometry();
  BuildMaterials();
  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(myCmdQueue.raw.Get());

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void WorldApp::OnResize() {
  D3DApp::OnResize();

  if (pipeline)
    pipeline->Resize(mClientWidth, mClientHeight, mScreenViewport, mScissorRect,
                     mDepthStencilBuffer.Get());
}

void WorldApp::Update() {
  OnKeyboardInput();
  UpdateCamera();

  world.Update();
  pipeline->UpdateRenderContext(world);
}

void WorldApp::Draw() {
  pipeline->BeginFrame(CurrentBackBuffer());
  pipeline->Render();
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

void WorldApp::OnKeyboardInput() {}

void WorldApp::UpdateCamera() {
  My::vecf3 eye = {mRadius * sinf(mTheta) * sinf(mPhi), mRadius * cosf(mTheta),
                   mRadius * sinf(mTheta) * cosf(mPhi)};
  auto camera = world.entityMngr.Get<My::MyGE::Camera>(cam);
  camera->fov = 0.33f * My::PI<float>;
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

  auto e0 =
      world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::WorldToLocal,
                              My::MyGE::Camera, My::MyGE::Translation,
                              My::MyGE::Rotation>();
  cam = std::get<My::MyECS::Entity>(e0);

  int num = 11;
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
    }
  }
}

void WorldApp::LoadTextures() {
  auto albedoImg = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Image>(
      "../assets/textures/iron/albedo.png");
  auto roughnessImg =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Image>(
          "../assets/textures/iron/roughness.png");
  auto metalnessImg =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Image>(
          "../assets/textures/iron/metalness.png");

  albedoTex2D = new My::MyGE::Texture2D;
  albedoTex2D->image = albedoImg;
  if (!My::MyGE::AssetMngr::Instance().CreateAsset(
          albedoTex2D, "../assets/textures/iron/albedo.tex2d")) {
    delete albedoTex2D;
    albedoTex2D =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            "../assets/textures/iron/albedo.tex2d");
  }

  roughnessTex2D = new My::MyGE::Texture2D;
  roughnessTex2D->image = roughnessImg;
  if (!My::MyGE::AssetMngr::Instance().CreateAsset(
          roughnessTex2D, "../assets/textures/iron/roughness.tex2d")) {
    delete roughnessTex2D;
    roughnessTex2D =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            "../assets/textures/iron/roughness.tex2d");
  }

  metalnessTex2D = new My::MyGE::Texture2D;
  metalnessTex2D->image = metalnessImg;
  if (!My::MyGE::AssetMngr::Instance().CreateAsset(
          metalnessTex2D, "../assets/textures/iron/metalness.tex2d")) {
    delete metalnessTex2D;
    metalnessTex2D =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            "../assets/textures/iron/metalness.tex2d");
  }

  My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), albedoTex2D);
  My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), roughnessTex2D);
  My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(), metalnessTex2D);
}

void WorldApp::BuildShapeGeometry() {
  auto mesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      "../assets/models/cube.obj");
  My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
      My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch(), myGCmdList.raw.Get(),
      mesh);
  My::MyECS::ArchetypeFilter filter;
  filter.all = {My::MyECS::CmptAccessType::Of<My::MyGE::MeshFilter>};
  auto meshFilters =
      world.entityMngr.GetCmptArray<My::MyGE::MeshFilter>(filter);
  for (auto meshFilter : meshFilters) meshFilter->mesh = mesh;
}

void WorldApp::BuildMaterials() {
  std::filesystem::path matPath = "../assets/materials/iron.mat";
  auto material = new My::MyGE::Material;
  material->shader =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Shader>(
          "../assets/shaders/geometry.shader");
  material->texture2Ds.emplace("gAlbedoMap", albedoTex2D);
  material->texture2Ds.emplace("gRoughnessMap", roughnessTex2D);
  material->texture2Ds.emplace("gMetalnessMap", metalnessTex2D);

  if (!My::MyGE::AssetMngr::Instance().CreateAsset(material, matPath)) {
    delete material;
    material =
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(matPath);
  }
  My::MyECS::ArchetypeFilter filter;
  filter.all = {My::MyECS::CmptAccessType::Of<My::MyGE::MeshRenderer>};
  auto meshRenderers =
      world.entityMngr.GetCmptArray<My::MyGE::MeshRenderer>(filter);
  for (auto meshRenderer : meshRenderers)
    meshRenderer->materials.push_back(material);
}
