// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::CmptPtr>
    : My::MySRefl::TypeInfoBase<My::MyECS::CmptPtr> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyECS::CmptPtr(My::MyECS::CmptType, void *)>(),
            AttrList{
                Attr{MY_MYSRefl_NAME_ARG(0),
                     AttrList{
                         Attr{Name::name, "type"},
                     }},
                Attr{MY_MYSRefl_NAME_ARG(1),
                     AttrList{
                         Attr{Name::name, "p"},
                     }},
            }},
      Field{"Type", &My::MyECS::CmptPtr::Type},
      Field{"Ptr", &My::MyECS::CmptPtr::Ptr},
  };
};
