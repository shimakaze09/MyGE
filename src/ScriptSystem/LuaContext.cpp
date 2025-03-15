#include <MyGE/ScriptSystem/LuaContext.h>
#include <MyLuaPP/MyLuaPP.h>

#include <mutex>
#include <set>
#include <vector>

#include "InitMyECS.h"
#include "InitUGraphviz.h"
#include "LuaArray.h"
#include "LuaBuffer.h"
#include "LuaMemory.h"
#include "LuaSystem.h"

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

LuaContext::~LuaContext() { delete pImpl; }

lua_State* LuaContext::Main() const { return pImpl->main; }

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
  for (auto L : pImpl->freeLuas) Impl::Destruct(L);
  delete pImpl;
}

class LuaArray_CmptType : public LuaArray<My::MyECS::CmptType> {};
template <>
struct My::MySRefl::TypeInfo<LuaArray_CmptType>
    : My::MySRefl::TypeInfoBase<LuaArray_CmptType,
                                Base<LuaArray<My::MyECS::CmptType>>> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {};
};

lua_State* LuaContext::Impl::Construct() {
  lua_State* L = luaL_newstate(); /* opens Lua */
  luaL_openlibs(L);               /* opens the standard libraries */
  detail::InitMyECS(L);
  detail::InitUGraphviz(L);
  MyLuaPP::Register<LuaArray_CmptType>(L);
  MyLuaPP::Register<LuaBuffer>(L);
  MyLuaPP::Register<LuaMemory>(L);
  MyLuaPP::Register<LuaSystem>(L);
  return L;
}

void LuaContext::Impl::Destruct(lua_State* L) { lua_close(L); }
