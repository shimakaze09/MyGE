//
// Created by Admin on 16/03/2025.
//

#include <MyDX12/FrameResourceMngr.h>
#include <MyGE/Asset/AssetMngr.h>
#include <MyGE/Core/Components/Camera.h>
#include <MyGE/Core/Components/MeshFilter.h>
#include <MyGE/Core/Components/MeshRenderer.h>
#include <MyGE/Core/GameTimer.h>
#include <MyGE/Core/HLSLFile.h>
#include <MyGE/Core/Image.h>
#include <MyGE/Core/Mesh.h>
#include <MyGE/Core/Shader.h>
#include <MyGE/Core/ShaderMngr.h>
#include <MyGE/Core/Systems/CameraSystem.h>
#include <MyGE/Core/Texture2D.h>
#include <MyGE/Render/DX12/MeshLayoutMngr.h>
#include <MyGE/Render/DX12/RsrcMngrDX12.h>
#include <MyGE/Render/DX12/ShaderCBMngrDX12.h>
#include <MyGE/Render/DX12/StdPipeline.h>
#include <MyGE/Transform/Transform.h>
#include <MyGE/_deps/imgui/imgui.h>
#include <MyGE/_deps/imgui/imgui_impl_dx12.h>
#include <MyGE/_deps/imgui/imgui_impl_win32.h>

using namespace My::MyGE;
using namespace My;

struct StdPipeline::Impl {
  Impl(InitDesc initDesc)
      : initDesc{initDesc},
        frameRsrcMngr{initDesc.numFrame, initDesc.device},
        fg{"Standard Pipeline"} {
    BuildFrameResources();
    BuildShadersAndInputLayout();
    BuildRootSignature();
    BuildPSOs();
  }

  size_t ID_PSO_defer_light;
  size_t ID_PSO_screen;

  static constexpr size_t ID_RootSignature_geometry = 0;
  static constexpr size_t ID_RootSignature_screen = 1;
  static constexpr size_t ID_RootSignature_defer_light = 2;

  struct ObjectConstants {
    My::transformf World = My::transformf::eye();
    My::transformf TexTransform = My::transformf::eye();
  };

  struct PassConstants {
    My::transformf View = My::transformf::eye();
    My::transformf InvView = My::transformf::eye();
    My::transformf Proj = My::transformf::eye();
    My::transformf InvProj = My::transformf::eye();
    My::transformf ViewProj = My::transformf::eye();
    My::transformf InvViewProj = My::transformf::eye();
    My::pointf3 EyePosW = {0.0f, 0.0f, 0.0f};
    float cbPerObjectPad1 = 0.0f;
    My::valf2 RenderTargetSize = {0.0f, 0.0f};
    My::valf2 InvRenderTargetSize = {0.0f, 0.0f};
    float NearZ = 0.0f;
    float FarZ = 0.0f;
    float TotalTime = 0.0f;
    float DeltaTime = 0.0f;

    My::vecf4 AmbientLight = {0.0f, 0.0f, 0.0f, 1.0f};

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point
    // lights; indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS,
    // NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS) are spot lights for a
    // maximum of MaxLights per object.
    struct Light {
      My::rgbf Strength = {0.5f, 0.5f, 0.5f};
      float FalloffStart = 1.0f;                  // point/spot light only
      My::vecf3 Direction = {0.0f, -1.0f, 0.0f};  // directional/spot light only
      float FalloffEnd = 10.0f;                   // point/spot light only
      My::pointf3 Position = {0.0f, 0.0f, 0.0f};  // point/spot light only
      float SpotPower = 64.0f;                    // spot light only
    };

    Light Lights[16];
  };

  struct MatConstants {
    My::rgbf albedoFactor;
    float roughnessFactor;
  };

  struct RenderContext {
    size_t numCameras;

    struct Object {
      const Mesh* mesh{nullptr};
      size_t submeshIdx{static_cast<size_t>(-1)};

      valf<16> l2w;
    };

    std::unordered_map<const Shader*,
                       std::unordered_map<const Material*, std::vector<Object>>>
        objectMap;
  };

  const InitDesc initDesc;

  RenderContext renderContext;

  MyDX12::FrameResourceMngr frameRsrcMngr;

