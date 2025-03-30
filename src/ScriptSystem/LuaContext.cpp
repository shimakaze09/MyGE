#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyGE/ScriptSystem/LuaScriptQueue.h>
#include <MyLuaPP/MyLuaPP.h>

#include <mutex>
#include <set>
#include <vector>

#include "Init/InitCore.h"
#include "Init/InitMyECS.h"
#include "Init/InitMyGraphviz.h"
#include "Init/InitRender.h"
#include "LuaArray.h"
#include "LuaBuffer.h"
#include "LuaECSAgency.h"
#include "LuaMemory.h"

using namespace My::MyGE;

struct LuaContext::Impl {
  Impl() : main{Construct()} {}

  ~Impl() { Destruct(main); }

  std::mutex m;
  lua_State* main;
  std::set<lua_State*> busyLuas;
  std::vector<lua_State*> freeLuas;

  static lua_State* Construct();
  static void Destruct(lua_State* L);
};

LuaContext::LuaContext() : pImpl{new Impl} {}

LuaContext::~LuaContext() {
  Clear();
  delete pImpl;
}

lua_State* LuaContext::Main() const {
  return pImpl->main;
}

void LuaContext::Reserve(size_t n) {
  size_t num = pImpl->busyLuas.size() + pImpl->freeLuas.size();
  if (num < n) {
    for (size_t i = 0; i < n - num; i++) {
      auto L = Impl::Construct();
      pImpl->freeLuas.push_back(L);
    }
  }
}

// lock
lua_State* LuaContext::Request() {
  std::lock_guard<std::mutex> guard(pImpl->m);

  if (pImpl->freeLuas.empty()) {
    auto L = Impl::Construct();
    pImpl->freeLuas.push_back(L);
  }

  auto L = pImpl->freeLuas.back();
  pImpl->freeLuas.pop_back();
  pImpl->busyLuas.insert(L);

  return L;
}

// lock
void LuaContext::Recycle(lua_State* L) {
  std::lock_guard<std::mutex> guard(pImpl->m);

  assert(pImpl->busyLuas.find(L) != pImpl->busyLuas.end());

  pImpl->busyLuas.erase(L);
  pImpl->freeLuas.push_back(L);
}

void LuaContext::Clear() {
  assert(pImpl->busyLuas.empty());
  for (auto L : pImpl->freeLuas)
    Impl::Destruct(L);
}

class LuaArray_CmptType : public LuaArray<My::MyECS::CmptType> {};

class LuaArray_CmptAccessType : public LuaArray<My::MyECS::CmptAccessType> {};

template <>
struct My::MySRefl::TypeInfo<LuaArray_CmptType>
    : TypeInfoBase<LuaArray_CmptType, Base<LuaArray<My::MyECS::CmptType>>> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[18] = "LuaArray_CmptType";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {};
};

template <>
struct My::MySRefl::TypeInfo<LuaArray_CmptAccessType>
    : TypeInfoBase<LuaArray_CmptAccessType,
                   Base<LuaArray<My::MyECS::CmptAccessType>>> {
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
  static constexpr char name[24] = "LuaArray_CmptAccessType";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {};
};

lua_State* LuaContext::Impl::Construct() {
  lua_State* L = luaL_newstate(); /* opens Lua */
  luaL_openlibs(L);               /* opens the standard libraries */
  detail::InitCore(L);
  detail::InitRender(L);
  detail::InitMyECS(L);
  detail::InitMyGraphviz(L);
  MyLuaPP::Register<LuaArray_CmptType>(L);
  MyLuaPP::Register<LuaArray_CmptAccessType>(L);
  MyLuaPP::Register<LuaBuffer>(L);
  MyLuaPP::Register<LuaMemory>(L);
  MyLuaPP::Register<LuaECSAgency>(L);
  MyLuaPP::Register<LuaScriptQueue>(L);
  return L;
}

void LuaContext::Impl::Destruct(lua_State* L) {
  lua_close(L);
}
