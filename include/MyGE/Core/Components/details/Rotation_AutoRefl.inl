// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template<>
struct My::MySRefl::TypeInfo<My::MyGE::Rotation> :
    TypeInfoBase<My::MyGE::Rotation>
{
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
    static constexpr char name[19] = "My::MyGE::Rotation";
#endif
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {TSTR("value"), &Type::value, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->quatf{ return {quatf::identity()}; }},
        }},
    };
};

