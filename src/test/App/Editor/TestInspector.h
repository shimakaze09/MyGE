#pragma once

#include <MyECS/Entity.h>
#include <MyGM/MyGM.h>
#include <MySRefl/MySRefl.h>

#include <variant>

namespace My::MyGE {
enum class TestEnum { A, B, C };

struct TestInspector {
  bool v_bool;
  uint8_t v_uint8;
  uint16_t v_uint16;
  uint32_t v_uint32;
  uint64_t v_uint64;
  int8_t v_int8;
  int16_t v_int16;
  int32_t v_int32;
  int64_t v_int64;
  // void* v_nullptr;
  float v_float;
  double v_double;
  std::string v_string;
  MyECS::Entity v_entity{MyECS::Entity::Invalid()};
  // const HLSLFile* v_hlslFile;
  std::array<int, 3> v_array;
  std::array<std::array<float, 2>, 3> v_array2;
  bboxf3 v_bbox;
  vecf3 v_vec;
  rgbf v_rgb;
  rgbaf v_rgba;
  std::vector<std::string> v_vector{"abc", "edf"};
  std::deque<size_t> v_deque;
  std::forward_list<size_t> v_forward_list;
  std::list<size_t> v_list;
  // std::set<size_t> v_set;
  // std::multiset<size_t> v_multiset;
  // std::unordered_set<size_t> v_unordered_set;
  // std::unordered_multiset<size_t> v_unordered_multiset;
  std::map<std::string, std::array<int, 3>> v_map{{"a", {1, 2, 3}},
                                                  {"b", {4, 5, 6}}};
  // std::multimap<size_t, std::string> v_multimap;
  // std::unordered_map<std::string, std::string> v_unordered_map;
  // std::unordered_multimap<std::string, std::string> v_unordered_multimap;
  std::tuple<size_t, bool, float> v_tuple;
  std::pair<size_t, bool> v_pair;
  std::variant<size_t, std::string> v_variant{std::string{"I'm varaint"}};
  // std::vector<Entity> v_vector_entity;
  // UserType v_usertype;
  TestEnum v_enum{TestEnum::B};
  [[MyInspector::hide]]
  int v_hide{1};
  [[MyInspector::name("another name")]]
  int v_name{1};
  [[MyInspector::header("this is a header")]]
  int v_header{1};
  [[MyInspector::tooltip("this is a tooltip")]]
  int v_tooltip{1};
  [[MyInspector::min_value(-5)]]
  int v_int_min_value{1};
  [[MyInspector::min_value(5u)]]
  uint32_t v_uint32_min_value{1};
  [[MyInspector::min_value(-0.5f)]]
  float v_float_min_value{1};
  [[MyInspector::range(std::pair{-10, 10})]]
  int v_int_range{1};
  [[MyInspector::range(std::pair{0, 10})]]
  uint32_t v_uint32_range{1};
  [[MyInspector::range(std::pair{-0.1f, 0.1f})]]
  float v_float_range{1};
  [[MyInspector::step(1.5f)]]
  float v_float_step{1};

  [[using MyInspector: name("[attrs] name"), header("[attrs] header"),
    tooltip("[attrs] tooltip"), step(2.f), min_value(1.5f)]]
  float v_attrs{1};
};
}  // namespace My::MyGE

template <>
struct My::MySRefl::TypeInfo<My::MyGE::TestEnum>
    : TypeInfoBase<My::MyGE::TestEnum> {
#ifdef My_MySRefl_NOT_USE_NAMEOF
  static constexpr char name[23] = "My::MyGE::TestEnum";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
      Field{TSTR("A"), Type::A},
      Field{TSTR("B"), Type::B},
      Field{TSTR("C"), Type::C},
  };
};

