//
// Created by Admin on 16/03/2025.
//

#include "../common/d3dApp.h"

#include <MyGE/Asset/AssetMngr.h>

#include <MyGE/Core/HLSLFile.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Mesh.h>
#include <MyGE/Core/Shader.h>
#include <MyGE/Core/Texture2D.h>

#include <MyDX12/UploadBuffer.h>

#include <MyGM/MyGM.h>

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

const int gNumFrameResources = 3;

constexpr size_t ID_RootSignature_geometry = 0;
constexpr size_t ID_RootSignature_screen = 1;
constexpr size_t ID_RootSignature_defer_light = 2;

struct GeometryObjectConstants {
  My::transformf World;
};

struct CameraConstants {
  My::transformf View;

  My::transformf InvView;

  My::transformf Proj;

  My::transformf InvProj;

  My::transformf ViewProj;

  My::transformf InvViewProj;

  My::pointf3 EyePosW;
  float _pad0;

  My::valf2 RenderTargetSize;
  My::valf2 InvRenderTargetSize;

  float NearZ;
  float FarZ;
  float TotalTime;
  float DeltaTime;
};

struct GeometryMaterialConstants {
  My::rgbf albedoFactor;
  float roughnessFactor;
  float metalnessFactor;
};

struct DirectionalLight {
  My::rgbf L;
  float _pad0;
  My::vecf3 dir;
  float _pad1;
};

struct LightingLights {
  UINT diectionalLightNum;
  UINT _pad0;
  UINT _pad1;
  UINT _pad2;
  DirectionalLight directionalLights[4];
};

// Lightweight structure stores parameters to draw a shape.  This will
// vary from app-to-app.
struct RenderItem {
  RenderItem() = default;

  // World matrix of the shape that describes the object's local space
  // relative to the world space, which defines the position, orientation,
  // and scale of the object in the world.
  My::transformf World = My::transformf::eye();

  My::transformf TexTransform = My::transformf::eye();

  // Dirty flag indicating the object data has changed and we need to update the constant buffer.
  // Because we have an object cbuffer for each FrameResource, we have to apply the
  // update to each FrameResource.  Thus, when we modify obect data we should set
  // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
  int NumFramesDirty = gNumFrameResources;

  // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
  UINT ObjCBIndex = -1;

  Material* Mat = nullptr;
  My::MyDX12::MeshGPUBuffer* Geo = nullptr;
  //std::string Geo;

  // Primitive topology.
  D3D12_PRIMITIVE_TOPOLOGY PrimitiveType =
      D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

  // DrawIndexedInstanced parameters.
  UINT IndexCount = 0;
  UINT StartIndexLocation = 0;
  int BaseVertexLocation = 0;
};

class DeferApp : public D3DApp {
 public:
  DeferApp(HINSTANCE hInstance);
  DeferApp(const DeferApp& rhs) = delete;
  DeferApp& operator=(const DeferApp& rhs) = delete;
  ~DeferApp();

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
  void UpdateObjectCBs();
  void UpdateMaterialCBs();
  void UpdateMainCameraCB();
  void UpdateLightsCB();

  void LoadTextures();
  void BuildRootSignature();
  void BuildShadersAndInputLayout();
  void BuildShapeGeometry();
  void BuildPSOs();
  void BuildFrameResources();
  void BuildMaterials();
  void BuildRenderItems();
  void DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                       const std::vector<RenderItem*>& ritems);

 private:
  std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;

  std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

  // List of all the render items.
  std::vector<std::unique_ptr<RenderItem>> mAllRitems;

  // Render items divided by PSO.
  std::vector<RenderItem*> mOpaqueRitems;

  CameraConstants mMainCameraCB;

  My::pointf3 mEyePos = {0.0f, 0.0f, 0.0f};
  My::transformf mView = My::transformf::eye();
  My::transformf mProj = My::transformf::eye();

  float mTheta = 0.4f * XM_PI;
  float mPhi = 1.3f * XM_PI;
  float mRadius = 5.0f;

  POINT mLastMousePos;

  // frame graph
  //My::MyDX12::FG::RsrcMngr fgRsrcMngr;
  My::MyDX12::FG::Executor fgExecutor;
  My::MyFG::Compiler fgCompiler;
  My::MyFG::FrameGraph fg;

  My::MyGE::Texture2D* albedoTex2D;
  My::MyGE::Texture2D* roughnessTex2D;
  My::MyGE::Texture2D* metalnessTex2D;

  My::MyGE::Shader* screenShader;
  My::MyGE::Shader* geomrtryShader;
  My::MyGE::Shader* deferShader;

  My::MyGE::Mesh* mesh;

  std::unique_ptr<My::MyDX12::FrameResourceMngr> frameRsrcMngr;

  size_t ID_PSO_geometry;
  size_t ID_PSO_defer_light;
  size_t ID_PSO_screen;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine,
                   int showCmd) {
  // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

  try {
    DeferApp theApp(hInstance);
    if (!theApp.Initialize())
      return 0;

    int rst = theApp.Run();
    My::MyGE::RsrcMngrDX12::Instance().Clear();
    return rst;
  } catch (My::MyDX12::Util::Exception& e) {
    MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
    return 0;
  }
}

