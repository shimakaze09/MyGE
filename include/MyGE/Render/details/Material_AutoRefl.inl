
// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGE::Material>
    : TypeInfoBase<My::MyGE::Material, Base<My::MyGE::Object>> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[23] = "My::MyGE::Material";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("shader"), &Type::shader},
      Field{TSTR("properties"), &Type::properties},
  };
};