  My::MyDX12::FG::Executor fgExecutor;
  My::MyFG::Compiler fgCompiler;
  My::MyFG::FrameGraph fg;

  My::MyGE::Shader* screenShader;
  My::MyGE::Shader* geomrtryShader;
  My::MyGE::Shader* deferShader;

  std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

  const DXGI_FORMAT dsFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

  void BuildFrameResources();
  void BuildShadersAndInputLayout();
  void BuildRootSignature();
  void BuildPSOs();

  size_t GetGeometryPSO_ID(const Mesh* mesh);
  std::unordered_map<size_t, size_t> PSOIDMap;

  void UpdateRenderContext(const MyECS::World& world);
  void UpdateShaderCBs(const ResizeData& resizeData, const MyECS::World& world,
                       const std::vector<CameraData>& camera);
  void Render(const ResizeData& resizeData, ID3D12Resource* rtb);
  void DrawObjects(ID3D12GraphicsCommandList*);
};

void StdPipeline::Impl::BuildFrameResources() {
  for (const auto& fr : frameRsrcMngr.GetFrameResources()) {
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> allocator;
    ThrowIfFailed(initDesc.device->CreateCommandAllocator(
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator)));

    fr->RegisterResource("CommandAllocator", allocator);

    fr->RegisterResource("ShaderCBMngrDX12", ShaderCBMngrDX12{initDesc.device});

    auto fgRsrcMngr = std::make_shared<My::MyDX12::FG::RsrcMngr>();
    fr->RegisterResource("FrameGraphRsrcMngr", fgRsrcMngr);
  }
}

void StdPipeline::Impl::BuildShadersAndInputLayout() {
  screenShader = ShaderMngr::Instance().Get("StdPipeline/Screen");
  geomrtryShader = ShaderMngr::Instance().Get("StdPipeline/Geometry");
  deferShader = ShaderMngr::Instance().Get("StdPipeline/Defer Lighting");
}

void StdPipeline::Impl::BuildRootSignature() {
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
    slotRootParameter[3].InitAsConstantBufferView(0);
    slotRootParameter[4].InitAsConstantBufferView(1);
    slotRootParameter[5].InitAsConstantBufferView(2);

    auto staticSamplers = RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        6, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    RsrcMngrDX12::Instance().RegisterRootSignature(ID_RootSignature_geometry,
                                                   &rootSigDesc);
  }

  {  // screen
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable,
                                               D3D12_SHADER_VISIBILITY_PIXEL);

    auto staticSamplers = RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        1, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    RsrcMngrDX12::Instance().RegisterRootSignature(ID_RootSignature_screen,
                                                   &rootSigDesc);
  }
  {  // defer lighting
    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[4];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable,
                                               D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsConstantBufferView(0);
    slotRootParameter[2].InitAsConstantBufferView(1);
    slotRootParameter[3].InitAsConstantBufferView(2);

    auto staticSamplers = RsrcMngrDX12::Instance().GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(
        4, slotRootParameter, (UINT)staticSamplers.size(),
        staticSamplers.data(),
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    RsrcMngrDX12::Instance().RegisterRootSignature(ID_RootSignature_defer_light,
                                                   &rootSigDesc);
  }
}