DeferApp::DeferApp(HINSTANCE hInstance)
    : D3DApp(hInstance), fg{"frame graph"} {}

DeferApp::~DeferApp() {
  if (!myDevice.IsNull())
    FlushCommandQueue();
}

bool DeferApp::Initialize() {
  if (!D3DApp::Initialize())
    return false;

  frameRsrcMngr = std::make_unique<My::MyDX12::FrameResourceMngr>(
      gNumFrameResources, myDevice.raw.Get());

  My::MyGE::RsrcMngrDX12::Instance().Init(myDevice.raw.Get());

  My::MyDX12::DescriptorHeapMngr::Instance().Init(myDevice.raw.Get(), 1024,
                                                  1024, 1024, 1024, 1024);

  //fgRsrcMngr.Init(myGCmdList, myDevice);

  // Reset the command list to prep for initialization commands.
  ThrowIfFailed(myGCmdList->Reset(mDirectCmdListAlloc.Get(), nullptr));

  // Get the increment size of a descriptor in this heap type.  This is hardware specific,
  // so we have to query this information.
  //mCbvSrvDescriptorSize = myDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().Begin();

  LoadTextures();
  BuildRootSignature();
  BuildShadersAndInputLayout();
  BuildShapeGeometry();
  BuildMaterials();
  BuildRenderItems();
  BuildFrameResources();
  BuildPSOs();

  // Execute the initialization commands.
  ThrowIfFailed(myGCmdList->Close());
  myCmdQueue.Execute(myGCmdList.raw.Get());

  My::MyGE::RsrcMngrDX12::Instance().GetUpload().End(myCmdQueue.raw.Get());

  // Wait until initialization is complete.
  FlushCommandQueue();

  return true;
}

void DeferApp::OnResize() {
  D3DApp::OnResize();

  // The window resized, so update the aspect ratio and recompute the projection matrix.
  mProj = My::transformf::perspective(0.25f * My::PI<float>, AspectRatio(),
                                      1.0f, 1000.0f, 0.f);

  auto clearFGRsrcMngr =
      [](std::shared_ptr<My::MyDX12::FG::RsrcMngr> rsrcMngr) {
        rsrcMngr->Clear();
      };

  if (frameRsrcMngr) {
    for (auto& frsrc : frameRsrcMngr->GetFrameResources())
      frsrc->DelayUpdateResource("FrameGraphRsrcMngr", clearFGRsrcMngr);
  }
}

void DeferApp::Update() {
  OnKeyboardInput();
  UpdateCamera();

  // Cycle through the circular frame resource array.
  // Has the GPU finished processing the commands of the current frame resource?
  // If not, wait until the GPU has completed commands up to this fence point.
  frameRsrcMngr->BeginFrame();

  UpdateObjectCBs();
  UpdateMaterialCBs();
  UpdateMainCameraCB();
  UpdateLightsCB();
}

