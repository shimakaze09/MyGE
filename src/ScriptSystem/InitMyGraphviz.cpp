//
// Created by Admin on 14/03/2025.
//

#include "InitMyGraphviz.h"
 
 #include "detail/MyGraphviz_AutoRefl/MyGraphviz_AutoRefl.h"
 
 void My::MyGE::detail::InitMyGraphviz(lua_State* L) {
   MyLuaPP::Register<MyGraphviz::Graph>(L);
   MyLuaPP::Register<MyGraphviz::Registry>(L);
   MyLuaPP::Register<MyGraphviz::Subgraph>(L);
 }