void StdPipeline::Impl::BuildPSOs() {
  auto screenPsoDesc = My::MyDX12::Desc::PSO::Basic(
      RsrcMngrDX12::Instance().GetRootSignature(ID_RootSignature_screen),
      nullptr, 0, RsrcMngrDX12::Instance().GetShaderByteCode_vs(screenShader),
      RsrcMngrDX12::Instance().GetShaderByteCode_ps(screenShader),
      initDesc.rtFormat, DXGI_FORMAT_UNKNOWN);
  screenPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  screenPsoDesc.DepthStencilState.DepthEnable = false;
  screenPsoDesc.DepthStencilState.StencilEnable = false;
  ID_PSO_screen = RsrcMngrDX12::Instance().RegisterPSO(&screenPsoDesc);

  /*auto geometryPsoDesc = My::MyDX12::Desc::PSO::MRT(
                RsrcMngrDX12::Instance().GetRootSignature(ID_RootSignature_geometry),
                mInputLayout.data(), (UINT)mInputLayout.size(),
                RsrcMngrDX12::Instance().GetShaderByteCode_vs(geomrtryShader),
                RsrcMngrDX12::Instance().GetShaderByteCode_ps(geomrtryShader),
                3,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                initDesc.depthStencilFormat
        );
        geometryPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
        ID_PSO_geometry =
     RsrcMngrDX12::Instance().RegisterPSO(&geometryPsoDesc);*/

  auto deferLightingPsoDesc = My::MyDX12::Desc::PSO::Basic(
      RsrcMngrDX12::Instance().GetRootSignature(ID_RootSignature_defer_light),
      nullptr, 0, RsrcMngrDX12::Instance().GetShaderByteCode_vs(deferShader),
      RsrcMngrDX12::Instance().GetShaderByteCode_ps(deferShader),
      initDesc.rtFormat, DXGI_FORMAT_UNKNOWN);
  deferLightingPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
  deferLightingPsoDesc.DepthStencilState.DepthEnable = false;
  deferLightingPsoDesc.DepthStencilState.StencilEnable = false;
  ID_PSO_defer_light =
      RsrcMngrDX12::Instance().RegisterPSO(&deferLightingPsoDesc);
}

size_t StdPipeline::Impl::GetGeometryPSO_ID(const Mesh* mesh) {
  size_t layoutID = MeshLayoutMngr::Instance().GetMeshLayoutID(mesh);
  auto target = PSOIDMap.find(layoutID);
  if (target == PSOIDMap.end()) {
    auto [uv, normal, tangent, color] =
        MeshLayoutMngr::Instance().DecodeMeshLayoutID(layoutID);
    if (!uv || !normal) return static_cast<size_t>(-1);  // not support

    const auto& layout =
        MeshLayoutMngr::Instance().GetMeshLayoutValue(layoutID);
    auto geometryPsoDesc = My::MyDX12::Desc::PSO::MRT(
        RsrcMngrDX12::Instance().GetRootSignature(ID_RootSignature_geometry),
        layout.data(), (UINT)layout.size(),
        RsrcMngrDX12::Instance().GetShaderByteCode_vs(geomrtryShader),
        RsrcMngrDX12::Instance().GetShaderByteCode_ps(geomrtryShader), 3,
        DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_D24_UNORM_S8_UINT);
    geometryPsoDesc.RasterizerState.FrontCounterClockwise = TRUE;
    size_t ID_PSO_geometry =
        RsrcMngrDX12::Instance().RegisterPSO(&geometryPsoDesc);
    target =
        PSOIDMap.emplace_hint(target, std::pair{layoutID, ID_PSO_geometry});
  }
  return target->second;
}

void StdPipeline::Impl::UpdateRenderContext(const MyECS::World& world) {
  renderContext.objectMap.clear();

  My::MyECS::ArchetypeFilter objectFilter;
  objectFilter.all = {My::MyECS::CmptAccessType::Of<MeshFilter>,
                      My::MyECS::CmptAccessType::Of<MeshRenderer>};

  MyECS::ArchetypeFilter filter;
  filter.all = {MyECS::CmptAccessType::Of<MyECS::Latest<MeshFilter>>,
                MyECS::CmptAccessType::Of<MyECS::Latest<MeshRenderer>>};

  const_cast<MyECS::World&>(world).RunChunkJob(
      [&](MyECS::ChunkView chunk) {
        auto meshFilterArr = chunk.GetCmptArray<MeshFilter>();
        auto meshRendererArr = chunk.GetCmptArray<MeshRenderer>();
        auto l2wArr = chunk.GetCmptArray<LocalToWorld>();
        size_t num = chunk.EntityNum();
        for (size_t i = 0; i < num; i++) {
          RenderContext::Object obj;
          obj.mesh = meshFilterArr[i].mesh;
          obj.l2w = l2wArr ? l2wArr[i].value.as<valf<16>>()
                           : transformf::eye().as<valf<16>>();
          for (size_t j = 0; j < std::min(meshRendererArr[i].materials.size(),
                                          obj.mesh->GetSubMeshes().size());
               j++) {
            auto material = meshRendererArr[i].materials[j];
            obj.submeshIdx = j;
            renderContext.objectMap[material->shader][material].push_back(obj);
          }
        }
      },
      filter, false);
}

