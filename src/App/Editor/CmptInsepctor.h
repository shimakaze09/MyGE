//
// Created by Admin on 19/03/2025.
//

#pragma once
 
 #include <MyECS/CmptPtr.h>
 
 #include <functional>
 
 #include <MyDP/Visitor/ncVisitor.h>
 
 namespace My::MyGE {
 class CmptInspector {
 public:
   static CmptInspector& Instance() noexcept {
     static CmptInspector instance;
     return instance;
   }
 
   struct InspectContext {
     const Visitor<void(void*, InspectContext)>& inspector;
   };
 
   void RegisterCmpt(MyECS::CmptType type, std::function<void(void*, InspectContext)> cmptInspectFunc);
 
   template<typename Func>
   void RegisterCmpt(Func&& func);
 
   template<typename... Cmpts>
   void RegisterCmpts();
 
   bool IsCmptRegistered(MyECS::CmptType type) const;
 
   void Inspect(MyECS::CmptPtr cmpt);
 
 private:
   CmptInspector();
   ~CmptInspector();
   struct Impl;
   Impl* pImpl;
 };
 }
 
#include "details/CmptInsepctor.inl"
