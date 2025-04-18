// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGraphviz::Subgraph>
    : TypeInfoBase<My::MyGraphviz::Subgraph> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[26] = "My::MyGraphviz::Subgraph";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR(MyMeta::constructor),
            WrapConstructor<Type(MyGraphviz::Registry*, std::string)>()},
      Field{TSTR(MyMeta::destructor), WrapDestructor<Type>()},
      Field{TSTR("GetID"), &Type::GetID},
      Field{TSTR("GetSubgraph"), &Type::GetSubgraph},
      Field{TSTR("GetRegistry"), &Type::GetRegistry},
      Field{TSTR("GenSubgraph"), &Type::GenSubgraph},
      Field{TSTR("RegisterGraphAttr"), &Type::RegisterGraphAttr},
      Field{TSTR("RegisterGraphNodeAttr"), &Type::RegisterGraphNodeAttr},
      Field{TSTR("RegisterGraphEdgeAttr"), &Type::RegisterGraphEdgeAttr},
      Field{TSTR("DeregisterGraphAttr"), &Type::DeregisterGraphAttr},
      Field{TSTR("DeregisterGraphNodeAttr"), &Type::DeregisterGraphNodeAttr},
      Field{TSTR("DeregisterGraphEdgeAttr"), &Type::DeregisterGraphEdgeAttr},
      Field{TSTR("HaveNode"), &Type::HaveNode},
      Field{TSTR("HaveEdge"), &Type::HaveEdge},
      Field{TSTR("AddNode"), &Type::AddNode},
      Field{TSTR("AddEdge"), &Type::AddEdge},
      Field{TSTR("EraseNode"), &Type::EraseNode},
      Field{TSTR("EraseEdge"), &Type::EraseEdge},
  };
};