void DeferApp::Draw() {
  auto cmdAlloc =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(
              "CommandAllocator");

  // Reuse the memory associated with command recording.
  // We can only reset when the associated command lists have finished execution on the GPU.
  ThrowIfFailed(cmdAlloc->Reset());

  // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
  // Reusing the command list reuses memory.
  //ThrowIfFailed(myGCmdList->Reset(cmdAlloc.Get(), nullptr));

  fg.Clear();
  auto fgRsrcMngr =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<std::shared_ptr<My::MyDX12::FG::RsrcMngr>>(
              "FrameGraphRsrcMngr");
  fgRsrcMngr->NewFrame();
  fgExecutor.NewFrame();
  ;

  auto gbuffer0 = fg.RegisterResourceNode("GBuffer0");
  auto gbuffer1 = fg.RegisterResourceNode("GBuffer1");
  auto gbuffer2 = fg.RegisterResourceNode("GBuffer2");
  auto backbuffer = fg.RegisterResourceNode("Back Buffer");
  auto depthstencil = fg.RegisterResourceNode("Depth Stencil");
  auto gbPass = fg.RegisterPassNode(
      "GBuffer Pass", {}, {gbuffer0, gbuffer1, gbuffer2, depthstencil});
  /*auto debugPass = fg.AddPassNode(
		"Debug",
		{ gbuffer1 },
		{ backbuffer }
	);*/
  auto deferLightingPass = fg.RegisterPassNode(
      "Defer Lighting", {gbuffer0, gbuffer1, gbuffer2}, {backbuffer});

  auto srvDesc = My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT);
  auto dsvDesc = My::MyDX12::Desc::DSV::Basic(mDepthStencilFormat);
  (*fgRsrcMngr)
      .RegisterTemporalRsrc(gbuffer0,
                            My::MyDX12::FG::RsrcType::RT2D(
                                DXGI_FORMAT_R32G32B32A32_FLOAT, mClientWidth,
                                mClientHeight, Colors::Black))
      .RegisterTemporalRsrc(gbuffer1,
                            My::MyDX12::FG::RsrcType::RT2D(
                                DXGI_FORMAT_R32G32B32A32_FLOAT, mClientWidth,
                                mClientHeight, Colors::Black))
      .RegisterTemporalRsrc(gbuffer2,
                            My::MyDX12::FG::RsrcType::RT2D(
                                DXGI_FORMAT_R32G32B32A32_FLOAT, mClientWidth,
                                mClientHeight, Colors::Black))

      .RegisterRsrcTable(
          {{gbuffer0, srvDesc}, {gbuffer1, srvDesc}, {gbuffer2, srvDesc}})

      .RegisterImportedRsrc(backbuffer,
                            {CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT})
      .RegisterImportedRsrc(depthstencil, {mDepthStencilBuffer.Get(),
                                           D3D12_RESOURCE_STATE_DEPTH_WRITE})

      .RegisterPassRsrc(gbPass, gbuffer0, D3D12_RESOURCE_STATE_RENDER_TARGET,
                        My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrc(gbPass, gbuffer1, D3D12_RESOURCE_STATE_RENDER_TARGET,
                        My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrc(gbPass, gbuffer2, D3D12_RESOURCE_STATE_RENDER_TARGET,
                        My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrc(gbPass, depthstencil, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                        My::MyDX12::Desc::DSV::Basic(mDepthStencilFormat))

      /*.RegisterPassRsrc(debugPass, gbuffer1, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))

		.RegisterPassRsrc(debugPass, backbuffer, D3D12_RESOURCE_STATE_RENDER_TARGET,
			My::MyDX12::FG::RsrcImplDesc_RTV_Null{})*/

      .RegisterPassRsrc(
          deferLightingPass, gbuffer0,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))
      .RegisterPassRsrc(
          deferLightingPass, gbuffer1,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))
      .RegisterPassRsrc(
          deferLightingPass, gbuffer2,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))

      .RegisterPassRsrc(deferLightingPass, backbuffer,
                        D3D12_RESOURCE_STATE_RENDER_TARGET,
                        My::MyDX12::FG::RsrcImplDesc_RTV_Null{});

  fgExecutor.RegisterPassFunc(
      gbPass, [&](ID3D12GraphicsCommandList* cmdList,
                  const My::MyDX12::FG::PassRsrcs& rsrcs) {
        auto heap = My::MyDX12::DescriptorHeapMngr::Instance()
                        .GetCSUGpuDH()
                        ->GetDescriptorHeap();
        cmdList->SetDescriptorHeaps(1, &heap);
        cmdList->RSSetViewports(1, &mScreenViewport);
        cmdList->RSSetScissorRects(1, &mScissorRect);

        cmdList->SetPipelineState(
            My::MyGE::RsrcMngrDX12::Instance().GetPSO(ID_PSO_geometry));
        auto gb0 = rsrcs.find(gbuffer0)->second;
        auto gb1 = rsrcs.find(gbuffer1)->second;
        auto gb2 = rsrcs.find(gbuffer2)->second;
        auto ds = rsrcs.find(depthstencil)->second;

        std::array rts{gb0.info.desc2info_srv.at(srvDesc).cpuHandle,
                       gb1.info.desc2info_srv.at(srvDesc).cpuHandle,
                       gb2.info.desc2info_srv.at(srvDesc).cpuHandle};
        // Clear the render texture and depth buffer.
        cmdList->ClearRenderTargetView(rts[0], Colors::Black, 0, nullptr);
        cmdList->ClearRenderTargetView(rts[1], Colors::Black, 0, nullptr);
        cmdList->ClearRenderTargetView(rts[2], Colors::Black, 0, nullptr);
        cmdList->ClearDepthStencilView(
            ds.info.desc2info_dsv.at(dsvDesc).cpuHandle,
            D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0,
            nullptr);

        // Specify the buffers we are going to render to.
        cmdList->OMSetRenderTargets(
            rts.size(), rts.data(), false,
            &ds.info.desc2info_dsv.at(dsvDesc).cpuHandle);

        cmdList->SetGraphicsRootSignature(
            My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
                ID_RootSignature_geometry));

        auto cameraCB =
            frameRsrcMngr->GetCurrentFrameResource()
                ->GetResource<My::MyDX12::ArrayUploadBuffer<CameraConstants>>(
                    "camera constants")
                .GetResource();
        cmdList->SetGraphicsRootConstantBufferView(
            5, cameraCB->GetGPUVirtualAddress());

        DrawRenderItems(cmdList, mOpaqueRitems);
      });

  fgExecutor.RegisterPassFunc(
      deferLightingPass, [&](ID3D12GraphicsCommandList* cmdList,
                             const My::MyDX12::FG::PassRsrcs& rsrcs) {
        auto heap = My::MyDX12::DescriptorHeapMngr::Instance()
                        .GetCSUGpuDH()
                        ->GetDescriptorHeap();
        cmdList->SetDescriptorHeaps(1, &heap);
        cmdList->RSSetViewports(1, &mScreenViewport);
        cmdList->RSSetScissorRects(1, &mScissorRect);

        auto gb0 = rsrcs.find(gbuffer0)->second;
        auto gb1 = rsrcs.find(gbuffer1)->second;
        auto gb2 = rsrcs.find(gbuffer2)->second;

        auto bb = rsrcs.find(backbuffer)->second;

        //cmdList->CopyResource(bb.resource, rt.resource);

        // Clear the render texture and depth buffer.
        cmdList->ClearRenderTargetView(bb.info.null_info_rtv.cpuHandle,
                                       Colors::LightSteelBlue, 0, nullptr);

        // Specify the buffers we are going to render to.
        cmdList->OMSetRenderTargets(1, &bb.info.null_info_rtv.cpuHandle, false,
                                    nullptr);

        cmdList->SetGraphicsRootSignature(
            My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
                ID_RootSignature_defer_light));

        cmdList->SetPipelineState(
            My::MyGE::RsrcMngrDX12::Instance().GetPSO(ID_PSO_defer_light));

        cmdList->SetGraphicsRootDescriptorTable(
            0, gb0.info.desc2info_srv.at(srvDesc).gpuHandle);
        auto lightsCB =
            frameRsrcMngr->GetCurrentFrameResource()
                ->GetResource<My::MyDX12::ArrayUploadBuffer<LightingLights>>(
                    "lights")
                .GetResource();
        cmdList->SetGraphicsRootConstantBufferView(
            1, lightsCB->GetGPUVirtualAddress());
        auto cameraCB =
            frameRsrcMngr->GetCurrentFrameResource()
                ->GetResource<My::MyDX12::ArrayUploadBuffer<CameraConstants>>(
                    "camera constants")
                .GetResource();
        cmdList->SetGraphicsRootConstantBufferView(
            2, cameraCB->GetGPUVirtualAddress());

        cmdList->IASetVertexBuffers(0, 0, nullptr);
        cmdList->IASetIndexBuffer(nullptr);
        cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        cmdList->DrawInstanced(6, 1, 0, 0);
      });

  static bool flag{false};
  if (!flag) {
    OutputDebugStringA(fg.ToGraphvizGraph().Dump().c_str());
    flag = true;
  }

  auto [success, crst] = fgCompiler.Compile(fg);
  fgExecutor.Execute(myDevice.raw.Get(), myCmdQueue.raw.Get(), cmdAlloc.Get(),
                     crst, *fgRsrcMngr);

  //   // Done recording commands.
  //   ThrowIfFailed(myGCmdList->Close());

  //   // Add the command list to the queue for execution.
  //myCmdQueue.Execute(myGCmdList.raw.Get());

  // Swap the back and front buffers
  ThrowIfFailed(mSwapChain->Present(0, 0));
  mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

  frameRsrcMngr->EndFrame(myCmdQueue.raw.Get());
}

