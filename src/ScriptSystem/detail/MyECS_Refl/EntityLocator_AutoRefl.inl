// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::EntityLocator>
    : My::MySRefl::TypeInfoBase<My::MyECS::EntityLocator> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyECS::EntityLocator(My::MyECS::CmptType*,
                                                     size_t)>(),
            AttrList{
                Attr{MY_MYSREFL_NAME_ARG(0),
                     AttrList{Attr{Name::name, "lastFrameCmpts"}}},
                Attr{MY_MYSREFL_NAME_ARG(1),
                     AttrList{Attr{Name::name, "writeFrameCmpts"}}},
                Attr{MY_MYSREFL_NAME_ARG(2),
                     AttrList{Attr{Name::name, "latestCmpts"}}},
            }},
      Field{"HashCode", &My::MyECS::EntityLocator::HashCode},
      Field{"LastFrameCmptTypes",
            &My::MyECS::EntityLocator::LastFrameCmptTypes},
      Field{"WriteCmptTypes", &My::MyECS::EntityLocator::WriteCmptTypes},
      Field{"LatestCmptTypes", &My::MyECS::EntityLocator::LatestCmptTypes},
      Field{"CmptTypes", &My::MyECS::EntityLocator::CmptTypes},
      Field{"GetCmptTagMode", &My::MyECS::EntityLocator::GetCmptTagMode,
            AttrList{
                Attr{MY_MYSREFL_NAME_ARG(0),
                     AttrList{
                         Attr{Name::name, "type"},
                     }},
            }},
      Field{"operator==", &My::MyECS::EntityLocator::operator==,
            AttrList{
                Attr{MY_MYSREFL_NAME_ARG(0),
                     AttrList{
                         Attr{Name::name, "locator"},
                     }},
            }},
  };
};
