// This file is generated by My::MySRefl::AutoRefl

#pragma once

#include <MySRefl/MySRefl.h>

template<>
struct My::MySRefl::TypeInfo<My::MyGE::Name> :
    TypeInfoBase<My::MyGE::Name>
{
#ifdef MY_MYSREFL_NOT_USE_NAMEOF
    static constexpr char name[15] = "My::MyGE::Name";
#endif
    static constexpr AttrList attrs = {};
    static constexpr FieldList fields = {
        Field {TSTR("value"), &Type::value},
    };
};

