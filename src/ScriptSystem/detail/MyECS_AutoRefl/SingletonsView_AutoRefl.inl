// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::SingletonsView>
    : My::MySRefl::TypeInfoBase<My::MyECS::SingletonsView> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor, WrapConstructor<My::MyECS::SingletonsView(
                                   const My::MyECS::CmptPtr *, size_t)>()},
      Field{"GetSingleton", &My::MyECS::SingletonsView::GetSingleton},
      Field{"Singletons", &My::MyECS::SingletonsView::Singletons},
      Field{"NumberOfSingletons",
            &My::MyECS::SingletonsView::NumberOfSingletons},
  };
};
