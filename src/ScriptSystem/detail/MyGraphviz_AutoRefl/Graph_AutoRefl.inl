// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGraphviz::Graph>
    : My::MySRefl::TypeInfoBase<My::MyGraphviz::Graph,
                                Base<My::MyGraphviz::Subgraph>> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor,
            WrapConstructor<My::MyGraphviz::Graph(std::string, bool)>(),
            AttrList{
                Attr{MY_MYSREFL_NAME_ARG(0),
                     AttrList{
                         Attr{Name::name, "id"},
                     }},
                Attr{MY_MYSREFL_NAME_ARG(1),
                     AttrList{
                         Attr{Name::name, "isDigraph"},
                         Attr{"Name::default_value", false},
                     }},
            }},
      Field{Name::destructor, WrapDestructor<My::MyGraphviz::Graph>()},
      /*Field{Name::constructor,
      WrapConstructor<My::MyGraphviz::Graph(My::MyGraphviz::Graph &&)>(),
              AttrList {
                      Attr{MY_MYSREFL_NAME_ARG(0)},
              }
      },*/
      /*Field{"operator=", &My::MyGraphviz::Graph::operator=,
              AttrList {
                      Attr{MY_MYSREFL_NAME_ARG(0)},
              }
      },*/
      Field{"Dump", &My::MyGraphviz::Graph::Dump},
      Field{"IsDigraph", &My::MyGraphviz::Graph::IsDigraph},
  };
};
