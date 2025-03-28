// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyECS::SystemMngr>
    : My::MySRefl::TypeInfoBase<My::MyECS::SystemMngr> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor, WrapConstructor<My::MyECS::SystemMngr()>()},
      Field{Name::constructor,
            WrapConstructor<My::MyECS::SystemMngr(My::MyECS::SystemMngr&)>()},
      Field{"GetSystems", &My::MyECS::SystemMngr::GetSystems},
      Field{"GetActiveSystemIndices",
            &My::MyECS::SystemMngr::GetActiveSystemIndices},
      Field{"GetNameToIndexMap", &My::MyECS::SystemMngr::GetNameToIndexMap},
      Field{"GetIndex",
            static_cast<size_t (My::MyECS::SystemMngr::*)(std::string_view)
                            const>(&My::MyECS::SystemMngr::GetIndex)},
      Field{"Clear", &My::MyECS::SystemMngr::Clear},
      Field{"Register", static_cast<size_t (My::MyECS::SystemMngr::*)(
                            std::string, My::MyECS::SystemMngr::Func)>(
                            &My::MyECS::SystemMngr::Register)},
      Field{"Unregister", static_cast<void (My::MyECS::SystemMngr::*)(size_t)>(
                              &My::MyECS::SystemMngr::Unregister)},
      Field{"Unregister",
            static_cast<void (My::MyECS::SystemMngr::*)(std::string_view)>(
                &My::MyECS::SystemMngr::Unregister)},
      Field{"Activate", &My::MyECS::SystemMngr::Activate},
      Field{"Deactivate", &My::MyECS::SystemMngr::Deactivate},
  };
};