void DeferApp::OnMouseDown(WPARAM btnState, int x, int y) {
  mLastMousePos.x = x;
  mLastMousePos.y = y;

  SetCapture(mhMainWnd);
}

void DeferApp::OnMouseUp(WPARAM btnState, int x, int y) {
  ReleaseCapture();
}

void DeferApp::OnMouseMove(WPARAM btnState, int x, int y) {
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

void DeferApp::OnKeyboardInput() {}

void DeferApp::UpdateCamera() {
  // Convert Spherical to Cartesian coordinates.
  mEyePos[0] = mRadius * sinf(mTheta) * sinf(mPhi);
  mEyePos[1] = mRadius * cosf(mTheta);
  mEyePos[2] = mRadius * sinf(mTheta) * cosf(mPhi);
  mView = My::transformf::look_at(mEyePos, {0.f});
}

void DeferApp::UpdateObjectCBs() {
  auto& currObjectCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<GeometryObjectConstants>>(
              "ArrayUploadBuffer<GeometryObjectConstants>");
  for (auto& e : mAllRitems) {
    // Only update the cbuffer data if the constants have changed.
    // This needs to be tracked per frame resource.
    if (e->NumFramesDirty > 0) {
      GeometryObjectConstants objConstants;
      objConstants.World = e->World;

      currObjectCB.Set(e->ObjCBIndex, objConstants);

      // Next FrameResource need to be updated too.
      e->NumFramesDirty--;
    }
  }
}

void DeferApp::UpdateMaterialCBs() {
  auto& currMaterialCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<
              My::MyDX12::ArrayUploadBuffer<GeometryMaterialConstants>>(
              "ArrayUploadBuffer<GeometryMaterialConstants>");
  for (auto& e : mMaterials) {
    // Only update the cbuffer data if the constants have changed.  If the cbuffer
    // data changes, it needs to be updated for each FrameResource.
    Material* mat = e.second.get();
    if (mat->NumFramesDirty > 0) {
      XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

      GeometryMaterialConstants matConstants;
      matConstants.albedoFactor = {1.f};
      matConstants.metalnessFactor = 1.f;
      matConstants.roughnessFactor = 1.f;

      currMaterialCB.Set(mat->MatCBIndex, matConstants);

      // Next FrameResource need to be updated too.
      mat->NumFramesDirty--;
    }
  }
}

void DeferApp::UpdateMainCameraCB() {
  mMainCameraCB.View = mView;
  mMainCameraCB.InvView = mMainCameraCB.View.inverse();
  mMainCameraCB.Proj = mProj;
  mMainCameraCB.InvProj = mMainCameraCB.Proj.inverse();
  mMainCameraCB.ViewProj = mMainCameraCB.Proj * mMainCameraCB.View;
  mMainCameraCB.InvViewProj = mMainCameraCB.InvView * mMainCameraCB.InvProj;
  mMainCameraCB.EyePosW = mEyePos;
  mMainCameraCB.RenderTargetSize = {mClientWidth, mClientHeight};
  mMainCameraCB.InvRenderTargetSize = {1.0f / mClientWidth,
                                       1.0f / mClientHeight};

  mMainCameraCB.NearZ = 1.0f;
  mMainCameraCB.FarZ = 1000.0f;
  mMainCameraCB.TotalTime = My::MyGE::GameTimer::Instance().TotalTime();
  mMainCameraCB.DeltaTime = My::MyGE::GameTimer::Instance().DeltaTime();

  auto& cameraCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<CameraConstants>>(
              "camera constants");
  cameraCB.Set(0, mMainCameraCB);
}

void DeferApp::UpdateLightsCB() {
  LightingLights lights;
  lights.diectionalLightNum = 3;
  lights.directionalLights[0].L = {6.f};
  lights.directionalLights[0].dir = {0.57735f, -0.57735f, 0.57735f};
  lights.directionalLights[1].L = {3.f};
  lights.directionalLights[1].dir = {-0.57735f, -0.57735f, 0.57735f};
  lights.directionalLights[2].L = {1.5f};
  lights.directionalLights[2].dir = {0.0f, -0.707f, -0.707f};

  auto& lightsCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<LightingLights>>(
              "lights");
  lightsCB.Set(0, lights);
}

void DeferApp::LoadTextures() {
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

void DeferApp::BuildRootSignature() {
  {  // geometry
    CD3DX12_DESCRIPTOR_RANGE texRange0;
    texRange0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
    CD3DX12_DESCRIPTOR_RANGE texRange1;
    texRange1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
    CD3DX12_DESCRIPTOR_RANGE texRange2;
    texRange2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[6];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texRange0);
    slotRootParameter[1].InitAsDescriptorTable(1, &texRange1);
    slotRootParameter[2].InitAsDescriptorTable(1, &texRange2);
    slotRootParameter[3].InitAsConstantBufferView(0);  // obj
    slotRootParameter[4].InitAsConstantBufferView(1);  // mat
    slotRootParameter[5].InitAsConstantBufferView(2);  // cam

    auto staticSamplers =
        My::MyGE::RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        6, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    My::MyGE::RsrcMngrDX12::Instance().RegisterRootSignature(
        ID_RootSignature_geometry, &rootSigDesc);
  }

  {  // screen
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable,
                                               D3D12_SHADER_VISIBILITY_PIXEL);

    auto staticSamplers =
        My::MyGE::RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        1, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    My::MyGE::RsrcMngrDX12::Instance().RegisterRootSignature(
        ID_RootSignature_screen, &rootSigDesc);
  }
  {  // defer lighting
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[3];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable,
                                               D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsConstantBufferView(0);  // lights
    slotRootParameter[2].InitAsConstantBufferView(1);  // camera

    auto staticSamplers =
        My::MyGE::RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        3, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    My::MyGE::RsrcMngrDX12::Instance().RegisterRootSignature(
        ID_RootSignature_defer_light, &rootSigDesc);
  }
}

