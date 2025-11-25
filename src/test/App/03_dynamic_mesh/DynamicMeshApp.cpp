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

using Microsoft::WRL::ComPtr;
using namespace DirectX;

const int gNumFrameResources = 3;

struct AnimateMeshSystem {
  size_t cnt = 0;
  static void OnUpdate(My::MyECS::Schedule& schedule) {
    schedule.RegisterEntityJob(
        [](My::MyGE::MeshFilter* meshFilter,
           My::MyECS::Latest<My::MyECS::Singleton<My::MyGE::WorldTime>>
               time) {
          if (!meshFilter->mesh) return;
          if (time->elapsedTime < 5.f) {
            if (meshFilter->mesh->IsEditable()) {
              auto positions = meshFilter->mesh->GetPositions();
              for (auto& pos : positions)
                pos[1] = 0.2f * (My::rand01<float>() - 0.5f);
              meshFilter->mesh->SetPositions(positions);
            }
          } else if (5.f < time->elapsedTime && time->elapsedTime < 7.f)
            // meshFilter->mesh->SetToNonEditable();
            ;
          else if (7.f < time->elapsedTime && time->elapsedTime < 9.f) {
            // meshFilter->mesh->SetToEditable();
            auto positions = meshFilter->mesh->GetPositions();
            for (auto& pos : positions)
              pos[1] = 0.2f * (My::rand01<float>() - 0.5f);
            meshFilter->mesh->SetPositions(positions);
          } else
            // meshFilter->mesh->SetToNonEditable();
            ;
        },
        "AnimateMesh");
    schedule.RegisterEntityJob(
        [](My::MyGE::MeshFilter* meshFilter,
           My::MyECS::Latest<My::MyECS::Singleton<My::MyGE::WorldTime>>
               time) {
          if (!meshFilter->mesh) return;
          if (time->elapsedTime > 10.f) {
            My::MyGE::GPURsrcMngrDX12::Instance().UnregisterMesh(
                meshFilter->mesh->GetInstanceID());
            meshFilter->mesh = nullptr;
          }
        },
        "DeleteMesh");
    schedule.RegisterJob([](My::MyECS::World* w) {
      auto time = w->entityMngr.ReadSingleton<My::MyGE::WorldTime>();
      if (!time) return;

      if (time->elapsedTime < 12.f) return;

      w->systemMngr.Deactivate<AnimateMeshSystem>();
    }, "StopAnimateMesh");
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

  std::shared_ptr<My::MyGE::Mesh> dynamicMesh;

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
    DynamicMeshApp theApp(hInstance);
    if (!theApp.Initialize()) return 0;

    rst = theApp.Run();
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    return 0;
  }

  return rst;
}

DynamicMeshApp::DynamicMeshApp(HINSTANCE hInstance) : D3DApp(hInstance) {
  mMainWndCaption = L"DynamicMeshApp";
}

DynamicMeshApp::~DynamicMeshApp() {}