template <>
struct My::MySRefl::TypeInfo<My::MyGE::TestInspector>
    : TypeInfoBase<My::MyGE::TestInspector> {
#ifdef My_MySRefl_NOT_USE_NAMEOF
  static constexpr char name[28] = "My::MyGE::TestInspector";
#endif
  static constexpr AttrList attrs = {};
  static constexpr FieldList fields = {
        Field {TSTR("v_bool"), &Type::v_bool},
        Field {TSTR("v_uint8"), &Type::v_uint8},
        Field {TSTR("v_uint16"), &Type::v_uint16},
        Field {TSTR("v_uint32"), &Type::v_uint32},
        Field {TSTR("v_uint64"), &Type::v_uint64},
        Field {TSTR("v_int8"), &Type::v_int8},
        Field {TSTR("v_int16"), &Type::v_int16},
        Field {TSTR("v_int32"), &Type::v_int32},
        Field {TSTR("v_int64"), &Type::v_int64},
        Field {TSTR("v_float"), &Type::v_float},
        Field {TSTR("v_double"), &Type::v_double},
        Field {TSTR("v_string"), &Type::v_string},
        Field {TSTR("v_entity"), &Type::v_entity, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->MyECS::Entity { return { MyECS::Entity::Invalid() }; }},
        }},
        Field {TSTR("v_array"), &Type::v_array},
        Field {TSTR("v_array2"), &Type::v_array2},
        Field {TSTR("v_bbox"), &Type::v_bbox},
        Field {TSTR("v_vec"), &Type::v_vec},
        Field {TSTR("v_rgb"), &Type::v_rgb},
        Field {TSTR("v_rgba"), &Type::v_rgba},
        Field {TSTR("v_vector"), &Type::v_vector, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->std::vector<std::string> { return { "abc","edf" }; }},
        }},
        Field {TSTR("v_deque"), &Type::v_deque},
        Field {TSTR("v_forward_list"), &Type::v_forward_list},
        Field {TSTR("v_list"), &Type::v_list},
        Field {TSTR("v_map"), &Type::v_map, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->std::map<std::string, std::array<int, 3> > { return { {"a", {1,2,3}}, {"b", {4,5,6}} }; }},
        }},
        Field {TSTR("v_tuple"), &Type::v_tuple},
        Field {TSTR("v_pair"), &Type::v_pair},
        Field {TSTR("v_variant"), &Type::v_variant, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->std::variant<size_t, std::string> { return { std::string{"I'm varaint"} }; }},
        }},
        Field {TSTR("v_enum"), &Type::v_enum, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->My::MyGE::TestEnum { return { My::MyGE::TestEnum::B }; }},
        }},
        Field {TSTR("v_hide"), &Type::v_hide, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->int { return { 1 }; }},
            Attr {TSTR(MyInspector::hide)},
        }},
        Field {TSTR("v_name"), &Type::v_name, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->int { return { 1 }; }},
            Attr {TSTR(MyInspector::name), "another name"},
        }},
        Field {TSTR("v_header"), &Type::v_header, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->int { return { 1 }; }},
            Attr {TSTR(MyInspector::header), "this is a header"},
        }},
        Field {TSTR("v_tooltip"), &Type::v_tooltip, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->int { return { 1 }; }},
            Attr {TSTR(MyInspector::tooltip), "this is a tooltip"},
        }},
        Field {TSTR("v_int_min_value"), &Type::v_int_min_value, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->int { return { 1 }; }},
            Attr {TSTR(MyInspector::min_value), -5},
        }},
        Field {TSTR("v_uint32_min_value"), &Type::v_uint32_min_value, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->uint32_t { return { 1 }; }},
            Attr {TSTR(MyInspector::min_value), 5u},
        }},
        Field {TSTR("v_float_min_value"), &Type::v_float_min_value, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->float { return { 1 }; }},
            Attr {TSTR(MyInspector::min_value), -0.5f},
        }},
        Field {TSTR("v_int_range"), &Type::v_int_range, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->int { return { 1 }; }},
            Attr {TSTR(MyInspector::range), std::pair{-10,10}},
        }},
        Field {TSTR("v_uint32_range"), &Type::v_uint32_range, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->uint32_t { return { 1 }; }},
            Attr {TSTR(MyInspector::range), std::pair{0,10}},
        }},
        Field {TSTR("v_float_range"), &Type::v_float_range, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->float { return { 1 }; }},
            Attr {TSTR(MyInspector::range), std::pair{-0.1f,0.1f}},
        }},
        Field {TSTR("v_float_step"), &Type::v_float_step, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->float { return { 1 }; }},
            Attr {TSTR(MyInspector::step), 1.5f},
        }},
        Field {TSTR("v_attrs"), &Type::v_attrs, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->float { return { 1 }; }},
            Attr {TSTR(MyInspector::name), "[attrs] name"},
            Attr {TSTR(MyInspector::header), "[attrs] header"},
            Attr {TSTR(MyInspector::tooltip), "[attrs] tooltip"},
            Attr {TSTR(MyInspector::step), 2.f},
            Attr {TSTR(MyInspector::min_value), 1.5f},
        }},
    };
};
