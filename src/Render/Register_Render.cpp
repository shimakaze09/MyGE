#include <MyGE/Render/Components/Components.h>
#include <MyGE/Render/Register_Render.h>
#include <MyGE/Render/Systems/Systems.h>

#include <MyDRefl/MyDRefl.hpp>

using namespace My::MyGE;
using namespace My::MyDRefl;

void My::MyGE::MyDRefl_Register_Render() {
  // Camera
  // TODO: attrs
  Mngr.RegisterType<Camera>();
  Mngr.AddField<&Camera::aspect>("aspect");
  Mngr.AddField<&Camera::fov>("fov");
  Mngr.AddField<&Camera::clippingPlaneMin>("clippingPlaneMin");
  Mngr.AddField<&Camera::clippingPlaneMax>("clippingPlaneMax");
  Mngr.AddField<&Camera::prjectionMatrix>("prjectionMatrix");

  // Light
  Mngr.RegisterType<Light::Mode>();
  Mngr.AddField<Light::Mode::Directional>("Directional");
  Mngr.AddField<Light::Mode::Point>("Point");
  Mngr.AddField<Light::Mode::Spot>("Spot");
  Mngr.AddField<Light::Mode::Rect>("Rect");
  Mngr.AddField<Light::Mode::Disk>("Disk");

  Mngr.RegisterType<Light>();
  Mngr.AddField<&Light::mode>("mode");
  Mngr.AddField<&Light::color>("color");
  Mngr.AddField<&Light::intensity>("intensity");
  Mngr.AddField<&Light::range>("range");
  Mngr.AddField<&Light::width>("width");
  Mngr.AddField<&Light::height>("height");
  Mngr.AddField<&Light::innerSpotAngle>("innerSpotAngle");
  Mngr.AddField<&Light::outerSpotAngle>("outerSpotAngle");

  // MeshFilter
  Mngr.RegisterType<MeshFilter>();
  Mngr.AddField<&MeshFilter::mesh>("mode");

  // MeshRenderer
  Mngr.RegisterType<MeshRenderer>();
  Mngr.AddField<&MeshRenderer::materials>("materials");

  // Skybox
  Mngr.RegisterType<Skybox>();
  Mngr.AddField<&Skybox::material>("materials");
}

