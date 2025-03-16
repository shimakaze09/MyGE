// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template <>
struct My::MySRefl::TypeInfo<My::MyGraphviz::Registry>
    : My::MySRefl::TypeInfoBase<My::MyGraphviz::Registry> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {
      Field{Name::constructor, WrapConstructor<My::MyGraphviz::Registry()>()},
      Field{"GetNodes", &My::MyGraphviz::Registry::GetNodes},
      Field{"GetEdges", &My::MyGraphviz::Registry::GetEdges},
      Field{"GetNodeAttrs", &My::MyGraphviz::Registry::GetNodeAttrs},
      Field{"GetEdgeAttrs", &My::MyGraphviz::Registry::GetEdgeAttrs},
      Field{"IsRegisteredNode", &My::MyGraphviz::Registry::IsRegisteredNode},
      Field{
          "IsRegisteredEdge",
          static_cast<bool (My::MyGraphviz::Registry::*)(size_t, size_t) const>(
              &My::MyGraphviz::Registry::IsRegisteredEdge)},
      Field{"IsRegisteredEdge",
            static_cast<bool (My::MyGraphviz::Registry::*)(
                std::string_view, std::string_view) const>(
                &My::MyGraphviz::Registry::IsRegisteredEdge)},
      Field{"GetNodeIndex", &My::MyGraphviz::Registry::GetNodeIndex},
      Field{"GetEdgeIndex", &My::MyGraphviz::Registry::GetEdgeIndex},
      Field{"RegisterNode", &My::MyGraphviz::Registry::RegisterNode},
      Field{"RegisterEdge", &My::MyGraphviz::Registry::RegisterEdge},
      Field{"RegisterNodeAttr", &My::MyGraphviz::Registry::RegisterNodeAttr},
      Field{"RegisterEdgeAttr", &My::MyGraphviz::Registry::RegisterEdgeAttr},
      Field{"DeregisterNodeAttr",
            &My::MyGraphviz::Registry::DeregisterNodeAttr},
      Field{"DeregisterEdgeAttr",
            &My::MyGraphviz::Registry::DeregisterEdgeAttr},
  };
};
