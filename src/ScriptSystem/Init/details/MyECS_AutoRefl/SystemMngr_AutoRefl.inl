// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::SystemMngr>
    : TypeInfoBase<My::MyECS::SystemMngr> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[23] = "My::MyECS::SystemMngr";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("systemTraits"), &Type::systemTraits},
      Field{TSTR(MyMeta::constructor), WrapConstructor<Type(MyECS::World*)>()},
      Field{TSTR(MyMeta::constructor),
            WrapConstructor<Type(const MyECS::SystemMngr&, MyECS::World*)>()},
      Field{TSTR(MyMeta::constructor),
            WrapConstructor<Type(MyECS::SystemMngr&&, MyECS::World*)>()},
      Field{TSTR(MyMeta::destructor), WrapDestructor<Type>()},
      Field{TSTR("GetAliveSystemIDs"), &Type::GetAliveSystemIDs},
      Field{TSTR("GetActiveSystemsIDs"), &Type::GetActiveSystemsIDs},
      Field{TSTR("Create"), static_cast<void (Type::*)(size_t)>(&Type::Create)},
      Field{TSTR("Activate"),
            static_cast<void (Type::*)(size_t)>(&Type::Activate)},
      Field{TSTR("Deactivate"),
            static_cast<void (Type::*)(size_t)>(&Type::Deactivate)},
      Field{TSTR("Destroy"),
            static_cast<void (Type::*)(size_t)>(&Type::Destroy)},
      Field{TSTR("IsAlive"),
            static_cast<bool (Type::*)(size_t) const>(&Type::IsAlive)},
      Field{TSTR("IsActive"),
            static_cast<bool (Type::*)(size_t) const>(&Type::IsActive)},
  };
};