bool DynamicMeshApp::Initialize() {
  if (!D3DApp::Initialize()) return false;

  // Reset the command list to prep for initialization commands.
  ThrowIfFailed(myGCmdList->Reset(mDirectCmdListAlloc.Get(), nullptr));

  frameRsrcMngr = std::make_unique<My::MyDX12::FrameResourceMngr>(
      gNumFrameResources, myDevice.raw.Get());

  LoadTextures();
  BuildShaders();
  BuildWorld();
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

void DynamicMeshApp::OnResize() {
  D3DApp::OnResize();

  if (pipeline)
    pipeline->Resize(mClientWidth, mClientHeight, mScreenViewport,
                     mScissorRect);
}

void DynamicMeshApp::Update() {
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

void DynamicMeshApp::OnMouseUp(WPARAM btnState, int x, int y) { ReleaseCapture(); }

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

void DynamicMeshApp::UpdateCamera() {
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

void DynamicMeshApp::BuildWorld() {
  auto systemIDs = world.systemMngr.systemTraits.Register<
      My::MyGE::CameraSystem, My::MyGE::LocalToParentSystem,
      My::MyGE::RotationEulerSystem, My::MyGE::TRSToLocalToParentSystem,
      My::MyGE::TRSToLocalToWorldSystem, My::MyGE::WorldToLocalSystem,
      My::MyGE::WorldTimeSystem, AnimateMeshSystem>();
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

  {
    std::vector<My::TypeID> camTypes = {
        My::TypeID_of<My::MyGE::LocalToWorld>,
        My::TypeID_of<My::MyGE::WorldToLocal>,
        My::TypeID_of<My::MyGE::Camera>,
        My::TypeID_of<My::MyGE::Translation>,
        My::TypeID_of<My::MyGE::Rotation>
    };
    cam = world.entityMngr.Create(camTypes);
  }
  {
    std::vector<My::TypeID> types = {My::TypeID_of<My::MyGE::WorldTime>};
    world.entityMngr.Create(types);
  }

  auto quadMesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      "../assets/models/quad.obj");
  std::vector<My::TypeID> cubeTypes = {
      My::TypeID_of<My::MyGE::LocalToWorld>,
      My::TypeID_of<My::MyGE::MeshFilter>,
      My::TypeID_of<My::MyGE::MeshRenderer>,
      My::TypeID_of<My::MyGE::Translation>,
      My::TypeID_of<My::MyGE::Rotation>,
      My::TypeID_of<My::MyGE::Scale>
  };
  auto dynamicCube = world.entityMngr.Create(cubeTypes);
  dynamicMesh = std::make_shared<My::MyGE::Mesh>();
  dynamicMesh->SetPositions(quadMesh->GetPositions());
  dynamicMesh->SetNormals(quadMesh->GetNormals());
  dynamicMesh->SetUV(quadMesh->GetUV());
  dynamicMesh->SetIndices(quadMesh->GetIndices());
  dynamicMesh->SetSubMeshCount(quadMesh->GetSubMeshes().size());
  for (size_t i = 0; i < quadMesh->GetSubMeshes().size(); i++)
    dynamicMesh->SetSubMesh(i, quadMesh->GetSubMeshes().at(i));
  world.entityMngr.WriteComponent<My::MyGE::MeshFilter>(dynamicCube)->mesh = dynamicMesh;
}

void DynamicMeshApp::LoadTextures() {
  auto tex2dGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.tex2d)"});
  for (const auto& guid : tex2dGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::GPURsrcMngrDX12::Instance().RegisterTexture2D(
        const_cast<My::MyGE::Texture2D&>(*My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Texture2D>(
            path)));
  }

  auto texcubeGUIDs = My::MyGE::AssetMngr::Instance().FindAssets(
      std::wregex{LR"(\.\.\\assets\\_internal\\.*\.texcube)"});
  for (const auto& guid : texcubeGUIDs) {
    const auto& path = My::MyGE::AssetMngr::Instance().GUIDToAssetPath(guid);
    My::MyGE::GPURsrcMngrDX12::Instance().RegisterTextureCube(
        const_cast<My::MyGE::TextureCube&>(*My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::TextureCube>(
            path)));
  }
}

void DynamicMeshApp::BuildShaders() {
  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto shaderGUIDs = assetMngr.FindAssets(std::wregex{LR"(.*\.shader)"});
  for (const auto& guid : shaderGUIDs) {
    const auto& path = assetMngr.GUIDToAssetPath(guid);
    auto shader = assetMngr.LoadAsset<My::MyGE::Shader>(path);
    My::MyGE::GPURsrcMngrDX12::Instance().RegisterShader(*shader);
    My::MyGE::ShaderMngr::Instance().Register(shader);
  }
}

void DynamicMeshApp::BuildMaterials() {
  auto material =
      My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Material>(
          L"..\\assets\\materials\\iron.mat");
  world.RunEntityJob([=](My::MyGE::MeshRenderer* meshRenderer) {
    meshRenderer->materials.push_back(material);
  });
}



