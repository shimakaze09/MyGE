// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGE::RenderRsrcObject>
    : My::MySRefl::TypeInfoBase<My::MyGE::RenderRsrcObject> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor, WrapConstructor<My::MyGE::RenderRsrcObject()>()},
      Field{"GetInstanceID", &My::MyGE::RenderRsrcObject::GetInstanceID},
  };
};