void StdPipeline::Impl::UpdateShaderCBs(
    const ResizeData& resizeData, const MyECS::World& world,
    const std::vector<CameraData>& cameras) {
  auto& shaderCBMngr =
      frameRsrcMngr.GetCurrentFrameResource()
          ->GetResource<My::MyGE::ShaderCBMngrDX12>("ShaderCBMngrDX12");

  renderContext.numCameras = cameras.size();

  for (const auto& [shader, mat2objects] : renderContext.objectMap) {
    size_t objectNum = 0;
    for (const auto& [mat, objects] : mat2objects) objectNum += objects.size();
    if (shader->shaderName == "StdPipeline/Geometry") {
      auto buffer = shaderCBMngr.GetBuffer(shader);
      buffer->Reserve(
          cameras.size() * My::MyDX12::Util::CalcConstantBufferByteSize(
                               sizeof(PassConstants)) +
          mat2objects.size() * My::MyDX12::Util::CalcConstantBufferByteSize(
                                   sizeof(MatConstants)) +
          objectNum * My::MyDX12::Util::CalcConstantBufferByteSize(
                          sizeof(ObjectConstants)));
      size_t offset = 0;
      for (size_t i = 0; i < cameras.size(); i++) {
        const auto& camData = cameras[i];
        auto cmptCamera = camData.world.entityMngr.Get<Camera>(camData.entity);
        auto cmptW2L =
            camData.world.entityMngr.Get<WorldToLocal>(camData.entity);
        auto cmptTranslation =
            camData.world.entityMngr.Get<Translation>(camData.entity);
        PassConstants passCB;
        passCB.View = cmptW2L->value;
        passCB.InvView = passCB.View.inverse();
        passCB.Proj = cmptCamera->prjectionMatrix;
        passCB.InvProj = passCB.Proj.inverse();
        passCB.ViewProj = passCB.Proj * passCB.View;
        passCB.InvViewProj = passCB.InvView * passCB.InvProj;
        passCB.EyePosW = cmptTranslation->value.as<pointf3>();
        passCB.RenderTargetSize = {resizeData.width, resizeData.height};
        passCB.InvRenderTargetSize = {1.0f / resizeData.width,
                                      1.0f / resizeData.height};

        passCB.NearZ = cmptCamera->clippingPlaneMin;
        passCB.FarZ = cmptCamera->clippingPlaneMax;
        passCB.TotalTime = My::MyGE::GameTimer::Instance().TotalTime();
        passCB.DeltaTime = My::MyGE::GameTimer::Instance().DeltaTime();
        passCB.AmbientLight = {0.25f, 0.25f, 0.35f, 1.0f};
        passCB.Lights[0].Direction = {0.57735f, -0.57735f, 0.57735f};
        passCB.Lights[0].Strength = {0.6f, 0.6f, 0.6f};
        passCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
        passCB.Lights[1].Strength = {0.3f, 0.3f, 0.3f};
        passCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
        passCB.Lights[2].Strength = {0.15f, 0.15f, 0.15f};
        buffer->Set(0, &passCB, sizeof(PassConstants));
        offset +=
            My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(PassConstants));
      }
      for (const auto& [mat, objects] : mat2objects) {
        MatConstants matC;
        matC.albedoFactor = {1.f};
        matC.roughnessFactor = 1.f;
        buffer->Set(offset, &matC, sizeof(MatConstants));
        offset +=
            My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(MatConstants));
        for (const auto& object : objects) {
          ObjectConstants objectConstants;
          objectConstants.TexTransform = My::transformf::eye();
          objectConstants.World = object.l2w;
          buffer->Set(offset, &objectConstants, sizeof(ObjectConstants));
          offset += My::MyDX12::Util::CalcConstantBufferByteSize(
              sizeof(ObjectConstants));
        }
      }
    }
  }
}

