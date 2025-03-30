// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::SingletonLocator>
    : TypeInfoBase<My::MyECS::SingletonLocator> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[29] = "My::MyECS::SingletonLocator";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR(MyMeta::constructor),
            WrapConstructor<Type(std::set<MyECS::CmptAccessType>)>()},
      Field{TSTR(MyMeta::constructor),
            WrapConstructor<Type(Span<const MyECS::CmptAccessType>)>()},
      Field{TSTR(MyMeta::constructor), WrapConstructor<Type()>()},
      Field{TSTR("SingletonTypes"), &Type::SingletonTypes},
      Field{TSTR("HasWriteSingletonType"), &Type::HasWriteSingletonType},
  };
};
