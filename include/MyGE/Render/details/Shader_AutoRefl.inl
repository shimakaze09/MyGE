// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGE::Shader>
    : My::MySRefl::TypeInfoBase<My::MyGE::Shader,
                                Base<My::MyGE::RenderRsrcObject>> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{"hlslFile", &My::MyGE::Shader::hlslFile},
      Field{"shaderName", &My::MyGE::Shader::shaderName},
      Field{"targetName", &My::MyGE::Shader::targetName},
      Field{"rootParameters", &My::MyGE::Shader::rootParameters},
      Field{"properties", &My::MyGE::Shader::properties},
      Field{"passes", &My::MyGE::Shader::passes},
  };
};