void DeferApp::BuildShadersAndInputLayout() {
  std::filesystem::path hlslScreenPath = "../assets/shaders/Screen.hlsl";
  std::filesystem::path shaderScreenPath = "../assets/shaders/Screen.shader";
  std::filesystem::path hlslGeomrtryPath = "../assets/shaders/Geometry.hlsl";
  std::filesystem::path shaderGeometryPath =
      "../assets/shaders/Geometry.shader";
  std::filesystem::path hlslDeferPath = "../assets/shaders/deferLighting.hlsl";
  std::filesystem::path shaderDeferPath =
      "../assets/shaders/deferLighting.shader";

  if (!std::filesystem::is_directory("../assets/shaders"))
    std::filesystem::create_directories("../assets/shaders");

  auto& assetMngr = My::MyGE::AssetMngr::Instance();
  auto hlslScreen = assetMngr.LoadAsset<My::MyGE::HLSLFile>(hlslScreenPath);
  auto hlslGeomrtry = assetMngr.LoadAsset<My::MyGE::HLSLFile>(hlslGeomrtryPath);
  auto hlslDefer = assetMngr.LoadAsset<My::MyGE::HLSLFile>(hlslDeferPath);

  screenShader = new My::MyGE::Shader;
  geomrtryShader = new My::MyGE::Shader;
  deferShader = new My::MyGE::Shader;

  screenShader->hlslFile = hlslScreen;
  geomrtryShader->hlslFile = hlslGeomrtry;
  deferShader->hlslFile = hlslDefer;

  screenShader->vertexName = "VS";
  geomrtryShader->vertexName = "VS";
  deferShader->vertexName = "VS";

  screenShader->fragmentName = "PS";
  geomrtryShader->fragmentName = "PS";
  deferShader->fragmentName = "PS";

  screenShader->targetName = "5_0";
  geomrtryShader->targetName = "5_0";
  deferShader->targetName = "5_0";

  screenShader->shaderName = "Screen";
  geomrtryShader->shaderName = "Geometry";
  deferShader->shaderName = "Defer";

  if (!assetMngr.CreateAsset(screenShader, shaderScreenPath)) {
    delete screenShader;
    screenShader = assetMngr.LoadAsset<My::MyGE::Shader>(shaderScreenPath);
  }

  if (!assetMngr.CreateAsset(geomrtryShader, shaderGeometryPath)) {
    delete geomrtryShader;
    geomrtryShader = assetMngr.LoadAsset<My::MyGE::Shader>(shaderGeometryPath);
  }

  if (!assetMngr.CreateAsset(deferShader, shaderDeferPath)) {
    delete deferShader;
    deferShader = assetMngr.LoadAsset<My::MyGE::Shader>(shaderDeferPath);
  }

  My::MyGE::RsrcMngrDX12::Instance().RegisterShader(screenShader);
  My::MyGE::RsrcMngrDX12::Instance().RegisterShader(geomrtryShader);
  My::MyGE::RsrcMngrDX12::Instance().RegisterShader(deferShader);

  mInputLayout = {
      {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
      {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,
       D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
  };
}

void DeferApp::BuildShapeGeometry() {
  mesh = My::MyGE::AssetMngr::Instance().LoadAsset<My::MyGE::Mesh>(
      "../assets/models/cube.obj");
  My::MyGE::RsrcMngrDX12::Instance().RegisterMesh(
      My::MyGE::RsrcMngrDX12::Instance().GetUpload(),
      My::MyGE::RsrcMngrDX12::Instance().GetDeleteBatch(), myGCmdList.raw.Get(),
      mesh);
}

void DeferApp::BuildPSOs() {
  auto screenPsoDesc = My::MyDX12::Desc::PSO::Basic(
      My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
          ID_RootSignature_screen),
      nullptr, 0,
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_vs(screenShader),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_ps(screenShader),
      mBackBufferFormat, DXGI_FORMAT_UNKNOWN);
  screenPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  ID_PSO_screen =
      My::MyGE::RsrcMngrDX12::Instance().RegisterPSO(&screenPsoDesc);

  auto geometryPsoDesc = My::MyDX12::Desc::PSO::MRT(
      My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
          ID_RootSignature_geometry),
      mInputLayout.data(), (UINT)mInputLayout.size(),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_vs(geomrtryShader),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_ps(geomrtryShader),
      3, DXGI_FORMAT_R32G32B32A32_FLOAT, mDepthStencilFormat);
  geometryPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  ID_PSO_geometry =
      My::MyGE::RsrcMngrDX12::Instance().RegisterPSO(&geometryPsoDesc);

  auto deferLightingPsoDesc = My::MyDX12::Desc::PSO::Basic(
      My::MyGE::RsrcMngrDX12::Instance().GetRootSignature(
          ID_RootSignature_defer_light),
      nullptr, 0,
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_vs(deferShader),
      My::MyGE::RsrcMngrDX12::Instance().GetShaderByteCode_ps(deferShader),
      mBackBufferFormat, DXGI_FORMAT_UNKNOWN);
  deferLightingPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  ID_PSO_defer_light =
      My::MyGE::RsrcMngrDX12::Instance().RegisterPSO(&deferLightingPsoDesc);
}

