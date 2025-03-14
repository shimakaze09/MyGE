// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::CmptsView>
    : My::MySRefl::TypeInfoBase<My::MyECS::CmptsView> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyECS::CmptsView(const My::MyECS::CmptPtr *,
                                                 size_t)>(),
            AttrList{
                Attr{MY_MYSREFL_NAME_ARG(0),
                     AttrList{
                         Attr{Name::name, "cmpts"},
                     }},
                Attr{MY_MYSREFL_NAME_ARG(1),
                     AttrList{
                         Attr{Name::name, "num"},
                     }},
            }},
      Field{"GetCmpt", &My::MyECS::CmptsView::GetCmpt,
            AttrList{
                Attr{MY_MYSREFL_NAME_ARG(0)},
            }},
      Field{"Components", &My::MyECS::CmptsView::Components},
      Field{"NumberOfComponents", &My::MyECS::CmptsView::NumberOfComponents},
  };
};
