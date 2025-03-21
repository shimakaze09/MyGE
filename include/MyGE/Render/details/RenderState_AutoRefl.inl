// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGE::CullMode>
    : My::MySRefl::TypeInfoBase<My::MyGE::CullMode> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"NONE", My::MyGE::CullMode::NONE},
      Field{"FRONT", My::MyGE::CullMode::FRONT},
      Field{"BACK", My::MyGE::CullMode::BACK},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::CompareFunc>
    : My::MySRefl::TypeInfoBase<My::MyGE::CompareFunc> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"NEVER", My::MyGE::CompareFunc::NEVER},
      Field{"LESS", My::MyGE::CompareFunc::LESS},
      Field{"EQUAL", My::MyGE::CompareFunc::EQUAL},
      Field{"LESS_EQUAL", My::MyGE::CompareFunc::LESS_EQUAL},
      Field{"GREATER", My::MyGE::CompareFunc::GREATER},
      Field{"NOT_EQUAL", My::MyGE::CompareFunc::NOT_EQUAL},
      Field{"GREATER_EQUAL", My::MyGE::CompareFunc::GREATER_EQUAL},
      Field{"ALWAYS", My::MyGE::CompareFunc::ALWAYS},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::Blend>
    : My::MySRefl::TypeInfoBase<My::MyGE::Blend> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"ZERO", My::MyGE::Blend::ZERO},
      Field{"ONE", My::MyGE::Blend::ONE},
      Field{"SRC_COLOR", My::MyGE::Blend::SRC_COLOR},
      Field{"INV_SRC_COLOR", My::MyGE::Blend::INV_SRC_COLOR},
      Field{"SRC_ALPHA", My::MyGE::Blend::SRC_ALPHA},
      Field{"INV_SRC_ALPHA", My::MyGE::Blend::INV_SRC_ALPHA},
      Field{"DEST_ALPHA", My::MyGE::Blend::DEST_ALPHA},
      Field{"INV_DEST_ALPHA", My::MyGE::Blend::INV_DEST_ALPHA},
      Field{"DEST_COLOR", My::MyGE::Blend::DEST_COLOR},
      Field{"INV_DEST_COLOR", My::MyGE::Blend::INV_DEST_COLOR},
      Field{"SRC_ALPHA_SAT", My::MyGE::Blend::SRC_ALPHA_SAT},
      Field{"BLEND_FACTOR", My::MyGE::Blend::BLEND_FACTOR},
      Field{"INV_BLEND_FACTOR", My::MyGE::Blend::INV_BLEND_FACTOR},
      Field{"SRC1_COLOR", My::MyGE::Blend::SRC1_COLOR},
      Field{"INV_SRC1_COLOR", My::MyGE::Blend::INV_SRC1_COLOR},
      Field{"SRC1_ALPHA", My::MyGE::Blend::SRC1_ALPHA},
      Field{"INV_SRC1_ALPHA", My::MyGE::Blend::INV_SRC1_ALPHA},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::BlendOp>
    : My::MySRefl::TypeInfoBase<My::MyGE::BlendOp> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"ADD", My::MyGE::BlendOp::ADD},
      Field{"SUBTRACT", My::MyGE::BlendOp::SUBTRACT},
      Field{"REV_SUBTRACT", My::MyGE::BlendOp::REV_SUBTRACT},
      Field{"MIN", My::MyGE::BlendOp::MIN},
      Field{"MAX", My::MyGE::BlendOp::MAX},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::BlendState>
    : My::MySRefl::TypeInfoBase<My::MyGE::BlendState> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"enable", &My::MyGE::BlendState::enable},
      Field{"src", &My::MyGE::BlendState::src},
      Field{"dest", &My::MyGE::BlendState::dest},
      Field{"op", &My::MyGE::BlendState::op},
      Field{"srcAlpha", &My::MyGE::BlendState::srcAlpha},
      Field{"destAlpha", &My::MyGE::BlendState::destAlpha},
      Field{"opAlpha", &My::MyGE::BlendState::opAlpha},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::StencilOp>
    : My::MySRefl::TypeInfoBase<My::MyGE::StencilOp> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"KEEP", My::MyGE::StencilOp::KEEP},
      Field{"ZERO", My::MyGE::StencilOp::ZERO},
      Field{"REPLACE", My::MyGE::StencilOp::REPLACE},
      Field{"INCR_SAT", My::MyGE::StencilOp::INCR_SAT},
      Field{"DECR_SAT", My::MyGE::StencilOp::DECR_SAT},
      Field{"INVERT", My::MyGE::StencilOp::INVERT},
      Field{"INCR", My::MyGE::StencilOp::INCR},
      Field{"DECR", My::MyGE::StencilOp::DECR},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::StencilState>
    : My::MySRefl::TypeInfoBase<My::MyGE::StencilState> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"enable", &My::MyGE::StencilState::enable},
      Field{"ref", &My::MyGE::StencilState::ref},
      Field{"readMask", &My::MyGE::StencilState::readMask},
      Field{"writeMask", &My::MyGE::StencilState::writeMask},
      Field{"failOp", &My::MyGE::StencilState::failOp},
      Field{"depthFailOp", &My::MyGE::StencilState::depthFailOp},
      Field{"passOp", &My::MyGE::StencilState::passOp},
      Field{"func", &My::MyGE::StencilState::func},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::RenderState>
    : My::MySRefl::TypeInfoBase<My::MyGE::RenderState> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"cullMode", &My::MyGE::RenderState::cullMode},
      Field{"zTest", &My::MyGE::RenderState::zTest},
      Field{"zWrite", &My::MyGE::RenderState::zWrite},
      Field{"stencilState", &My::MyGE::RenderState::stencilState},
      Field{"blendState", &My::MyGE::RenderState::blendState},
      Field{"colorMask", &My::MyGE::RenderState::colorMask},
  };
};