void DeferApp::BuildFrameResources() {
  for (const auto& fr : frameRsrcMngr->GetFrameResources()) {
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    ThrowIfFailed(myDevice->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));

    fr->RegisterResource("CommandAllocator", allocator);

    fr->RegisterResource("camera constants",
                         My::MyDX12::ArrayUploadBuffer<CameraConstants>{
                             myDevice.raw.Get(), 1, true});

    fr->RegisterResource(
        "ArrayUploadBuffer<GeometryMaterialConstants>",
        My::MyDX12::ArrayUploadBuffer<GeometryMaterialConstants>{
            myDevice.raw.Get(), mMaterials.size(), true});

    fr->RegisterResource("ArrayUploadBuffer<GeometryObjectConstants>",
                         My::MyDX12::ArrayUploadBuffer<GeometryObjectConstants>{
                             myDevice.raw.Get(), mAllRitems.size(), true});

    fr->RegisterResource("lights",
                         My::MyDX12::ArrayUploadBuffer<LightingLights>{
                             myDevice.raw.Get(), 1, true});

    auto fgRsrcMngr = std::make_shared<My::MyDX12::FG::RsrcMngr>();
    fr->RegisterResource("FrameGraphRsrcMngr", fgRsrcMngr);
  }
}

void DeferApp::BuildMaterials() {
  auto woodCrate = std::make_unique<Material>();
  woodCrate->Name = "iron";
  woodCrate->MatCBIndex = 0;
  woodCrate->AlbedoSrvGpuHandle =
      My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(albedoTex2D);
  woodCrate->RoughnessSrvGpuHandle =
      My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
          roughnessTex2D);
  woodCrate->MetalnessSrvGpuHandle =
      My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
          metalnessTex2D);
  woodCrate->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
  woodCrate->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
  woodCrate->Roughness = 0.2f;

  mMaterials["iron"] = std::move(woodCrate);
}

