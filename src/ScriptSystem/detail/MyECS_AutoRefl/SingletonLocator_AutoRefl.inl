// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::SingletonLocator>
    : My::MySRefl::TypeInfoBase<My::MyECS::SingletonLocator> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyECS::SingletonLocator(
                const My::MyECS::CmptAccessType *, size_t)>()},
      Field{Name::constructor,
            WrapConstructor<My::MyECS::SingletonLocator()>()},
      Field{"SingletonTypes", &My::MyECS::SingletonLocator::SingletonTypes},
  };
};
