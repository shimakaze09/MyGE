// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::Entity>
    : My::MySRefl::TypeInfoBase<My::MyECS::Entity> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyECS::Entity(size_t, size_t)>()},
      Field{Name::constructor, WrapConstructor<My::MyECS::Entity()>()},
      Field{"Idx", &My::MyECS::Entity::Idx},
      Field{"Version", &My::MyECS::Entity::Version},
      Field{"Invalid", &My::MyECS::Entity::Invalid},
      Field{"Valid", &My::MyECS::Entity::Valid},
      Field{"operator==", &My::MyECS::Entity::operator== },
            Field{
                "operator<",
                &My::MyECS::Entity::operator<},
      };
};
