//
// Created by Admin on 14/03/2025.
//

#include <MyECS/CmptType.h>
#include <MyGE/ScriptSystem/LuaMngr.h>

#include <mutex>

#include "InitMyECS.h"
#include "LuaArray.h"
#include "LuaBuffer.h"
#include "LuaMemory.h"
#include "LuaSystem.h"

using namespace My::MyGE;

struct LuaMngr::Impl {
  std::mutex m;
  std::set<lua_State*> busyLuas;
  std::vector<lua_State*> freeLuas;

  static lua_State* Construct();
  static void Destruct(lua_State* L);
};

void LuaMngr::Init() { pImpl = new LuaMngr::Impl; }

void LuaMngr::Reserve(size_t n) {
  size_t num = pImpl->busyLuas.size() + pImpl->freeLuas.size();
  if (num < n) {
    for (size_t i = 0; i < n - num; i++) {
      auto L = Impl::Construct();
      pImpl->freeLuas.push_back(L);
    }
  }
}

lua_State* LuaMngr::Request() {
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

void LuaMngr::Recycle(lua_State* L) {
  std::lock_guard<std::mutex> guard(pImpl->m);

  assert(pImpl->busyLuas.find(L) != pImpl->busyLuas.end());

  pImpl->busyLuas.erase(L);
  pImpl->freeLuas.push_back(L);
}

void LuaMngr::Clear() {
  assert(pImpl->busyLuas.empty());
  for (auto L : pImpl->freeLuas) Impl::Destruct(L);
  delete pImpl;
}

// ================================

class LuaArray_CmptType : public LuaArray<My::MyECS::CmptType> {};

template <>
struct My::MySRefl::TypeInfo<LuaArray_CmptType>
    : My::MySRefl::TypeInfoBase<LuaArray_CmptType,
                                Base<LuaArray<My::MyECS::CmptType>>> {
  static constexpr AttrList attrs = {};

  static constexpr FieldList fields = {};
};

lua_State* LuaMngr::Impl::Construct() {
  lua_State* L = luaL_newstate(); /* opens Lua */
  luaL_openlibs(L);               /* opens the standard libraries */
  detail::InitMyECS(L);
  MyLuaPP::Register<LuaArray_CmptType>(L);
  MyLuaPP::Register<LuaBuffer>(L);
  MyLuaPP::Register<LuaMemory>(L);
  MyLuaPP::Register<LuaSystem>(L);
  return L;
}

void LuaMngr::Impl::Destruct(lua_State* L) { lua_close(L); }
