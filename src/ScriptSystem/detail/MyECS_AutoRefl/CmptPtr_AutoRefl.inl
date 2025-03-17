// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::CmptPtr>
    : My::MySRefl::TypeInfoBase<My::MyECS::CmptPtr> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyECS::CmptPtr(My::MyECS::CmptType, void*)>()},
      Field{"Ptr", &My::MyECS::CmptPtr::Ptr},
      Field{"Type", &My::MyECS::CmptPtr::Type},
      Field{"Invalid", &My::MyECS::CmptPtr::Invalid},
      Field{"Valid", &My::MyECS::CmptPtr::Valid},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyECS::CmptAccessPtr>
    : My::MySRefl::TypeInfoBase<My::MyECS::CmptAccessPtr> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyECS::CmptAccessPtr(My::MyECS::CmptType, void*,
                                                     My::MyECS::AccessMode)>()},
      Field{Name::constructor, WrapConstructor<My::MyECS::CmptAccessPtr(
                                   My::MyECS::CmptAccessType, void*)>()},
      Field{Name::constructor, WrapConstructor<My::MyECS::CmptPtr()>()},
      Field{Name::constructor,
            WrapConstructor<My::MyECS::CmptAccessPtr(My::MyECS::CmptPtr,
                                                     My::MyECS::AccessMode)>()},
      Field{Name::constructor,
            WrapConstructor<My::MyECS::CmptAccessPtr(My::MyECS::CmptPtr)>()},
      Field{Name::constructor, WrapConstructor<My::MyECS::CmptAccessPtr()>()},
      Field{"Ptr", &My::MyECS::CmptAccessPtr::Ptr},
      Field{"AccessType", &My::MyECS::CmptAccessPtr::AccessType},
      Field{"Invalid", &My::MyECS::CmptAccessPtr::Invalid},
      Field{"Valid", &My::MyECS::CmptAccessPtr::Valid},
  };
};