void StdPipeline::Impl::Render(const ResizeData& resizeData,
                               ID3D12Resource* rtb) {
  size_t width = resizeData.width;
  size_t height = resizeData.height;

  auto cmdAlloc =
      frameRsrcMngr.GetCurrentFrameResource()
          ->GetResource<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>>(
              "CommandAllocator");
  cmdAlloc->Reset();

  fg.Clear();
  auto fgRsrcMngr =
      frameRsrcMngr.GetCurrentFrameResource()
          ->GetResource<std::shared_ptr<My::MyDX12::FG::RsrcMngr>>(
              "FrameGraphRsrcMngr");
  fgRsrcMngr->NewFrame();
  fgExecutor.NewFrame();
  ;

  auto gbuffer0 = fg.RegisterResourceNode("GBuffer0");
  auto gbuffer1 = fg.RegisterResourceNode("GBuffer1");
  auto gbuffer2 = fg.RegisterResourceNode("GBuffer2");
  auto rt = fg.RegisterResourceNode("Render Target");
  auto depthstencil = fg.RegisterResourceNode("Depth Stencil");
  auto gbPass = fg.RegisterPassNode(
      "GBuffer Pass", {}, {gbuffer0, gbuffer1, gbuffer2, depthstencil});
  auto deferLightingPass = fg.RegisterPassNode(
      "Defer Lighting", {gbuffer0, gbuffer1, gbuffer2}, {rt});

  D3D12_RESOURCE_DESC dsDesc = MyDX12::Desc::RSRC::Basic(
      D3D12_RESOURCE_DIMENSION_TEXTURE2D, width, (UINT)height,
      DXGI_FORMAT_R24G8_TYPELESS,
      // Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer
      // to read from the depth buffer.  Therefore, because we need to create
      // two views to the same resource:
      //   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
      //   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
      // we need to create the depth buffer resource with a typeless format.
      D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

  D3D12_CLEAR_VALUE dsClear;
  dsClear.Format = dsFormat;
  dsClear.DepthStencil.Depth = 1.0f;
  dsClear.DepthStencil.Stencil = 0;

  (*fgRsrcMngr)
      .RegisterTemporalRsrc(gbuffer0, My::MyDX12::FG::RsrcType::RT2D(
                                          DXGI_FORMAT_R32G32B32A32_FLOAT, width,
                                          height, DirectX::Colors::Black))
      .RegisterTemporalRsrc(gbuffer1, My::MyDX12::FG::RsrcType::RT2D(
                                          DXGI_FORMAT_R32G32B32A32_FLOAT, width,
                                          height, DirectX::Colors::Black))
      .RegisterTemporalRsrc(gbuffer2, My::MyDX12::FG::RsrcType::RT2D(
                                          DXGI_FORMAT_R32G32B32A32_FLOAT, width,
                                          height, DirectX::Colors::Black))
      .RegisterTemporalRsrc(depthstencil, {dsClear, dsDesc})

      .RegisterRsrcTable(
          {{gbuffer0,
            My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT)},
           {gbuffer1,
            My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT)},
           {gbuffer2,
            My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT)}})

      .RegisterImportedRsrc(rt, {rtb, D3D12_RESOURCE_STATE_PRESENT})

      .RegisterPassRsrcs(gbPass, gbuffer0, D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrcs(gbPass, gbuffer1, D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrcs(gbPass, gbuffer2, D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{})
      .RegisterPassRsrcs(gbPass, depthstencil, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                         My::MyDX12::Desc::DSV::Basic(dsFormat))

      .RegisterPassRsrcs(
          deferLightingPass, gbuffer0,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))
      .RegisterPassRsrcs(
          deferLightingPass, gbuffer1,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))
      .RegisterPassRsrcs(
          deferLightingPass, gbuffer2,
          D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
          My::MyDX12::Desc::SRV::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT))

      .RegisterPassRsrcs(deferLightingPass, rt,
                         D3D12_RESOURCE_STATE_RENDER_TARGET,
                         My::MyDX12::FG::RsrcImplDesc_RTV_Null{});

  fgExecutor.RegisterPassFunc(
      gbPass, [&](ID3D12GraphicsCommandList* cmdList,
                  const My::MyDX12::FG::PassRsrcs& rsrcs) {
        auto heap = My::MyDX12::DescriptorHeapMngr::Instance()
                        .GetCSUGpuDH()
                        ->GetDescriptorHeap();
        cmdList->SetDescriptorHeaps(1, &heap);
        cmdList->RSSetViewports(1, &resizeData.screenViewport);
        cmdList->RSSetScissorRects(1, &resizeData.scissorRect);

        auto gb0 = rsrcs.find(gbuffer0)->second;
        auto gb1 = rsrcs.find(gbuffer1)->second;
        auto gb2 = rsrcs.find(gbuffer2)->second;
        auto ds = rsrcs.find(depthstencil)->second;

        // Clear the render texture and depth buffer.
        cmdList->ClearRenderTargetView(gb0.cpuHandle, DirectX::Colors::Black, 0,
                                       nullptr);
        cmdList->ClearRenderTargetView(gb1.cpuHandle, DirectX::Colors::Black, 0,
                                       nullptr);
        cmdList->ClearRenderTargetView(gb2.cpuHandle, DirectX::Colors::Black, 0,
                                       nullptr);
        cmdList->ClearDepthStencilView(
            ds.cpuHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
            1.f, 0, 0, nullptr);

        // Specify the buffers we are going to render to.
        std::array rts{gb0.cpuHandle, gb1.cpuHandle, gb2.cpuHandle};
        cmdList->OMSetRenderTargets(rts.size(), rts.data(), false,
                                    &ds.cpuHandle);

        cmdList->SetGraphicsRootSignature(
            RsrcMngrDX12::Instance().GetRootSignature(
                Impl::ID_RootSignature_geometry));

        auto passCB = frameRsrcMngr.GetCurrentFrameResource()
                          ->GetResource<ShaderCBMngrDX12>("ShaderCBMngrDX12")
                          .GetBuffer(geomrtryShader)
                          ->GetResource();

        constexpr UINT passCBByteSize =
            My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(PassConstants));
        size_t offset = passCBByteSize * 0;  // not support multi camera now

        cmdList->SetGraphicsRootConstantBufferView(
            4, passCB->GetGPUVirtualAddress() + offset);

        DrawObjects(cmdList);
      });

  fgExecutor.RegisterPassFunc(
      deferLightingPass, [&](ID3D12GraphicsCommandList* cmdList,
                             const My::MyDX12::FG::PassRsrcs& rsrcs) {
        auto heap = My::MyDX12::DescriptorHeapMngr::Instance()
                        .GetCSUGpuDH()
                        ->GetDescriptorHeap();
        cmdList->SetDescriptorHeaps(1, &heap);
        cmdList->RSSetViewports(1, &resizeData.screenViewport);
        cmdList->RSSetScissorRects(1, &resizeData.scissorRect);

        cmdList->SetPipelineState(
            RsrcMngrDX12::Instance().GetPSO(ID_PSO_defer_light));
        auto gb0 = rsrcs.find(gbuffer0)->second;
        auto gb1 = rsrcs.find(gbuffer1)->second;
        auto gb2 = rsrcs.find(gbuffer2)->second;

        auto bb = rsrcs.find(rt)->second;

        // cmdList->CopyResource(bb.resource, rt.resource);

        // Clear the render texture and depth buffer.
        cmdList->ClearRenderTargetView(bb.cpuHandle, DirectX::Colors::Black, 0,
                                       nullptr);

        // Specify the buffers we are going to render to.
        cmdList->OMSetRenderTargets(1, &bb.cpuHandle, false, nullptr);

        cmdList->SetGraphicsRootSignature(
            RsrcMngrDX12::Instance().GetRootSignature(
                Impl::ID_RootSignature_defer_light));

        cmdList->SetGraphicsRootDescriptorTable(0, gb0.gpuHandle);

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
  fgExecutor.Execute(initDesc.device, initDesc.cmdQueue, cmdAlloc.Get(), crst,
                     *fgRsrcMngr);
}

void StdPipeline::Impl::DrawObjects(ID3D12GraphicsCommandList* cmdList) {
  constexpr UINT passCBByteSize =
      My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(PassConstants));
  constexpr UINT matCBByteSize =
      My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(MatConstants));
  constexpr UINT objCBByteSize =
      My::MyDX12::Util::CalcConstantBufferByteSize(sizeof(ObjectConstants));

  auto& shaderCBMngr =
      frameRsrcMngr.GetCurrentFrameResource()
          ->GetResource<My::MyGE::ShaderCBMngrDX12>("ShaderCBMngrDX12");

  auto buffer = shaderCBMngr.GetBuffer(geomrtryShader);

  const auto& mat2objects =
      renderContext.objectMap.find(geomrtryShader)->second;

  size_t offset = passCBByteSize * renderContext.numCameras;
  for (const auto& [mat, objects] : mat2objects) {
    // For each render item...
    size_t objIdx = 0;
    for (size_t i = 0; i < objects.size(); i++) {
      auto object = objects[i];
      auto& meshGPUBuffer =
          My::MyGE::RsrcMngrDX12::Instance().GetMeshGPUBuffer(object.mesh);
      const auto& submesh = object.mesh->GetSubMeshes().at(object.submeshIdx);
      cmdList->IASetVertexBuffers(0, 1, &meshGPUBuffer.VertexBufferView());
      cmdList->IASetIndexBuffer(&meshGPUBuffer.IndexBufferView());
      // submesh.topology
      cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

      D3D12_GPU_VIRTUAL_ADDRESS objCBAddress =
          buffer->GetResource()->GetGPUVirtualAddress() + offset +
          matCBByteSize + objIdx * objCBByteSize;
      D3D12_GPU_VIRTUAL_ADDRESS matCBAddress =
          buffer->GetResource()->GetGPUVirtualAddress() + offset;

      auto albedo = mat->texture2Ds.find("gAlbedoMap")->second;
      auto roughness = mat->texture2Ds.find("gRoughnessMap")->second;
      auto metalness = mat->texture2Ds.find("gMetalnessMap")->second;
      auto albedoHandle =
          My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(albedo);
      auto roughnessHandle =
          My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
              roughness);
      auto matalnessHandle =
          My::MyGE::RsrcMngrDX12::Instance().GetTexture2DSrvGpuHandle(
              metalness);
      cmdList->SetGraphicsRootDescriptorTable(0, albedoHandle);
      cmdList->SetGraphicsRootDescriptorTable(1, roughnessHandle);
      cmdList->SetGraphicsRootDescriptorTable(2, matalnessHandle);
      cmdList->SetGraphicsRootConstantBufferView(3, objCBAddress);
      cmdList->SetGraphicsRootConstantBufferView(5, matCBAddress);

      cmdList->SetPipelineState(
          RsrcMngrDX12::Instance().GetPSO(GetGeometryPSO_ID(object.mesh)));
      cmdList->DrawIndexedInstanced(submesh.indexCount, 1, submesh.indexStart,
                                    submesh.baseVertex, 0);
      objIdx++;
    }
    offset += matCBByteSize + objects.size() * objCBByteSize;
  }
}

