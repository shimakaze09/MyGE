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
#include <MyGE/Core/ShaderMngr.h>
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

struct AnimateMeshSystem : My::MyECS::System {
  using My::MyECS::System::System;
  size_t cnt = 0;

  virtual void OnUpdate(My::MyECS::Schedule& schedule) override {
    if (cnt < 600) {
      schedule.RegisterEntityJob(
          [](My::MyGE::MeshFilter* meshFilter) {
            if (meshFilter->mesh->IsEditable()) {
              auto positions = meshFilter->mesh->GetPositions();
              for (auto& pos : positions)
                pos[1] = 0.2f * (My::rand01<float>() - 0.5f);
              meshFilter->mesh->SetPositions(positions);
            }
          },
          "AnimateMesh");
    } else if (cnt == 600) {
      schedule.RegisterEntityJob(
          [](My::MyGE::MeshFilter* meshFilter) {
            meshFilter->mesh->SetToNonEditable();
          },
          "set mesh static");
    }
    cnt++;
  }
};

class DynamicMeshApp : public D3DApp {
 public:
  DynamicMeshApp(HINSTANCE hInstance);
  DynamicMeshApp(const DynamicMeshApp& rhs) = delete;
  DynamicMeshApp& operator=(const DynamicMeshApp& rhs) = delete;
  ~DynamicMeshApp();

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
  void BuildShaders();
  void BuildMaterials();

 private:
  float mTheta = 0.4f * XM_PI;
  float mPhi = 1.3f * XM_PI;
  float mRadius = 5.0f;

  POINT mLastMousePos;

  My::MyECS::World world;
  My::MyECS::Entity cam{My::MyECS::Entity::Invalid()};

  std::unique_ptr<My::MyGE::IPipeline> pipeline;
  std::unique_ptr<My::MyGE::Mesh> dynamicMesh;

  std::unique_ptr<My::MyDX12::FrameResourceMngr> frameRsrcMngr;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  try {
    DynamicMeshApp theApp(hInstance);
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

DynamicMeshApp::DynamicMeshApp(HINSTANCE hInstance) : D3DApp(hInstance) {}

DynamicMeshApp::~DynamicMeshApp() {
  if (!myDevice.IsNull())
    FlushCommandQueue();
}

bool DynamicMeshApp::Initialize() {
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

  My::MyGE::MeshLayoutMngr::Instance().Init();

  My::MyGE::AssetMngr::Instance().ImportAssetRecursively(L"..\\assets");

  BuildWorld();

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();
  LoadTextures();
  BuildShaders();
  BuildMaterials();
  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(myCmdQueue.raw.Get());

  My::MyGE::IPipeline::InitDesc initDesc;
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

void DynamicMeshApp::OnResize() {
  D3DApp::OnResize();

  if (pipeline)
    pipeline->Resize(mClientWidth, mClientHeight, mScreenViewport,
                     mScissorRect);
}

void DynamicMeshApp::Update() {
  OnKeyboardInput();
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
  upload.End(myCmdQueue.raw.Get());
  myGCmdList->Close();
  myCmdQueue.Execute(myGCmdList.raw.Get());
  deleteBatch.Commit(myDevice.raw.Get(), myCmdQueue.raw.Get());
  frameRsrcMngr->EndFrame(myCmdQueue.raw.Get());

  std::vector<My::MyGE::IPipeline::CameraData> gameCameras;
  My::MyECS::ArchetypeFilter camFilter{
      {My::MyECS::CmptAccessType::Of<My::MyGE::Camera>}};
  world.RunEntityJob(
      [&](My::MyECS::Entity e) { gameCameras.emplace_back(e, world); }, false,
      camFilter);
  pipeline->BeginFrame(world, gameCameras);
}

void DynamicMeshApp::Draw() {
  pipeline->Render(CurrentBackBuffer());
  // Swap the back and front buffers
  ThrowIfFailed(mSwapChain->Present(0, 0));
  mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
  pipeline->EndFrame();
}

void DynamicMeshApp::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(mhMainWnd);
}

void DynamicMeshApp::OnMouseUp(WPARAM btnState, int x, int y) {
  ReleaseCapture();
}

void DynamicMeshApp::OnMouseMove(WPARAM btnState, int x, int y) {
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

void DynamicMeshApp::OnKeyboardInput() {}

void DynamicMeshApp::UpdateCamera() {
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

void DynamicMeshApp::BuildWorld() {
  world.systemMngr.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      AnimateMeshSystem>();

  auto e0 =
      world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::WorldToLocal,
                              My::MyGE::Camera, My::MyGE::Translation,
                              My::MyGE::Rotation>();
  cam = std::get<My::MyECS::Entity>(e0);

  auto quadMesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      "../assets/models/quad.obj");
  auto dynamicCube =
      world.entityMngr.Create<My::MyGE::LocalToWorld, My::MyGE::MeshFilter,
                              My::MyGE::MeshRenderer, My::MyGE::Translation,
                              My::MyGE::Rotation, My::MyGE::Scale>();
  dynamicMesh = std::make_unique<My::MyGE::Mesh>();
  dynamicMesh->SetPositions(quadMesh->GetPositions());
  dynamicMesh->SetNormals(quadMesh->GetNormals());
  dynamicMesh->SetUV(quadMesh->GetUV());
  dynamicMesh->SetIndices(quadMesh->GetIndices());
  dynamicMesh->SetSubMeshCount(quadMesh->GetSubMeshes().size());
  for (size_t i = 0; i < quadMesh->GetSubMeshes().size(); i++)
    dynamicMesh->SetSubMesh(i, quadMesh->GetSubMeshes().at(i));
  std::get<My::MyGE::MeshFilter*>(dynamicCube)->mesh = dynamicMesh.get();
}

void DynamicMeshApp::LoadTextures() {
  auto tex2dGUIDs =
      My::MyGE::AssetMngr::Instance().FindAssets(std::wregex{LR"(.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::RsrcMngrDX12::Instance().RegisterTexture2D(
        My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
        My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(path));
  }
}

void DynamicMeshApp::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::RsrcMngrDX12::Instance().RegisterShader(shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}

void DynamicMeshApp::BuildMaterials() {
  auto material = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(
      L"..\\assets\\materials\\iron.mat");
  world.RunEntityJob([=](My::MyGE::MeshRenderer* meshRenderer) {
    meshRenderer->materials.push_back(material);
  });
}
