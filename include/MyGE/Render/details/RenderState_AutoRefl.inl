// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGE::FillMode>
    : TypeInfoBase<My::MyGE::FillMode> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[23] = "My::MyGE::FillMode";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("WIREFRAME"), Type::WIREFRAME},
      Field{TSTR("SOLID"), Type::SOLID},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::CullMode>
    : TypeInfoBase<My::MyGE::CullMode> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[23] = "My::MyGE::CullMode";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("NONE"), Type::NONE},
      Field{TSTR("FRONT"), Type::FRONT},
      Field{TSTR("BACK"), Type::BACK},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::CompareFunc>
    : TypeInfoBase<My::MyGE::CompareFunc> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[26] = "My::MyGE::CompareFunc";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("NEVER"), Type::NEVER},
      Field{TSTR("LESS"), Type::LESS},
      Field{TSTR("EQUAL"), Type::EQUAL},
      Field{TSTR("LESS_EQUAL"), Type::LESS_EQUAL},
      Field{TSTR("GREATER"), Type::GREATER},
      Field{TSTR("NOT_EQUAL"), Type::NOT_EQUAL},
      Field{TSTR("GREATER_EQUAL"), Type::GREATER_EQUAL},
      Field{TSTR("ALWAYS"), Type::ALWAYS},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::Blend> : TypeInfoBase<My::MyGE::Blend> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[20] = "My::MyGE::Blend";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("ZERO"), Type::ZERO},
      Field{TSTR("ONE"), Type::ONE},
      Field{TSTR("SRC_COLOR"), Type::SRC_COLOR},
      Field{TSTR("INV_SRC_COLOR"), Type::INV_SRC_COLOR},
      Field{TSTR("SRC_ALPHA"), Type::SRC_ALPHA},
      Field{TSTR("INV_SRC_ALPHA"), Type::INV_SRC_ALPHA},
      Field{TSTR("DEST_ALPHA"), Type::DEST_ALPHA},
      Field{TSTR("INV_DEST_ALPHA"), Type::INV_DEST_ALPHA},
      Field{TSTR("DEST_COLOR"), Type::DEST_COLOR},
      Field{TSTR("INV_DEST_COLOR"), Type::INV_DEST_COLOR},
      Field{TSTR("SRC_ALPHA_SAT"), Type::SRC_ALPHA_SAT},
      Field{TSTR("BLEND_FACTOR"), Type::BLEND_FACTOR},
      Field{TSTR("INV_BLEND_FACTOR"), Type::INV_BLEND_FACTOR},
      Field{TSTR("SRC1_COLOR"), Type::SRC1_COLOR},
      Field{TSTR("INV_SRC1_COLOR"), Type::INV_SRC1_COLOR},
      Field{TSTR("SRC1_ALPHA"), Type::SRC1_ALPHA},
      Field{TSTR("INV_SRC1_ALPHA"), Type::INV_SRC1_ALPHA},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::BlendOp>
    : TypeInfoBase<My::MyGE::BlendOp> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[22] = "My::MyGE::BlendOp";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("ADD"), Type::ADD},
      Field{TSTR("SUBTRACT"), Type::SUBTRACT},
      Field{TSTR("REV_SUBTRACT"), Type::REV_SUBTRACT},
      Field{TSTR("MIN"), Type::MIN},
      Field{TSTR("MAX"), Type::MAX},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::BlendState>
    : TypeInfoBase<My::MyGE::BlendState> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[25] = "My::MyGE::BlendState";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("enable"), &Type::enable,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> bool {
                       return {false};
                     }},
            }},
      Field{TSTR("src"), &Type::src,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::Blend {
                       return {MyGE::Blend::SRC_ALPHA};
                     }},
            }},
      Field{TSTR("dest"), &Type::dest,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::Blend {
                       return {MyGE::Blend::INV_SRC_ALPHA};
                     }},
            }},
      Field{TSTR("op"), &Type::op,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::BlendOp {
                       return {MyGE::BlendOp::ADD};
                     }},
            }},
      Field{TSTR("srcAlpha"), &Type::srcAlpha,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::Blend {
                       return {MyGE::Blend::ONE};
                     }},
            }},
      Field{TSTR("destAlpha"), &Type::destAlpha,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::Blend {
                       return {MyGE::Blend::INV_SRC_ALPHA};
                     }},
            }},
      Field{TSTR("opAlpha"), &Type::opAlpha,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::BlendOp {
                       return {MyGE::BlendOp::ADD};
                     }},
            }},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::StencilOp>
    : TypeInfoBase<My::MyGE::StencilOp> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[24] = "My::MyGE::StencilOp";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("KEEP"), Type::KEEP},
      Field{TSTR("ZERO"), Type::ZERO},
      Field{TSTR("REPLACE"), Type::REPLACE},
      Field{TSTR("INCR_SAT"), Type::INCR_SAT},
      Field{TSTR("DECR_SAT"), Type::DECR_SAT},
      Field{TSTR("INVERT"), Type::INVERT},
      Field{TSTR("INCR"), Type::INCR},
      Field{TSTR("DECR"), Type::DECR},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::StencilState>
    : TypeInfoBase<My::MyGE::StencilState> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[27] = "My::MyGE::StencilState";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("enable"), &Type::enable,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> bool {
                       return {false};
                     }},
            }},
      Field{TSTR("ref"), &Type::ref,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> uint8_t {
                       return {0};
                     }},
            }},
      Field{TSTR("readMask"), &Type::readMask,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> uint8_t {
                       return {0xff};
                     }},
            }},
      Field{TSTR("writeMask"), &Type::writeMask,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> uint8_t {
                       return {0xff};
                     }},
            }},
      Field{TSTR("failOp"), &Type::failOp,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::StencilOp {
                       return {MyGE::StencilOp::KEEP};
                     }},
            }},
      Field{TSTR("depthFailOp"), &Type::depthFailOp,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::StencilOp {
                       return {MyGE::StencilOp::KEEP};
                     }},
            }},
      Field{TSTR("passOp"), &Type::passOp,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::StencilOp {
                       return {MyGE::StencilOp::KEEP};
                     }},
            }},
      Field{TSTR("func"), &Type::func,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::CompareFunc {
                       return {MyGE::CompareFunc::ALWAYS};
                     }},
            }},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::RenderState>
    : TypeInfoBase<My::MyGE::RenderState> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[26] = "My::MyGE::RenderState";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("fillMode"), &Type::fillMode,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::FillMode {
                       return {MyGE::FillMode::SOLID};
                     }},
            }},
      Field{TSTR("cullMode"), &Type::cullMode,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::CullMode {
                       return {MyGE::CullMode::BACK};
                     }},
            }},
      Field{TSTR("zTest"), &Type::zTest,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> MyGE::CompareFunc {
                       return {MyGE::CompareFunc::LESS};
                     }},
            }},
      Field{TSTR("zWrite"), &Type::zWrite,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() -> bool {
                       return {true};
                     }},
            }},
      Field{TSTR("stencilState"), &Type::stencilState},
      Field{TSTR("blendStates"), &Type::blendStates},
      Field{TSTR("colorMask"), &Type::colorMask,
            AttrList{
                Attr{TSTR(MyMeta::initializer),
                     []() {
                       static const uint8_t value[8]{0x0f, 0x0f, 0x0f, 0x0f,
                                                     0x0f, 0x0f, 0x0f, 0x0f};
                       return value;
                     }},
            }},
  };
};