void DeferApp::BuildRenderItems() {
  auto boxRitem = std::make_unique<RenderItem>();
  boxRitem->ObjCBIndex = 0;
  boxRitem->Mat = mMaterials["iron"].get();
  boxRitem->Geo = &My::MyGE::RsrcMngrDX12::Instance().GetMeshGPUBuffer(mesh);
  boxRitem->PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
  boxRitem->IndexCount =
      boxRitem->Geo->IndexBufferView().SizeInBytes /
      (boxRitem->Geo->IndexBufferView().Format == DXGI_FORMAT_R16_UINT ? 2 : 4);
  boxRitem->StartIndexLocation = 0;
  boxRitem->BaseVertexLocation = 0;
  mAllRitems.push_back(std::move(boxRitem));

  // All the render items are opaque.
  for (auto& e : mAllRitems)
    mOpaqueRitems.push_back(e.get());
}

void DeferApp::DrawRenderItems(ID3D12GraphicsCommandList* cmdList,
                               const std::vector<RenderItem*>& ritems) {
  UINT objCBByteSize = My::MyDX12::Util::CalcConstantBufferByteSize(
      sizeof(GeometryObjectConstants));
  UINT matCBByteSize =
      My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(MaterialConstants));

  auto objectCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<My::MyDX12::ArrayUploadBuffer<GeometryObjectConstants>>(
              "ArrayUploadBuffer<GeometryObjectConstants>")
          .GetResource();
  auto matCB =
      frameRsrcMngr->GetCurrentFrameResource()
          ->GetResource<
              My::MyDX12::ArrayUploadBuffer<GeometryMaterialConstants>>(
              "ArrayUploadBuffer<GeometryMaterialConstants>")
          .GetResource();

  // For each render item...
  for (size_t i = 0; i < ritems.size(); ++i) {
    auto ri = ritems[i];

    cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
    cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
    cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

    D3D12_GPU_VIRTUAL_ADDRESS objCBAddress =
        objectCB->GetGPUVirtualAddress() + ri->ObjCBIndex * objCBByteSize;
    D3D12_GPU_VIRTUAL_ADDRESS matCBAddress =
        matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

    cmdList->SetGraphicsRootDescriptorTable(0, ri->Mat->AlbedoSrvGpuHandle);
    cmdList->SetGraphicsRootDescriptorTable(1, ri->Mat->RoughnessSrvGpuHandle);
    cmdList->SetGraphicsRootDescriptorTable(2, ri->Mat->MetalnessSrvGpuHandle);
    cmdList->SetGraphicsRootConstantBufferView(3, objCBAddress);
    cmdList->SetGraphicsRootConstantBufferView(4, matCBAddress);

    cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation,
                                  ri->BaseVertexLocation, 0);
  }
}