StdPipeline::StdPipeline(InitDesc initDesc)
    : IPipeline{initDesc}, pImpl{new Impl{initDesc}} {}

StdPipeline::~StdPipeline() { delete pImpl; }

void StdPipeline::BeginFrame(const MyECS::World& world,
                             const std::vector<CameraData>& cameras) {
  // collect some cpu data
  pImpl->UpdateRenderContext(world);

  // Cycle through the circular frame resource array.
  // Has the GPU finished processing the commands of the current frame resource?
  // If not, wait until the GPU has completed commands up to this fence point.
  pImpl->frameRsrcMngr.BeginFrame();

  // cpu -> gpu
  pImpl->UpdateShaderCBs(GetResizeData(), world, cameras);
}

void StdPipeline::Render(ID3D12Resource* rt) {
  pImpl->Render(GetResizeData(), rt);
}

void StdPipeline::EndFrame() {
  pImpl->frameRsrcMngr.EndFrame(initDesc.cmdQueue);
}

void StdPipeline::Impl_Resize() {
  for (auto& frsrc : pImpl->frameRsrcMngr.GetFrameResources()) {
    frsrc->DelayUpdateResource(
        "FrameGraphRsrcMngr",
        [](std::shared_ptr<My::MyDX12::FG::RsrcMngr> rsrcMngr) {
          rsrcMngr->Clear();
        });
  }
}
