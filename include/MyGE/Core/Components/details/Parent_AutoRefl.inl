// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template<>
struct My::MySRefl::TypeInfo<My::MyGE::Parent> :
    TypeInfoBase<My::MyGE::Parent>
{
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
    static constexpr char name[17] = "My::MyGE::Parent";
#endif
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {TSTR("value"), &Type::value, AttrList {
            Attr {TSTR(MyMeta::initializer), []()->MyECS::Entity{ return {MyECS::Entity::Invalid()}; }},
        }},
    };
};

