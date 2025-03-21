#include "InitMyGraphviz.h"
#include "details/MyGraphviz_AutoRefl/MyGraphviz_AutoRefl.h"

void My::MyGE::detail::InitMyGraphviz(lua_State* L) {
  MyLuaPP::Register<MyGraphviz::Graph>(L);
  MyLuaPP::Register<MyGraphviz::Registry>(L);
  MyLuaPP::Register<MyGraphviz::Subgraph>(L);
}
